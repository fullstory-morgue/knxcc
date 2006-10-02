#include <qspinbox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qwidgetstack.h>
#include <qmessagebox.h>

#include <kdialog.h>
#include <klocale.h>
#include <kpassdlg.h>

#include <unistd.h> // for getuid()

#include "knxcc_network_interface.h"

knxcc_network_interface::knxcc_network_interface(QWidget *parent, const char *name, const QStringList &)
:InterfaceWidget(parent, name)
{
    this->shell = new Process();
    this->hasChanged = false;
        
    hexValidator = new QHexValidator(this);
    ipValidator = new IpAddressValidator(this);
    
    staticIP->setValidator( ipValidator );
    staticNetmask->setValidator( ipValidator );
    staticBroadcast->setValidator( ipValidator );
    staticGateway->setValidator( ipValidator );
    
    wepKeyLenChanged();
    
    if (getuid() != 0)
    {
        staticIP->setDisabled(true);
        staticNetmask->setDisabled(true);
        staticBroadcast->setDisabled(true);
        staticGateway->setDisabled(true);
        useDHCP->setDisabled(true);
        useStatic->setDisabled(true);
        
        ifaceTabWidget->setDisabled(true);
        wlanConfTab->setDisabled(true);
        wlanSecurityTab->setDisabled(true);
    }
}

void knxcc_network_interface::load(QString interface)
{
    // store interface
    this->interface = interface;
    
    // get new Values
    this->shell->setCommand(QString("knxcc network print_interface %1").arg(interface));
    this->shell->start(true);
    QStringList valueList = QStringList::split( "\n", this->shell->getBuffer() );
    QStringList::Iterator it = valueList.begin();
    // interface infos
    QString iface = (*it++).mid(1);
    QString driver = (*it++).mid(1);
    QString medium = (*it++).mid(1);
    QString bus = (*it++).mid(1);
    QString mac = (*it++).mid(1);
    QString desc = (*it++).mid(1);
    // interface modes (auto, allow-*)
    QString modes = (*it++).mid(1);
    // ip-config
    QString method = (*it++).mid(1);
    QString address = (*it++).mid(1);
    QString netmask = (*it++).mid(1);
    QString broadcast = (*it++).mid(1);
    QString gateway = (*it++).mid(1);
    // wlan
    QString essid = (*it++).mid(1);
    QString nwid = (*it++).mid(1);
    QString mode = (*it++).mid(1);
    QString channel = (*it++).mid(1);
    QString freq = (*it++).mid(1);
    QString sens = (*it++).mid(1);
    QString rate = (*it++).mid(1);
    QString rts = (*it++).mid(1);
    QString frag = (*it++).mid(1);
    QString iwconfig = (*it++).mid(1);
    QString iwspy = (*it++).mid(1);
    QString iwpriv = (*it++).mid(1);
    QString wpadriver = (*it++).mid(1);
    QString wpakey = (*it++).mid(1);
    QString wepkey = (*it++).mid(1);
    QString pubkey = (*it++).mid(1);
    
    // set interfaceDetails
    interfaceDetails->setText(i18n("<b>%1</b> (%2, %3) uses driver: %4, MAC-address: %5\n<br><b>Interface description</b>: %6").arg(iface).arg(medium).arg(bus).arg(driver).arg(mac).arg(desc));
    
    // set interface modes
    autoEnable->setChecked(false);
    hotplugEnable->setChecked(false);
    if(modes.contains("auto")) autoEnable->setChecked(true);
    if(modes.contains("allow-hotplug")) hotplugEnable->setChecked(true);
    
    // set ip-config
    if( method == QString("static") ) useStatic->setChecked(true);
    else if( method == QString("dhcp") ) useDHCP->setChecked(true);
    staticIP->setText(address);
    staticNetmask->setText(netmask);
    staticBroadcast->setText(broadcast);
    staticGateway->setText(gateway);
    
    // set wlan-stuff
    if( medium != QString("Wireless") )
    {
        configTabWidget->removePage( wlanConfTab );
        configTabWidget->removePage( wlanSecurityTab );
        this->isWireless = false;
    }
    else
    {
        this->isWireless = true;
        wirelessEssid->setText(essid);
        // wireless mode
        if( mode == "Managed" )
                wirelessMode->setCurrentItem(1);
        else if( mode == "Ad-Hoc" )
                wirelessMode->setCurrentItem(2);
        else if( mode == "Master" )
                wirelessMode->setCurrentItem(3);
        else if( mode == "Repeater" )
                wirelessMode->setCurrentItem(4);
        else if( mode == "Secondary" )
                wirelessMode->setCurrentItem(5);
        else if( mode == "Auto" )
                wirelessMode->setCurrentItem(6);
        else
                wirelessMode->setCurrentItem(0);
        // encryption
        if( wepkey.isEmpty() && wpakey.isEmpty())
            enableEncryption->setChecked(false);
        else
        {
            if( pubkey != QString("0") )
            {
                // WEP
                encryptionType->setCurrentItem( 0 );
                encryptionTypeStack->raiseWidget( 0 );
                
                if( pubkey == QString("1") ) storePublic->setChecked(true);
                else if( pubkey == QString("-1") ) storePrivate->setChecked(true);
                
                if( wepkey.startsWith("s:") )
                {
                    // WEP ASCII
                    wepKey->setText(wepkey.mid(2));
                    wepKeyAscii->setChecked(true);
                }
                else
                {
                    // WEP Hex
                    wepKey->setText(wepkey);
                    wepKeyHex->setChecked(true);
                }
            }
            else
            {
                // WPA
                encryptionType->setCurrentItem( 1 );
                encryptionTypeStack->raiseWidget( 1 );
                wpaKey->setText(wpakey);
            }
        }
        if( wpadriver.isEmpty() )
        {
            // NOTE: if a wpakey is set for the device (could be done manually) and "wpadriver" is empty and pubkey = 0 we might have a problem here...
            encryptionType->removeItem( 1 );
            encryptionTypeStack->raiseWidget( 0 );
        }
        // advanced options
        advNwid->setText(nwid);
        advIwconfig->setText(iwconfig);
        advIwspy->setText(iwspy);
        advIwpriv->setText(iwpriv);
        // channel/ frequency
        if( channel )
        {
            wirelessChanChanRadio->setChecked(true);
            wirelessChan->setValue(channel.toInt());
        }
        else if( freq )
        {
            wirelessChanFreqRadio->setChecked(true);
            wirelessFreq->setText(freq);
        }
        else
        {
            wirelessChanAutoRadio->setChecked(true);
        }
    }
    this->hasChanged = false;
}

