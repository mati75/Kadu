/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


///#define QT_CLEAN_NAMESPACE
// Avoid compilation error: http://www.kadu.net/forum/viewtopic.php?p=84272#84272
// >   In file included from /usr/include/X11/extensions/XI.h:55,
// >                    from /usr/include/X11/extensions/XInput.h:56,
// >                    from /usr/include/X11/extensions/XTest.h:50,
// >                    from panelkadu/panelkadu.cpp:23:
// >   /usr/include/X11/Xmd.h: At global scope:
// >   /usr/include/X11/Xmd.h:137: error: conflicting declaration 'typedef long int INT32'
// >   /opt/qt/include/qglobal.h:706: error: 'INT32' has a previous declaration as 'typedef int INT32'


#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QX11Info>

#include "config_file.h"
#include "debug.h"
#include "kadu.h"

#include "panelkadu.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>


PanelKadu *panelKadu;


extern "C" int panelkadu_init()
{
	kdebugf();
	panelKadu = new PanelKadu();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/panelkadu.ui"), panelKadu );
	kdebugf2();
	return 0;
}


extern "C" void panelkadu_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/panelkadu.ui"), panelKadu );
	delete panelKadu;
	panelKadu = NULL;
	kdebugf2();
}


PanelKadu::PanelKadu() : QObject( NULL, "panelkadu" )
{
	// configuration handling
	createDefaultConfiguration();
	// save window geometry
	oldGeometry = kadu->geometry();
	// hide Kadu
	kadu->hide();
	// change window type to dock
	Atom atomproperty = XInternAtom( QX11Info::display(), "_NET_WM_WINDOW_TYPE", False );
	Atom atomvalue    = XInternAtom( QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False );
	XChangeProperty( QX11Info::display(), kadu->winId(), atomproperty, XA_ATOM, 32, PropModeReplace, (unsigned char *)&atomvalue, 1 );
	// create the event filter
	eventfilter = new EventFilter();
	// read the configuration and force its usage
	configurationUpdated();
	// create activation timer
	activationTimer = new QTimer( this );
	connect( activationTimer, SIGNAL(timeout()), this, SLOT(showKadu()) );
	// create hiding timer
	hidingTimer = new QTimer( this );
	connect( hidingTimer, SIGNAL(timeout()), this, SLOT(hideKadu()) );
	// start the mouse timer
	mouseTimer = new QTimer( this );
	connect( mouseTimer, SIGNAL(timeout()), this, SLOT(checkMouse()) );
	mouseTimer->start( PANELKADU_MOUSEITMERINTERVAL, FALSE );
	// hide panel at module startup
	QTimer::singleShot( 1, this, SLOT(hideKadu()) );
}


PanelKadu::~PanelKadu()
{
	// remove the event filter (just in case)
	qApp->removeEventFilter( eventfilter );
	// stop and delete the mouse timer
	mouseTimer->stop();
	delete mouseTimer;
	// stop and delete activation timer
	activationTimer->stop();
	delete activationTimer;
	// stop and delete hiding timer
	hidingTimer->stop();
	delete hidingTimer;
	// hide Kadu (and remove StaysOnTop flag)
	kadu->hide();
	// set window type to ordinary TopLevel
	kadu->reparent( 0, Qt::WType_TopLevel, kadu->pos(), false );
	// disable window's size limits
	kadu->setMinimumSize( 0, 0 );
	kadu->setMaximumSize( 32767, 32767 );
	// restore old window's geometry
	kadu->setGeometry( oldGeometry );
	if( ! kadu->closing() )  // if the module is being unloaded
	{
		// show Kadu normally
		kadu->show();
	}
}


void PanelKadu::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	QLineEdit *activationRanges;
	activationRanges = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("panelkadu/activationRanges"));
	connect( mainConfigurationWindow->widgetById("panelkadu/useActivationRanges"), SIGNAL(toggled(bool)), activationRanges, SLOT(setEnabled(bool)) );
	QSpinBox *panelPosition;
	panelPosition = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("panelkadu/panelPosition"));
	connect( mainConfigurationWindow->widgetById("panelkadu/userDefinedPanelLength"), SIGNAL(toggled(bool)), panelPosition, SLOT(setEnabled(bool)) );
	QSpinBox *panelLength;
	panelLength = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("panelkadu/panelLength"));
	connect( mainConfigurationWindow->widgetById("panelkadu/userDefinedPanelLength"), SIGNAL(toggled(bool)), panelLength, SLOT(setEnabled(bool)) );
}


