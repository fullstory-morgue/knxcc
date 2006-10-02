#include <qvalidator.h>
#include <qwidget.h>
#include <malloc.h>

#include "ipaddressvalidator.h"

IpAddressValidator::IpAddressValidator ( QWidget * parent, const char * name )
  : QValidator(parent, name)
{}

IpAddressValidator::~IpAddressValidator()
{}

QValidator::State IpAddressValidator::validate ( QString &str, int &pos ) const
{
    // valid if no IP entered:
    if(str.length() == 0 || str == "...")
            return QValidator::Acceptable;

    for(unsigned int i = 0; i < 4; i++)
    {
        QString section = str.section(".", i, i);
        if(section.toInt() > 255) return QValidator::Invalid;
        if(section.isEmpty()) return QValidator::Intermediate;
    }
    
    return QValidator::Acceptable;
}


