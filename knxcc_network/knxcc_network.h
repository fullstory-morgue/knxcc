#ifndef KNXCC_NETWORK_H_
#define KNXCC_NETWORK_H_

#include <kcmodule.h>
#include "networkdialog.h"
#include "knxcc_network_interface.h"
#include "process.h"
#include "ipaddressvalidator.h"

class knxcc_network : public NetworkDialog
{
   Q_OBJECT

   public:
      knxcc_network(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();
      IpAddressValidator* ipValidator;

   private:
      Process* shell;
};

#endif // KNXCC_NETWORK_H_
