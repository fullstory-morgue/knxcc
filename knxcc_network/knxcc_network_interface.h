#ifndef KNXCC_NETWORK_INTERFACE_H_
#define KNXCC_NETWORK_INTERFACE_H_

#include <kcmodule.h>
#include "interfacewidget.h"
#include "process.h"
#include "qhexvalidator.h"
#include "ipaddressvalidator.h"

class knxcc_network_interface : public InterfaceWidget
{
   Q_OBJECT

   public:
      knxcc_network_interface(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load(QString interface);
      void saveChanges();
      bool hasChanges();
      QHexValidator* hexValidator;
      IpAddressValidator* ipValidator;

   private:
      Process* shell;
      bool hasChanged;
      bool isWireless;
      QString interface;
      bool isValidIP(QString ip);
      QString toValidIP(QString ip);

   signals:
      void changed(bool);

   protected slots:
      virtual void changed();
      virtual void wepUseAscii(bool yesno);
      virtual void wepUseHex(bool yesno);
      virtual void wepKeyLenChanged();
      virtual void wpaKeyLenCheck();
};

#endif // KNXCC_NETWORK_INTERFACE_H_