void knxcc_network_interface::changed()
{
    this->hasChanged = true;
    emit changed(true);
}

void knxcc_network_interface::wepUseAscii(bool yesno)
{
    if(!yesno) return;
    
    QString asciiKey = "";
    QString str = wepKey->text().replace('-',"");
    for(unsigned int i=0; i<str.length(); i+=2)
    {
        QChar asciiChar = str.mid(i, 2).toInt(0, 16);
        asciiKey+=QString(asciiChar);
    }
    
    wepKeyLenChanged();
    wepKey->setText(asciiKey);
    wepKey->setValidator(0);
}

void knxcc_network_interface::wepUseHex(bool yesno)
{
    if(!yesno) return;
    
    QString hexKey = "";
    QString str = wepKey->text();
    for(unsigned int i=0; i<str.length(); i++)
    {
        QString hexNr;
        int x = (int)str.at(i).latin1();
        if( x == '~' ) x = '0';
        hexNr.sprintf("%02X", x);
        hexKey+=hexNr;
    }
    
    wepKeyLenChanged();
    wepKey->setText(hexKey);
    wepKey->setValidator(hexValidator);
}

void knxcc_network_interface::wepKeyLenChanged()
{
    if( wepKeyAscii->isChecked() )
    switch( wepBits->currentItem() )
    {
        case 0:
            wepKey->setInputMask("XXXXX;~");
            break;
        case 1:
            wepKey->setInputMask("XXXXXXXXXXXXX;~");
            break;
        case 2:
            wepKey->setInputMask("XXXXXXXXXXXXXXXX;~");
            break;
        case 3:
            wepKey->setInputMask("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX;~");
            break;
    }
    else if( wepKeyHex->isChecked() )
    switch( wepBits->currentItem() )
    {
        case 0:
            wepKey->setInputMask(">NNNN-NNNN-NN;#");
            break;
        case 1:
            wepKey->setInputMask(">NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NN;#");
            break;
        case 2:
            wepKey->setInputMask(">NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN;#");
            break;
        case 3:
            wepKey->setInputMask(">NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NNNN-NN;#");
            break;
    }
}

