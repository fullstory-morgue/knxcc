#ifndef __IPADDRESSVALIDATOR_H
#define __IPADDRESSVALIDATOR_H

#include <qvalidator.h>

class IpAddressValidator : public QValidator {

  public:
    IpAddressValidator ( QWidget * parent, const char * name = 0 );
    virtual ~IpAddressValidator ();
    virtual State validate ( QString &, int & ) const;
};


#endif

