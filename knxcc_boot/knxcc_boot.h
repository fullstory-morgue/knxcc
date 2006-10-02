#ifndef KNXCC_BOOT_H_
#define KNXCC_BOOT_H_

#include <kcmodule.h>
#include "bootdialog.h"
#include "process.h"

class knxcc_boot : public BootDialog
{
   Q_OBJECT

   public:
      knxcc_boot(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();

   private:
      Process* shell;
      QValueList<QString> servicesToEnable;
      QValueList<QString> servicesToDisable;

   protected slots:
      void disableService();
      void enableService();

};

#endif // KNXCC_BOOT_H_
