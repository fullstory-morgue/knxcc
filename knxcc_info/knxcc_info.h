#ifndef KNXCC_INFO_H_
#define KNXCC_INFO_H_

#include <kcmodule.h>
#include "infodialog.h"
#include "process.h"

class knxcc_info : public InfoDialog
{
   Q_OBJECT

   public:
      knxcc_info(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();

   private:
      Process* shell;

};

#endif // KNXCC_INFO_H_