void PanelKadu::createDefaultConfiguration()
{
	config_file.addVariable( "PanelKadu", "Side"                   , PANELKADU_SIDE_RIGHT            );
	config_file.addVariable( "PanelKadu", "Size"                   , kadu->width()                   );
	config_file.addVariable( "PanelKadu", "UserDefinedPanelLength" , false                           );
	config_file.addVariable( "PanelKadu", "PanelPosition"          , "0"                             );
	config_file.addVariable( "PanelKadu", "PanelLength"            , PANELKADU_DEFAULTLENGTH         );
	config_file.addVariable( "PanelKadu", "ActivationTime"         , PANELKADU_DEFAULTACTIVATIONTIME );
	config_file.addVariable( "PanelKadu", "HideTime"               , PANELKADU_DEFAULTHIDETIME       );
	config_file.addVariable( "PanelKadu", "HidingMargin"           , 0                               );
	config_file.addVariable( "PanelKadu", "UseActivationRanges"    , false                           );
	config_file.addVariable( "PanelKadu", "ActivationRanges"       , ""                              );
	config_file.addVariable( "PanelKadu", "DontHidePanelWhenActive", false                           );
}


void PanelKadu::configurationUpdated()
{
	// update configuration data
	side                    = config_file.readNumEntry(  "PanelKadu", "Side"                    );
	size                    = config_file.readNumEntry(  "PanelKadu", "Size"                    );
	userDefinedPanelLength  = config_file.readBoolEntry( "PanelKadu", "UserDefinedPanelLength"  );
	panelPosition           = config_file.readNumEntry(  "PanelKadu", "PanelPosition"           );
	panelLength             = config_file.readNumEntry(  "PanelKadu", "PanelLength"             );
	activationTime          = config_file.readNumEntry(  "PanelKadu", "ActivationTime"          );
	hideTime                = config_file.readNumEntry(  "PanelKadu", "HideTime"                );
	hidingMargin            = config_file.readNumEntry(  "PanelKadu", "HidingMargin"            );
	useActivationRanges     = config_file.readBoolEntry( "PanelKadu", "UseActivationRanges"     );
	activationRanges        = config_file.readEntry(     "PanelKadu", "ActivationRanges"        );
	dontHidePanelWhenActive = config_file.readBoolEntry( "PanelKadu", "DontHidePanelWhenActive" );
	// obtain desktop dimensions
	int dW = QApplication::desktop()->width();
	int dH = QApplication::desktop()->height();
	// hide Kadu
	kadu->hide();
	// minimum kadu dimensions
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
	{
		if( size < PANELKADU_KADUMINIMUMWIDTH ) size = PANELKADU_KADUMINIMUMWIDTH;
		if( panelLength < PANELKADU_KADUMINIMUMHEIGHT ) panelLength = PANELKADU_KADUMINIMUMHEIGHT;
	}
	else
	{
		if( size < PANELKADU_KADUMINIMUMHEIGHT ) size = PANELKADU_KADUMINIMUMHEIGHT;
		if( panelLength < PANELKADU_KADUMINIMUMWIDTH ) panelLength = PANELKADU_KADUMINIMUMWIDTH;
	}
	// panel position and length
	int position = 0;
	int length = 0;
	if( ! userDefinedPanelLength )
	{
		position = 0;
		if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
			length = dH;
		else
			length = dW;
	}
	else
	{
		int sidelength = 0;
		if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
			sidelength = dH;
		else
			sidelength = dW;
		position = panelPosition;
		length = panelLength;
		if( position > sidelength - length )
			position = sidelength - length;
		if( position < 0 )
			position = 0;
		if( length > sidelength - position )
			length = sidelength - position;
	}
	// move Kadu to selected side and resize it
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
	{
		kadu->setMinimumSize( size, length );
		kadu->setMaximumSize( size, length );
	}
	else
	{
		kadu->setMinimumSize( length, size );
		kadu->setMaximumSize( length, size );
	}
	switch( side )
	{
		case PANELKADU_SIDE_RIGHT:
			kadu->setGeometry( dW-size, position, size, length );
			break;
		case PANELKADU_SIDE_BOTTOM:
			kadu->setGeometry( position, dH-size, length, size );
			break;
		case PANELKADU_SIDE_LEFT:
			kadu->setGeometry( 0, position, size, length );
			break;
		case PANELKADU_SIDE_TOP:
			kadu->setGeometry( position, 0, length, size );
			break;
	}
}


bool PanelKadu::isInActivationRanges( int number )
{
	// this functions returns true on any activationRanges syntax error!
	if( activationRanges.stripWhiteSpace() == "" )
		return true;
	QStringList rangeslist;
	rangeslist = QStringList::split( QString(" "), activationRanges.stripWhiteSpace(), true );
	QStringList range;
	int a = 0, b = 0;
	bool ok = false;
	for( QStringList::Iterator it = rangeslist.begin(); it != rangeslist.end(); ++it )
	{
		range = QStringList::split( "-", *it, true );
		if( range.count() != 2 )
			return true;
		a = range.at(0).toInt( &ok, 10 );
		if( ! ok )
			return true;
		b = range.at(1).toInt( &ok, 10 );
		if( ! ok )
			return true;
		if( ( number >= a ) && ( number <= b ) )
			return true;
	}
	return false;
}


