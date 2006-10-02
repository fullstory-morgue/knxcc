#include <qfile.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qregexp.h>
#include <qspinbox.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>

#include <unistd.h> // for getuid()

#include "knxcc_display.h"

typedef KGenericFactory<knxcc_display, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_knxcc_display, ModuleFactory("knxccdisplay") )

knxcc_display::knxcc_display(QWidget *parent, const char *name, const QStringList &)
:DisplayDialog(parent, name)
{
    this->shell = new Process();
    this->setUseRootOnlyMsg(true);
    load();
    
    if (getuid() != 0)
    {
        // Disable User-Input-Widgets
        dpiSettingsGroup->setDisabled(true);
        resGroup->setDisabled(true);
        colorDepthGroup->setDisabled(true);
        chDriverGroup->setDisabled(true);
        nvidiaActionGroup->setDisabled(true);
        nvidiaOptionGroup->setDisabled(true);
        twinViewGroup->setDisabled(true);
        atiActionGroup->setDisabled(true);
        atiOptionGroup->setDisabled(true);
        fixFontsGroup->setDisabled(true);
    }
}

void knxcc_display::load(){
    // Resolution
    this->shell->setCommand("knxcc display get_resolution");
    this->shell->start(true);
    QString resolution = this->shell->getBuffer().stripWhiteSpace();
    userdefResolution->setText(resolution);
    radioUserdefRes->setChecked(true);
    // set the resolution as current item for selectResolution
    for(int i = 0; i < selectResolution->count(); i++)
    {
        if(selectResolution->text(i) == resolution)
        {
            selectResolution->setCurrentItem(i);
            radioSelectRes->setChecked(true);
            break;
        }
    }
    
    // DPI-Settings
    this->shell->setCommand("knxcc display get_dpi_settings");
    this->shell->start(true);
    dpiValue->setValue(this->shell->getBuffer().toInt());
    
    // DefaultColorDepth
    this->shell->setCommand("knxcc display get_colordepth");
    this->shell->start(true);
    QString colorDepth = this->shell->getBuffer().stripWhiteSpace() + " bit";
    for(int i = 0; i < selectColorDepth->count(); i++)
    {
        if(selectColorDepth->text(i) == colorDepth)
        {
            selectColorDepth->setCurrentItem(i);
            break;
        }
    }
    if(selectColorDepth->currentText() != colorDepth)
    {
        selectColorDepth->insertItem(colorDepth);
    }
    
    // Driver
    this->shell->setCommand("knxcc display get_driver");
    this->shell->start(true);
    driverUsing->setText(this->shell->getBuffer().stripWhiteSpace());
    
    // Nvidia-Drivers
    this->shell->setCommand("knxcc display nvidia_driver_supported");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        nvidiaActionGroup->setEnabled(false);
        chDriverNv->setEnabled(false);
        chDriverNvidia->setEnabled(false);
    }
    else
    {
        nvidiaActionGroup->setEnabled(true);
        chDriverNv->setEnabled(true);
        chDriverNvidia->setEnabled(true);
    }
    
    // reset nvidiaActionGroup
    nvidiaActionInstall->setChecked(false);
    nvidiaActionReinstall->setChecked(false);
    nvidiaActionRemove->setChecked(false);
    
    this->shell->setCommand("knxcc display nvidia_used");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        chDriverNvidia->setChecked(false);
        chDriverNvidia->setEnabled(false);
        nvidiaActionInstall->setEnabled(true);
        nvidiaActionReinstall->setEnabled(false);
        nvidiaActionRemove->setEnabled(false);
    }
    else
    {
        chDriverNvidia->setEnabled(true);
        chDriverNvidia->setChecked(true);
        nvidiaActionInstall->setEnabled(false);
        nvidiaActionReinstall->setEnabled(true);
        nvidiaActionRemove->setEnabled(true);
    }
    
    // Nvidia-Extension: Composite
    this->shell->setCommand("knxcc display nvidia_composite_used");
    this->shell->start(true);
    if(this->shell->exitStatus())
        nvidiaOptionComposite->setChecked(false);
    else
        nvidiaOptionComposite->setChecked(true);
    
    // Nvidia-Extension: TwinView
    this->shell->setCommand("knxcc display twinview_used");
    this->shell->start(true);
    if(this->shell->exitStatus())
        nvidiaOptionTwinView->setChecked(false);
    else
        nvidiaOptionTwinView->setChecked(true);
    
    this->shell->setCommand("knxcc display twinview_get_option TwinViewOrientation");
    this->shell->start(true);
    QString orientation = this->shell->getBuffer().stripWhiteSpace();
    for(int i = 0; i < twinViewOrientation->count(); i++)
    {
        if(twinViewOrientation->text(i) == orientation)
        {
            twinViewOrientation->setCurrentItem(i);
            break;
        }
    }
    
    this->shell->setCommand("knxcc display twinview_get_option MetaModes");
    this->shell->start(true);
    twinViewMetaModes->setText(this->shell->getBuffer().stripWhiteSpace());
    
    this->shell->setCommand("knxcc display twinview_get_option SecondMonitorHorizSync");
    this->shell->start(true);
    twinViewHSync->setText(this->shell->getBuffer().stripWhiteSpace());
    
    this->shell->setCommand("knxcc display twinview_get_option SecondMonitorVertRefresh");
    this->shell->start(true);
    twinViewVRefresh->setText(this->shell->getBuffer().stripWhiteSpace());
    
    // ATI-Drivers
    this->shell->setCommand("knxcc display radeon_driver_supported");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        atiActionGroup->setEnabled(false);
        chDriverAti->setEnabled(false);
        chDriverFglrx->setEnabled(false);
    }
    else
    {
        atiActionGroup->setEnabled(true);
        chDriverAti->setEnabled(true);
        chDriverFglrx->setEnabled(true);
    }
    
    // reset atiActionGroup
    atiActionInstall->setChecked(false);
    atiActionReinstall->setChecked(false);
    atiActionRemove->setChecked(false);
    
    this->shell->setCommand("knxcc display radeon_used");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        chDriverFglrx->setChecked(false);
        chDriverFglrx->setEnabled(false);
        atiActionInstall->setEnabled(true);
        atiActionReinstall->setEnabled(false);
        atiActionRemove->setEnabled(false);
    }
    else
    {
        chDriverFglrx->setEnabled(true);
        chDriverFglrx->setChecked(true);
        atiActionInstall->setEnabled(false);
        atiActionReinstall->setEnabled(true);
        atiActionRemove->setEnabled(true);
    }
    
    // preselect driver
    chDriverNv->setChecked(false);
    chDriverAti->setChecked(false);
    chDriverVesa->setChecked(false);
    chDriverFbdev->setChecked(false);
    chDriverRedetect->setChecked(false);
    
    if(driverUsing->text() == "nv")
        chDriverNv->setChecked(true);
    else if(driverUsing->text() == "ati")
        chDriverAti->setChecked(true);
    else if(driverUsing->text() == "vesa")
        chDriverVesa->setChecked(true);
    else if(driverUsing->text() == "fbdev")
        chDriverFbdev->setChecked(true);
    
    // uncheck Fix-Fonts
    fixFonts->setChecked(false);
    // uncheck removeModelines
    removeModelines->setChecked(false);
}


