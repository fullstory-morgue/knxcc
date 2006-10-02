#ifndef KNXCC_TIME_H_
#define KNXCC_TIME_H_

#include <kcmodule.h>

class QTabWidget;

class knxcc_time : public KCModule
{
   Q_OBJECT

   public:
      knxcc_time( QWidget *parent, const char *name, const QStringList& );
      void load();
      void save();
   
   private slots:
      void moduleChanged(bool isChanged);
   
   private:
      void addTab( const QString &name, const QString &label );
    
      QTabWidget *m_tabs;
      QMap<KCModule*, bool> m_modules;
      bool m_changed;

};

#endif // KNXCC_TIME_H_
