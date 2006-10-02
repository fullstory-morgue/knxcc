#ifndef KNXCC_DISPLAY_H_
#define KNXCC_DISPLAY_H_

#include <kcmodule.h>
#include "displaydialog.h"
#include "process.h"

class knxcc_display : public DisplayDialog
{
   Q_OBJECT

   public:
      knxcc_display(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();

   private:
      Process* shell;

};

#endif // KNXCC_DISPLAY_H_