void knxcc_display::save(){
    bool restartXserver = false;
    
    // Resolution
    if(radioSelectRes->isChecked())
        this->shell->setCommand("knxcc display set_resolution "+selectResolution->currentText());
    else
        this->shell->setCommand("knxcc display set_resolution "+userdefResolution->text());
    this->shell->start(true);
    // set_resolution returns with exitstatus 128 if the Resolution is unchanged,
    // so we need to restart the Xserver only if the exitstatus is 0:
    if(!this->shell->exitStatus()) restartXserver = true;
    
    // Fix-Fonts
    if(fixFonts->isChecked())
    {
        this->shell->setCommand("knxcc display fix_fonts_konsole");
        this->shell->start(true);
        restartXserver = true;
    }
    
    // Remove Modelines
    if(removeModelines->isChecked())
    {
        this->shell->setCommand("knxcc display remove_modelines");
        this->shell->start(true);
        restartXserver = true;
    }
    
    // DPI-Settings
    this->shell->setCommand("knxcc display set_dpi_settings "+QString::number(dpiValue->value()));
    this->shell->start(true);
    // set_dpi_settings returns with exitstatus 128 if the DPI-Value is unchanged,
    // so we need to restart the Xserver only if the exitstatus is 0:
    if(!this->shell->exitStatus()) restartXserver = true;
    
    // DefaultColorDepth
    this->shell->setCommand("knxcc display set_colordepth "+selectColorDepth->currentText());
    this->shell->start(true);
    if(!this->shell->exitStatus()) restartXserver = true;
    
    // define Restart-Command here, might get changed by Nvidia/ATI-Drivers
    QString restartCommand = "restart_xserver";
    
    // set display-driver
    QString newdriver = QString::null;
    if(chDriverVesa->isChecked())           newdriver = "vesa";
    else if(chDriverFbdev->isChecked())     newdriver = "fbdev";
    else if(chDriverNv->isChecked())        newdriver = "nv";
    else if(chDriverAti->isChecked())       newdriver = "ati";
    if(newdriver)
    {
        /*
        this->shell->setCommand("knxcc display set_driver "+newdriver);
        this->shell->start(true);
        if(!this->shell->exitStatus()) restartXserver = true;
        */
        
        this->shell->setCommand("knxcc display get_driver");
        this->shell->start(true);
        QString olddriver = this->shell->getBuffer().stripWhiteSpace();
        
        if(olddriver != newdriver)
        {
            restartCommand="change_driver "+newdriver;
            restartXserver=true;
        }
    }
    
    if(chDriverRedetect->isChecked())
    {
        restartCommand = "redetect_driver";
        restartXserver = true;
    }
    else
    {
        // Nvidia-Drivers
        if(nvidiaActionInstall->isChecked() || nvidiaActionReinstall->isChecked())
        {
            if(nvidiaActionReinstall->isChecked())
                restartCommand = "nvidia_driver_reinstall";
            else
                restartCommand = "nvidia_driver_install";
            
            // Composite
            if(nvidiaOptionComposite->isChecked())
                restartCommand+= " -c";
            
            restartXserver = true;
        }
        else if(nvidiaActionRemove->isChecked())
        {
            restartCommand = "nvidia_driver_remove";
            restartXserver = true;
        }
        
        // ATI-Drivers
        if(atiActionInstall->isChecked() || atiActionReinstall->isChecked())
        {
            if(atiActionReinstall->isChecked())
                restartCommand = "radeon_driver_reinstall";
            else
                restartCommand = "radeon_driver_install";
            
            // Remove ModeLines
            if(atiOptionRemoveModelines->isChecked())
                restartCommand+= " -r";
            
            restartXserver = true;
        }
        else if(atiActionRemove->isChecked())
        {
            restartCommand = "radeon_driver_remove";
            restartXserver = true;
        }
    }
    
    // TwinView
    if(nvidiaOptionTwinView->isChecked())
    {
        this->shell->setCommand("knxcc display twinview_enable");
        this->shell->start(true);
        this->shell->setCommand("knxcc display twinview_change TwinViewOrientation "+twinViewOrientation->currentText());
        this->shell->start(true);
        this->shell->setCommand("knxcc display twinview_change SecondMonitorHorizSync "+twinViewHSync->text());
        this->shell->start(true);
        this->shell->setCommand("knxcc display twinview_change SecondMonitorVertRefresh "+twinViewVRefresh->text());
        this->shell->start(true);
        this->shell->setCommand("knxcc display twinview_change MetaModes "+twinViewMetaModes->text());
        this->shell->start(true);
    }
    else
    {
        this->shell->setCommand("knxcc display twinview_disable");
        this->shell->start(true);
    }
    if(!this->shell->exitStatus()) restartXserver = true;
    
    if(restartXserver)
    {
        if(KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("The XServer has to be restarted.\n"
            "\n"
            "Changes have been made that affect the XServer-configuration and thus it has to be restarted. Please save all work in progess, close other programs and press the Yes-button when ready.\n"
            "\n"
            "Do you want to restart the XServer now?"),
            i18n("Restart XServer")))
        {
            this->shell->setCommand("knxcc display "+restartCommand);
            this->shell->start(true);
        }
    }
}

void knxcc_display::defaults(){

}

#include "knxcc_display.moc"
