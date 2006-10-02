#ifndef INTERFACE_STACK_H_
#define INTERFACE_STACK_H_

#include <qintdict.h>
#include <qptrdict.h>
#include <qwidgetstack.h>

#include "knxcc_network_interface.h"

class InterfaceStackPrivate;


class Q_EXPORT InterfaceStack: public QWidgetStack
{
    Q_OBJECT
public:
    InterfaceStack( QWidget * parent = 0, const char *name = 0 );
    ~InterfaceStack();

    void addWidget( knxcc_network_interface *, int );
    void removeWidget( knxcc_network_interface * );
    void show();
    knxcc_network_interface * widget( int ) const;
    int id( knxcc_network_interface * ) const;

public slots:
    void raiseWidget( int );
    void raiseWidget( knxcc_network_interface * );

protected:
    virtual void setChildGeometries();

private:
    bool isMyChild( knxcc_network_interface * );

    InterfaceStackPrivate * d;
    QIntDict<knxcc_network_interface> * dict;
    QPtrDict<QWidget> * focusWidgets;
    QWidget * topWidget;
    QWidget * invisible;
};

#endif // INTERFACE_STACK_H_
