#include <qfile.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qregexp.h>
#include <qwidgetstack.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>
#include <keditlistbox.h>
#include <klineedit.h>

#include <unistd.h> // for getuid()

#include "knxcc_network.h"

typedef KGenericFactory<knxcc_network, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_knxcc_network, ModuleFactory("knxccnetwork") )

knxcc_network::knxcc_network(QWidget *parent, const char *name, const QStringList &)
:NetworkDialog(parent, name)
{
    this->shell = new Process();
    this->setUseRootOnlyMsg(true);
    load();
    
    ipValidator = new IpAddressValidator(this);
    dnsServers->lineEdit()->setValidator( ipValidator );
    dnsServers->lineEdit()->setInputMask( "000.000.000.000; " );
    
    if (getuid() != 0)
    {
        dnsServers->setDisabled(true);
    }
}

void knxcc_network::load(){
    this->shell->setCommand("knxcc network get_interface_list");
    this->shell->start(true);
    
    QStringList interfacesList = QStringList::split( "\n", this->shell->getBuffer() );
    int id = 0;
    interfaces->clear();
    for ( QStringList::Iterator it = interfacesList.begin(); it != interfacesList.end(); ++it ) {
        QString interface = *it;
        interfaces->insertItem(interface, id);
        widgetStack->removeWidget(widgetStack->widget(id));
        knxcc_network_interface *interfaceWidget = new knxcc_network_interface(NULL);
        interfaceWidget->load(interface);
        connect( interfaceWidget, SIGNAL( changed(bool) ), this, SLOT( changed() ) );
        widgetStack->addWidget(interfaceWidget, id);
        id++;
    }
    interfaces->setCurrentItem(0);
    
    // Hostname
    this->shell->setCommand("hostname");
    this->shell->start(true);
    hostName->setText( this->shell->getBuffer() );
    
    // Nameservers
    this->shell->setCommand("knxcc network get_nameservers");
    this->shell->start(true);
    
    QStringList nameserverList = QStringList::split( "\n", this->shell->getBuffer() );
    dnsServers->clear();
    for ( QStringList::Iterator it = nameserverList.begin(); it != nameserverList.end(); ++it )
        dnsServers->insertItem(*it);
}


void knxcc_network::save(){
    QStringList changedInterfaces;
    // Interfaces
    for(uint i = 0; i < this->interfaces->count(); i++)
    {
        if(this->widgetStack->widget(i)->hasChanges()) changedInterfaces += this->interfaces->text(i);
        this->widgetStack->widget(i)->saveChanges();
    }
    
    // Nameservers
    QString command = "knxcc network set_nameservers ";
    QStringList nameserverList = dnsServers->items();
    for ( QStringList::Iterator it = nameserverList.begin(); it != nameserverList.end(); ++it )
        command+=(*it) + " ";
    this->shell->setCommand(command);
    this->shell->start(true);
    
    // Restart networking
    if( QMessageBox::question( this, i18n( "Restart networking" ), i18n( "Your settings are saved, the following interfaces have been changed:\n"
        "\n"
        "%1\n"
        "\n"
        "Do you want to restart them now?" ).arg(changedInterfaces.join("\n")), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No ) == QMessageBox::Yes )
    {
        this->shell->setCommand(QString("knxcc network restart_interfaces %1").arg(changedInterfaces.join(" ")));
        this->shell->start(true);
    }
}

void knxcc_network::defaults(){
}

#include "knxcc_network.moc"