void PanelKadu::checkMouse()
{
	// obtain cursor position
	int x = QCursor().pos().x();
	int y = QCursor().pos().y();
	// obtain desktop dimensions
	int dW = QApplication::desktop()->width();
	int dH = QApplication::desktop()->height();
	// side width
	int sidewidth = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		sidewidth = dH;
	else
		sidewidth = dW;
	// cursor distance from given side
	int sidecursordist = 0;
	switch( side )
	{
		case PANELKADU_SIDE_RIGHT:
			sidecursordist = dW - x - 1;
			break;
		case PANELKADU_SIDE_BOTTOM:
			sidecursordist = dH - y - 1;
			break;
		case PANELKADU_SIDE_LEFT:
			sidecursordist = x;
			break;
		case PANELKADU_SIDE_TOP:
			sidecursordist = y;
			break;
	}
	// side-oriented cursor position
	int sidecursorpos = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		sidecursorpos = y;
	else
		sidecursorpos = x;
	// Kadu window size
	int windowsize = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		windowsize = kadu->width();
	else
		windowsize = kadu->height();
	// Kadu window position
	int position = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		position = kadu->y();
	else
		position = kadu->x();
	// Kadu window length
	int length = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		length = kadu->height();
	else
		length = kadu->width();
	// show/hide Kadu
	if( ! kadu->isVisible() )  // panel is hidden
	{
		hidingTimer->stop();
		if( sidecursordist == 0 )  // is the cursor touching the edge?
		{
			if( ! useActivationRanges )  // do we use activation ranges?
			{
				if( ( ( sidecursorpos >= position ) && ( sidecursorpos <= position+length ) ) )  // cursor is inside the space occupied by the (shown) panel
				{
					if( ! activationTimer->isActive() )
						activationTimer->start( activationTime, true );
				}
				else
				{
					activationTimer->stop();
				}
			}
			else
			{
				if( isInActivationRanges( sidecursorpos ) )  // is the cursor position in the defined activation range?
				{
					if( ! activationTimer->isActive() )
						activationTimer->start( activationTime, true );
				}
				else
				{
					activationTimer->stop();
				}
			}
		}
		else
		{
			activationTimer->stop();
		}
	}
	else if( kadu->isVisible() && ( ( ! dontHidePanelWhenActive ) || ( ! kadu->isActiveWindow() ) ) )  // panel is show and can be hidden
	{
		activationTimer->stop();
		if( sidecursordist >= windowsize+hidingMargin )  // cursor distance from the edge is greater than the panel size
		{
			if( ! hidingTimer->isActive() )
				hidingTimer->start( hideTime, true );
		}
		else if( ( ( sidecursorpos < position-hidingMargin ) || ( sidecursorpos > position+length+hidingMargin ) ) )  // cursor is outside the panel
		{
			if( sidecursordist > 0 )  // cursor is not touching the edge
			{
				if( ! hidingTimer->isActive() )
					hidingTimer->start( hideTime, true );
			}
			else if( ! useActivationRanges )  // the cursor is outside the panel
			{
				if( ! hidingTimer->isActive() )
					hidingTimer->start( hideTime, true );
			}
			else if( useActivationRanges && ( ! isInActivationRanges( sidecursorpos ) ) )  // the activation ranges are being used and cursor is outside them
			{
				if( ! hidingTimer->isActive() )
					hidingTimer->start( hideTime, true );
			}
			else
			{
				hidingTimer->stop();
			}
		}
		else  // the cursor is inside the Kadu window
		{
			hidingTimer->stop();
		}
	}
}


void PanelKadu::showKadu()
{
	// show panel
	kadu->show();
	// install the event filter
	qApp->installEventFilter( eventfilter );
}


void PanelKadu::hideKadu()
{
	// remove the event filter
	qApp->removeEventFilter( eventfilter );
	// hide panel
	kadu->hide();
}


bool EventFilter::eventFilter( QObject *o, QEvent *e )
{
	if( ( e->type() == QEvent::MouseButtonPress ) )
	{
		QMouseEvent *me = (QMouseEvent*)e;
		if( kadu->geometry().contains( me->globalPos() ) )
		{
			// activate Kadu's window
			Atom netactivewindow = XInternAtom( QX11Info::display(), "_NET_ACTIVE_WINDOW", False );
			XEvent e;
			e.xclient.type = ClientMessage;
			e.xclient.message_type = netactivewindow;
			e.xclient.display =  QX11Info::display();
			e.xclient.window = kadu->winId();
			e.xclient.format = 32;
			e.xclient.data.l[0] = 2; // from tool
			e.xclient.data.l[1] = CurrentTime;
			e.xclient.data.l[2] = 0;
			e.xclient.data.l[3] = 0l;
			e.xclient.data.l[4] = 0l;
			XSendEvent( QX11Info::display(), DefaultRootWindow( QX11Info::display() ), False, SubstructureRedirectMask | SubstructureNotifyMask, &e );
			// remove the event filter
			qApp->removeEventFilter( panelKadu->eventfilter );
		}
	}
	// forward the event
	return FALSE;
}
