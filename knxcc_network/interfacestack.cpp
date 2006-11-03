#include "interfacestack.h"

#include <qobjectlist.h>
#include <qobjectdict.h>
#include <qbutton.h>
#include <qbuttongroup.h>
#include <qwidgetstack.h>
#include <qapplication.h>

#include "knxcc_network.h"

class InterfaceStackPrivate {
public:
    class Invisible: public QWidget
    {
    public:
	Invisible( InterfaceStack * parent ): QWidget( parent )
	{
	    setBackgroundMode( NoBackground );
	}
	const char * className() const
	{
	    return "InterfaceStackPrivate::Invisible";
	}
    };
};

InterfaceStack::InterfaceStack( QWidget * parent, const char *name )
    : QWidgetStack( parent, name )
{
    d = 0;
    dict = new QIntDict<knxcc_network_interface>;
    focusWidgets = 0;
    topWidget = 0;
    invisible = new InterfaceStackPrivate::Invisible( this );
}

InterfaceStack::~InterfaceStack()
{
    delete focusWidgets;
    focusWidgets = 0;
    delete d;
    d = 0;
    delete dict;
    dict = 0;
}

void InterfaceStack::addWidget( knxcc_network_interface * w, int id )
{
    if ( !w || w == invisible )
	return;

    dict->insert( id+1, w );

    // preserve existing focus
    QWidget * f = w->focusWidget();
    while( f && f != w )
	f = f->parentWidget();
    if ( f ) {
	if ( !focusWidgets )
	    focusWidgets = new QPtrDict<QWidget>( 17 );
	focusWidgets->replace( w, w->focusWidget() );
    }

    w->hide();
    if ( w->parent() != this )
	w->reparent( this, 0, contentsRect().topLeft(), FALSE );
    w->setGeometry( contentsRect() );
}

void InterfaceStack::removeWidget( knxcc_network_interface * w )
{
    if ( !w )
	return;
    int i = id( w );
    if ( i != -1 )
	dict->take( i+1 );
    if ( w == topWidget )
	topWidget = 0;
    if ( dict->isEmpty() )
	invisible->hide(); // let background shine through again
}

void InterfaceStack::raiseWidget( int id )
{
    if ( id == -1 )
	return;
    knxcc_network_interface * w = dict->find( id+1 );
    if ( w )
	this->raiseWidget( w );
}

void InterfaceStack::raiseWidget( knxcc_network_interface * w )
{
    if ( !w || !isMyChild( w ) )
	return;

    topWidget = w;
    if ( !isVisible() )
	return;

    if ( !invisible->isVisible() ) {
	invisible->setGeometry( contentsRect() );
	invisible->lower();
	invisible->show();
    }

    // try to move focus onto the incoming widget if focus
    // was somewhere on the outgoing widget.
    QWidget * f = w->focusWidget();
    while ( f && f->parent() != this )
	f = f->parentWidget();
    if ( f && f->parent() == this ) {
	if ( !focusWidgets )
	    focusWidgets = new QPtrDict<QWidget>( 17 );
	focusWidgets->replace( f, f->focusWidget() );
	f->focusWidget()->clearFocus();
	if ( w->focusPolicy() != QWidget::NoFocus ) {
	    f = w;
	} else {
	    // look for the best focus widget we can find
	    // best == what we had (which may be deleted)
	    f = focusWidgets->find( w );
	    if ( f )
		focusWidgets->take( w );
	    // second best == selected button from button group
	    QWidget * fb = 0;
	    // third best == whatever candidate we see first
	    QWidget * fc = 0;
	    bool done = FALSE;
	    const QObjectList * c = w->children();
	    if ( c ) {
		QObjectListIt it( *c );
		QObject * wc;
		while( !done && (wc=it.current()) != 0 ) {
		    ++it;
		    if ( wc->isWidgetType() ) {
			if ( f == wc ) {
			    done = TRUE;
			} else if ( (((QWidget *)wc)->focusPolicy()&QWidget::TabFocus)
				    == QWidget::TabFocus ) {
			    QButton * b = (QButton *)wc;
			    if ( wc->inherits( "QButton" ) &&
				 b->group() && b->isOn() &&
				 b->group()->isExclusive() &&
				 ( fc == 0 ||
				   !fc->inherits( "QButton" ) ||
				   ((QButton*)fc)->group() == b->group() ) )
				fb = b;
			    else if ( !fc )
				fc = (QWidget*)wc;
			}
		    }
		}
		// f exists iff done
		if ( !done ) {
		    if ( fb )
			f = fb;
		    else if ( fc )
			f = fc;
		    else
			f = 0;
		}
	    }
	}
    }

    const QObjectList * c = children();
    QObjectListIt it( *c );
    QObject * o;

    while( (o=it.current()) != 0 ) {
	++it;
	if ( o->isWidgetType() && o != w && o != invisible )
	    ((QWidget *)o)->hide();
    }
    if ( f )
	f->setFocus();

    if ( isVisible() ) {
	emit aboutToShow( w );
	if ( receivers( SIGNAL(aboutToShow(int)) ) ) {
	    int i = id( w );
	    if ( i >= 0 )
		emit aboutToShow( i );
	}
    }

    w->setGeometry( invisible->geometry() );
    w->show();
}

bool InterfaceStack::isMyChild( knxcc_network_interface * w )
{
    const QObjectList * c = children();
    if ( !c || !w || w == invisible )
	return FALSE;
    QObjectListIt it( *c );
    QObject * o;

    while( (o=it.current()) != 0 ) {
	++it;
	if ( o->isWidgetType() && o == w )
	    return TRUE;
    }
    return FALSE;
}

void InterfaceStack::setChildGeometries()
{
    invisible->setGeometry( contentsRect() );
    if ( topWidget )
	topWidget->setGeometry( invisible->geometry() );
}

void InterfaceStack::show()
{
    //  Reimplemented in order to set the children's geometries
    //  appropriately.
    if ( !isVisible() && children() ) {
	setChildGeometries();

	const QObjectList * c = children();
	QObjectListIt it( *c );
	QObject * o;

	while( (o=it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() )
		if ( o == topWidget )
		    ((QWidget *)o)->show();
		else if ( o == invisible && topWidget != 0 )
		    ((QWidget *)o)->show();
		else
		    ((QWidget *)o)->hide();
	}
    }

    QFrame::show();
}

knxcc_network_interface * InterfaceStack::widget( int id ) const
{
    return id != -1 ? dict->find( id+1 ) : 0;
}

int InterfaceStack::id( knxcc_network_interface * widget ) const
{
    if ( !widget || !dict )
	return -1;

    QIntDictIterator<knxcc_network_interface> it( *dict );
    while ( it.current() && it.current() != widget )
	++it;
    return it.current() == widget ? it.currentKey()-1 : -1;
}