void knxcc_network_interface::wpaKeyLenCheck()
{
    if(wpaKey->text().length() < 8 && wpaKey->text().length() > 0 && enableEncryption->isChecked())
    {
        if( QMessageBox::warning( this, i18n( "WPA-key length error" ), i18n( "Your WPA-key has to be at least 8 characters long.\n"
        "Do you want to correct your input?\n\n"
        "(if not WPA-encryption will be disabled)" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
        {
            wpaKey->setFocus();
        }
        else
        {
            enableEncryption->setChecked(false);
            wpaKey->setText("");
        }
    }
}

bool knxcc_network_interface::hasChanges()
{
    if(this->hasChanged) return true;
    
    return false;
}

void knxcc_network_interface::saveChanges()
{
    if(!this->hasChanged) return;
    
    QString cmdargs = " set_interface_config "+this->interface;
    // Method (dhcp/static/"")
    if(useDHCP->isChecked()) cmdargs+=" METHOD=dhcp";
    else if(useStatic->isChecked()) cmdargs+=" METHOD=static";
    else cmdargs+=" METHOD=\"\"";
    // IP-config
    cmdargs+=QString(" ADDRESS=\"%1\"").arg(toValidIP(staticIP->text()));
    cmdargs+=QString(" NETMASK=\"%1\"").arg(toValidIP(staticNetmask->text()));
    cmdargs+=QString(" BROADCAST=\"%1\"").arg(toValidIP(staticBroadcast->text()));
    cmdargs+=QString(" GATEWAY=\"%1\"").arg(toValidIP(staticGateway->text()));
    // WLAN
    if(this->isWireless)
    {
        // WLAN: advanced Options
        cmdargs+=QString(" NWID=\"%1\"").arg(advNwid->text());
        cmdargs+=QString(" IWCONFIG=\"%1\"").arg(advIwconfig->text());
        cmdargs+=QString(" IWSPY=\"%1\"").arg(advIwspy->text());
        cmdargs+=QString(" IWPRIV=\"%1\"").arg(advIwpriv->text());
        // WLAN: channel/ frequency
        if( wirelessChanChanRadio->isChecked() )
            cmdargs+=QString(" CHANNEL=\"%1\" FREQ=\"\"").arg(wirelessChan->value());
        else if( wirelessChanFreqRadio->isChecked() )
            cmdargs+=QString(" CHANNEL=\"\" FREQ=\"%1\"").arg(wirelessFreq->text());
        else
            cmdargs+=QString(" CHANNEL=\"\" FREQ=\"\"");
        // WLAN: essid/ mode
        cmdargs+=QString(" ESSID=\"%1\"").arg(wirelessEssid->text());
        switch( wirelessMode->currentItem() )
        {
            case 0:
                cmdargs+=QString(" MODE=\"\"");
                break;
            case 1:
                cmdargs+=QString(" MODE=\"Managed\"");
                break;
            case 2:
                cmdargs+=QString(" MODE=\"Ad-Hoc\"");
                break;
            case 3:
                cmdargs+=QString(" MODE=\"Master\"");
                break;
            case 4:
                cmdargs+=QString(" MODE=\"Repeater\"");
                break;
            case 5:
                cmdargs+=QString(" MODE=\"Secondary\"");
                break;
            case 6:
                cmdargs+=QString(" MODE=\"Auto\"");
                break;
        }
        // WLAN: encryption
        if( enableEncryption->isChecked() )
        {
            if( encryptionType->currentItem() == 0 )
            {
                // WEP
                cmdargs+=QString(" WPAKEY=\"\"");
                
                if( storePublic->isChecked() )
                    cmdargs+=QString(" PUBKEY=\"1\"");
                else if( storePrivate->isChecked() )
                    cmdargs+=QString(" PUBKEY=\"-1\"");
                
                if( wepKeyAscii->isChecked() )
                    cmdargs+=QString(" KEY=\"s:%1\"").arg(wepKey->text());
                else if( wepKeyHex->isChecked() )
                    cmdargs+=QString(" KEY=\"%1\"").arg(wepKey->text());
            }
            else if( encryptionType->currentItem() == 1 )
            {
                // WPA-PSK (only if key has more that 8 characters)
                cmdargs+=QString(" KEY=\"\" PUBKEY=\"0\"");
                
                if( wpaKey->text().length() >= 8 )
                    cmdargs+=QString(" WPAKEY=\"%1\"").arg(wpaKey->text());
                else
                    cmdargs+=QString(" WPAKEY=\"\"");
            }
        }
        else
        {
            cmdargs+=QString(" KEY=\"\" PUBKEY=\"0\" WPAKEY=\"\"");
        }
    }
    
    this->shell->setCommand(QString("knxcc network").append(cmdargs));
    this->shell->start();
    
    if(autoEnable->isChecked())
        this->shell->setCommand("knxcc network set_mode auto "+this->interface);
    else
        this->shell->setCommand("knxcc network unset_mode auto "+this->interface);
    this->shell->start();
    
    if(hotplugEnable->isChecked())
        this->shell->setCommand("knxcc network set_mode allow-hotplug "+this->interface);
    else
        this->shell->setCommand("knxcc network unset_mode allow-hotplug "+this->interface);
    this->shell->start();
    
    // reload the new saved data
    // this is a bad workaround, will be fixed when the value-check-method is implemented
    this->load(this->interface);
}

bool knxcc_network_interface::isValidIP(QString ip)
{
    for(int i = 0; i < 4; i++)
    {
        QString section = ip.section(".", i, i);
        if( section.isEmpty() || section.toInt() >= 256 || section.toInt() < 0 ) return false;
    }
    return true;
}

QString knxcc_network_interface::toValidIP(QString ip)
{
    if(!isValidIP(ip)) return "";
    return ip;
}

#include "knxcc_network_interface.moc"
