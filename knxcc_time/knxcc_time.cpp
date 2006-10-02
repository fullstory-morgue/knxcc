#include <qapplication.h>
#include <qlayout.h>
#include <qtabwidget.h>

#include <kcmoduleloader.h>
#include <kdialog.h>
#include <kgenericfactory.h>

#include "knxcc_time.h"

typedef KGenericFactory<knxcc_time, QWidget> ModuleFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_knxcc_time, ModuleFactory("knxcctime") )

knxcc_time::knxcc_time( QWidget *parent, const char *name, const QStringList& )
  : KCModule( parent, name )
  , m_changed(false)
{
  m_tabs = new QTabWidget( this );

  addTab( "clock", i18n( "Date && Time" ) );

  QVBoxLayout *top = new QVBoxLayout( this, 0, KDialog::spacingHint() );
  top->addWidget( m_tabs );

  setButtons( Apply|Help );
  load();
}

void knxcc_time::addTab( const QString &name, const QString &label )
{
  QWidget *page = new QWidget( m_tabs, name.latin1() );
  QVBoxLayout *top = new QVBoxLayout( page, KDialog::marginHint() );

  KCModule *kcm = KCModuleLoader::loadModule( name, page );

  if ( kcm )
  {
    top->addWidget( kcm );
    m_tabs->addTab( page, label );

    connect( kcm, SIGNAL( changed(bool) ), SLOT( moduleChanged(bool) ) );
    m_modules.insert(kcm, false);
  }
  else
    delete page;
}

void knxcc_time::load(){
  for (QMap<KCModule*, bool>::ConstIterator it = m_modules.begin(); it != m_modules.end(); ++it)
    it.key()->load();
}


void knxcc_time::save(){
  for (QMap<KCModule*, bool>::Iterator it = m_modules.begin(); it != m_modules.end(); ++it)
    if (it.data())
      it.key()->save();
}

void knxcc_time::moduleChanged( bool isChanged )
{
  QMap<KCModule*, bool>::Iterator currentModule = m_modules.find(static_cast<KCModule*>(const_cast<QObject*>(sender())));
  Q_ASSERT(currentModule != m_modules.end());
  if (currentModule.data() == isChanged)
    return;
    
  currentModule.data() = isChanged;

  bool c = false;
  
  for (QMap<KCModule*, bool>::ConstIterator it = m_modules.begin(); it != m_modules.end(); ++it) {
    if (it.data()) {
      c = true;
      break;
    }
  }
    
  if (m_changed != c) {
    m_changed = c;
    emit changed(c);
  }
}
