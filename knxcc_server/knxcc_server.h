#ifndef KNXCC_SERVER_H_
#define KNXCC_SERVER_H_

#include <kcmodule.h>
#include "serverdialog.h"
#include "process.h"

class knxcc_server : public ServerDialog
{
   Q_OBJECT

   public:
      knxcc_server(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();

   private:
      Process* shell;
};

#endif // KNXCC_SERVER_H_
