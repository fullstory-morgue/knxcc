#include <qfile.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qregexp.h>
#include <qlistbox.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>

#include <unistd.h> // for getuid()

#include "knxcc_boot.h"

typedef KGenericFactory<knxcc_boot, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_knxcc_boot, ModuleFactory("knxccboot") )

knxcc_boot::knxcc_boot(QWidget *parent, const char *name, const QStringList &)
:BootDialog(parent, name)
{
    this->shell = new Process();
    this->setUseRootOnlyMsg(true);
    load();
    
    if (getuid() != 0)
    {
        freezeGroup->setDisabled(true);
        servicesGroup->setDisabled(true);
    }
}

void knxcc_boot::load(){
    // Freeze-State
    if (QFile::exists("/etc/frozen-rc.d"))
    {
        freezeServices->setChecked(true);
    }
    
    // Services
    activeServices->clear();
    this->shell->setCommand("knxcc boot get_active_services");
    this->shell->start(true);
    QStringList activeServicesList = QStringList::split( "\n", this->shell->getBuffer() );
    for ( QStringList::Iterator it = activeServicesList.begin(); it != activeServicesList.end(); ++it ) {
        QString service = *it;
        activeServices->insertItem(service);
    }

    disabledServices->clear();
    this->shell->setCommand("knxcc boot get_disabled_services");
    this->shell->start(true);
    QStringList disabledServicesList = QStringList::split( "\n", this->shell->getBuffer() );
    for ( QStringList::Iterator it = disabledServicesList.begin(); it != disabledServicesList.end(); ++it ) {
        QString service = *it;
        disabledServices->insertItem(service);
    }
}


void knxcc_boot::save(){
    // Freeze
    if (freezeServices->isChecked() && ! QFile::exists("/etc/frozen-rc.d"))
    {
        // execute freeze-rc.d
        this->shell->setCommand("freeze-rc.d");
        this->shell->start(true);
    }
    else if(! freezeServices->isChecked() && QFile::exists("/etc/frozen-rc.d"))
    {
        // execute unfreeze-rc.d
        this->shell->setCommand("unfreeze-rc.d");
        this->shell->start(true);
    }
    
    // Services
    for ( QValueList<QString>::Iterator it = this->servicesToDisable.begin(); it != this->servicesToDisable.end(); ++it ) {
        QString service = *it;
        this->shell->setCommand("knxcc boot disable_service "+service);
        this->shell->start(true);
    }
    this->servicesToDisable = QStringList();
    for ( QValueList<QString>::Iterator it = this->servicesToEnable.begin(); it != this->servicesToEnable.end(); ++it ) {
        QString service = *it;
        this->shell->setCommand("knxcc boot enable_service "+service);
        this->shell->start(true);
    }
    this->servicesToEnable = QStringList();
}

void knxcc_boot::defaults(){
}

void knxcc_boot::disableService(){
    QString currentService = activeServices->currentText();
    if(!currentService) return;
    QValueList<QString>::Iterator it = this->servicesToEnable.find(currentService);
    if(*it) this->servicesToEnable.erase(it);
    this->servicesToDisable.append(currentService);
    activeServices->removeItem(activeServices->currentItem());
    activeServices->clearSelection();
    disabledServices->insertItem(currentService);
    disabledServices->sort();
    disabledServices->clearSelection();
    emit changed(true);
}

void knxcc_boot::enableService(){
    QString currentService = disabledServices->currentText();
    if(!currentService) return;
    QValueList<QString>::Iterator it = this->servicesToDisable.find(currentService);
    if(*it) this->servicesToDisable.erase(it);
    this->servicesToEnable.append(currentService);
    disabledServices->removeItem(disabledServices->currentItem());
    disabledServices->clearSelection();
    activeServices->insertItem(currentService);
    activeServices->sort();
    activeServices->clearSelection();
    emit changed(true);
}

#include "knxcc_boot.moc"
