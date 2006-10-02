#include <qlineedit.h>
#include <qtextedit.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>

#include "knxcc_info.h"

typedef KGenericFactory<knxcc_info, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_knxcc_info, ModuleFactory("knxccinfo") )

knxcc_info::knxcc_info(QWidget *parent, const char *name, const QStringList &)
:InfoDialog(parent, name)
{
    this->shell = new Process();
    load();
}

void knxcc_info::load(){
    this->shell->setCommand("knxcc info kanotix_version");
    this->shell->start(true);
    kanotixVersion->setText(this->shell->getBuffer().simplifyWhiteSpace());
    
    this->shell->setCommand("knxcc info kanotix_info -v3");
    this->shell->start(true);
    infobash->setText(this->shell->getBuffer());
}


void knxcc_info::save(){

}

void knxcc_info::defaults(){

}

#include "knxcc_info.moc"
