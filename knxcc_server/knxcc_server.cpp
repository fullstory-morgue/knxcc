#include <qfile.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qregexp.h>
#include <qlistbox.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>
#include <kled.h>

#include <unistd.h> // for getuid()

#include "knxcc_server.h"

typedef KGenericFactory<knxcc_server, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_knxcc_server, ModuleFactory("knxccserver") )

knxcc_server::knxcc_server(QWidget *parent, const char *name, const QStringList &)
:ServerDialog(parent, name)
{
    this->shell = new Process();
    this->setUseRootOnlyMsg(true);
    load();
    
    if (getuid() != 0)
    {
        sshActionGroup->setDisabled(true);
        hostKeyGroup->setDisabled(true);
        mySqlActionGroup->setDisabled(true);
    }
}

void knxcc_server::load(){
    this->shell->setCommand("pidof sshd");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        statusLabel->setText(i18n("Status: %1").arg(i18n("not running...")));
        statusLed->setColor(QColor(0xff0000));
    }
    else
    {
        statusLabel->setText(i18n("Status: %1").arg(i18n("running...")));
        statusLed->setColor(QColor(0x00ff00));
    }
    
    this->shell->setCommand("pidof mysqld");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        mySqlStatusLabel->setText(i18n("Status: %1").arg(i18n("not running...")));
        mySqlStatusLed->setColor(QColor(0xff0000));
    }
    else
    {
        mySqlStatusLabel->setText(i18n("Status: %1").arg(i18n("running...")));
        mySqlStatusLed->setColor(QColor(0x00ff00));
    }
    
    this->shell->setCommand("knxcc server ssh_hostkeys_exist");
    this->shell->start(true);
    if(this->shell->exitStatus())
    {
        // Hostkey doesn't exist
        oldHostKey->setDisabled(true);
        newHostKey->setDisabled(false);
        newHostKey->setChecked(true);
    }
    else
    {
        // Hostkey exists
        oldHostKey->setDisabled(false);
        newHostKey->setDisabled(true);
        oldHostKey->setChecked(true);
    }
}


void knxcc_server::save(){
    if(oldHostKey->isChecked())
    {
        this->shell->setCommand("knxcc server ssh_create_hostkeys");
        this->shell->start(true);
    }

    if(sshActionStartNow->isChecked())
        this->shell->setCommand("/etc/init.d/ssh start");
    else if(sshActionStopNow->isChecked())
        this->shell->setCommand("/etc/init.d/ssh stop");
    else if(sshActionRestart->isChecked())
        this->shell->setCommand("/etc/init.d/ssh restart");
    else if(sshActionReload->isChecked())
        this->shell->setCommand("/etc/init.d/ssh reload");
    else if(sshActionAdd->isChecked())
        this->shell->setCommand("update-rc.d -f ssh defaults");
    else if(sshActionDelete->isChecked())
        this->shell->setCommand("update-rc.d -f ssh remove");
    
    this->shell->start(true);
    sshActionUpdateStatus->setChecked(true);
    
    if(mySqlActionStartNow->isChecked())
        this->shell->setCommand("/etc/init.d/mysql start");
    else if(mySqlActionStopNow->isChecked())
        this->shell->setCommand("/etc/init.d/mysql stop");
    else if(mySqlActionRestart->isChecked())
        this->shell->setCommand("/etc/init.d/mysql restart");
    else if(mySqlActionReload->isChecked())
        this->shell->setCommand("/etc/init.d/mysql reload");
    else if(mySqlActionAdd->isChecked())
        this->shell->setCommand("update-rc.d -f mysql defaults");
    else if(mySqlActionDelete->isChecked())
        this->shell->setCommand("update-rc.d -f mysql remove");
    
    this->shell->start(true);
    mySqlActionUpdateStatus->setChecked(true);
    
    this->load();
}

void knxcc_server::defaults(){
}

#include "knxcc_server.moc"
