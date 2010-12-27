/****************************************************************************
*                                                                           *
*   PanelKadu module for Kadu                                               *
*   Copyright (C) 2008-2010  Piotr Dąbrowski ultr@ultr.pl                   *
*                                                                           *
*   This program is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation, either version 3 of the License, or       *
*   (at your option) any later version.                                     *
*                                                                           *
*   This program is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*   GNU General Public License for more details.                            *
*                                                                           *
*   You should have received a copy of the GNU General Public License       *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                           *
****************************************************************************/




#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMouseEvent>
#include <QSpinBox>
#include <QVarLengthArray>
#include <QX11Info>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "misc/path-conversion.h"
#include "activate.h"
#include "debug.h"
#include "x11tools.h"

#include "panelkadu.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>




PanelKadu *panelkadu;




extern "C" int panelkadu_init()
{
	kdebugf();
	panelkadu = new PanelKadu();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/panelkadu.ui") );
	kdebugf2();
	return 0;
}


extern "C" void panelkadu_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/panelkadu.ui") );
	delete panelkadu;
	panelkadu = NULL;
	kdebugf2();
}


PanelKadu::PanelKadu() : QObject()
{
	// reparenting handling
	connect( Core::instance()->kaduWindow(), SIGNAL(parentChanged(QWidget*)), this, SLOT(kaduParentChanged(QWidget*)) );
	// configuration handling
	createDefaultConfiguration();
	// save window geometry
	oldGeometry = Core::instance()->kaduWindow()->window()->geometry();
	// panelize
	panelize( Core::instance()->kaduWindow()->window() );
}


PanelKadu::~PanelKadu()
{
	// remove reparenting handling
	disconnect( Core::instance()->kaduWindow(), SIGNAL(parentChanged(QWidget*)), this, SLOT(kaduParentChanged(QWidget*)) );
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
	// depanelize
	depanelize( Core::instance()->kaduWindow()->window() );
}


void PanelKadu::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	connect( mainConfigurationWindow->widget()->widgetById("panelkadu/useActivationRanges")   , SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("panelkadu/activationRanges"), SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widget()->widgetById("panelkadu/userDefinedPanelLength"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("panelkadu/panelPosition")   , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widget()->widgetById("panelkadu/userDefinedPanelLength"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("panelkadu/panelLength")     , SLOT(setEnabled(bool)) );
}


void PanelKadu::createDefaultConfiguration()
{
	config_file.addVariable( "PanelKadu", "Side"                   , PANELKADU_SIDE_RIGHT                              );
	config_file.addVariable( "PanelKadu", "Size"                   , Core::instance()->kaduWindow()->window()->width() );
	config_file.addVariable( "PanelKadu", "UserDefinedPanelLength" , false                                             );
	config_file.addVariable( "PanelKadu", "PanelPosition"          , 0                                                 );
	config_file.addVariable( "PanelKadu", "PanelLength"            , PANELKADU_DEFAULTLENGTH                           );
	config_file.addVariable( "PanelKadu", "ActivationTime"         , PANELKADU_DEFAULTACTIVATIONTIME                   );
	config_file.addVariable( "PanelKadu", "HideTime"               , PANELKADU_DEFAULTHIDETIME                         );
	config_file.addVariable( "PanelKadu", "HidingMargin"           , 0                                                 );
	config_file.addVariable( "PanelKadu", "UseActivationRanges"    , false                                             );
	config_file.addVariable( "PanelKadu", "ActivationRanges"       , ""                                                );
	config_file.addVariable( "PanelKadu", "DontHidePanelWhenActive", false                                             );
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
	Core::instance()->kaduWindow()->window()->hide();
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
		Core::instance()->kaduWindow()->window()->setMinimumSize( size, length );
		Core::instance()->kaduWindow()->window()->setMaximumSize( size, length );
	}
	else
	{
		Core::instance()->kaduWindow()->window()->setMinimumSize( length, size );
		Core::instance()->kaduWindow()->window()->setMaximumSize( length, size );
	}
	switch( side )
	{
		case PANELKADU_SIDE_RIGHT:
			Core::instance()->kaduWindow()->window()->setGeometry( dW-size, position, size, length );
			break;
		case PANELKADU_SIDE_BOTTOM:
			Core::instance()->kaduWindow()->window()->setGeometry( position, dH-size, length, size );
			break;
		case PANELKADU_SIDE_LEFT:
			Core::instance()->kaduWindow()->window()->setGeometry( 0, position, size, length );
			break;
		case PANELKADU_SIDE_TOP:
			Core::instance()->kaduWindow()->window()->setGeometry( position, 0, length, size );
			break;
	}
}


bool PanelKadu::isInActivationRanges( int number )
{
	// this functions returns true on any activationRanges syntax error!
	if( activationRanges.trimmed().isEmpty() )
		return true;
	QStringList rangeslist;
	rangeslist = activationRanges.trimmed().split( QString(" "), QString::SkipEmptyParts );
	QStringList range;
	int a = 0, b = 0;
	bool ok = false;
	for( QStringList::Iterator it = rangeslist.begin(); it != rangeslist.end(); ++it )
	{
		range = (*it).split( "-", QString::SkipEmptyParts );
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
		windowsize = Core::instance()->kaduWindow()->window()->width();
	else
		windowsize = Core::instance()->kaduWindow()->window()->height();
	// Kadu window position
	int position = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		position = Core::instance()->kaduWindow()->window()->y();
	else
		position = Core::instance()->kaduWindow()->window()->x();
	// Kadu window length
	int length = 0;
	if( ( side == PANELKADU_SIDE_RIGHT ) || ( side == PANELKADU_SIDE_LEFT ) )
		length = Core::instance()->kaduWindow()->window()->height();
	else
		length = Core::instance()->kaduWindow()->window()->width();
	// show/hide Kadu
	if( ! Core::instance()->kaduWindow()->window()->isVisible() )  // panel is hidden
	{
		hidingTimer->stop();
		if( sidecursordist == 0 )  // is the cursor touching the edge?
		{
			if( ! useActivationRanges )  // do we use activation ranges?
			{
				if( ( ( sidecursorpos >= position ) && ( sidecursorpos <= position+length ) ) )  // cursor is inside the space occupied by the (shown) panel
				{
					if( ! activationTimer->isActive() )
						activationTimer->start( activationTime );
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
						activationTimer->start( activationTime );
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
	else if( Core::instance()->kaduWindow()->window()->isVisible() )  // panel is shown
	{
		bool canhide = true;
		if( dontHidePanelWhenActive )
		{
			// don't hide when active
			if( _isActiveWindow( Core::instance()->kaduWindow()->window() ) )
				canhide = false;
		}
		else
		{
			// don't hide when a menu is open
			if( X11_getActiveWindow( QX11Info::display() ) == Core::instance()->kaduWindow()->window()->winId() )
				if( ! Core::instance()->kaduWindow()->window()->isActiveWindow() )
					canhide = false;
		}
		if( canhide )  // panel can be hidden
		{
			activationTimer->stop();
			if( sidecursordist >= windowsize+hidingMargin )  // cursor distance from the edge is greater than the panel size
			{
				if( ! hidingTimer->isActive() )
					hidingTimer->start( hideTime );
			}
			else if( ( ( sidecursorpos < position-hidingMargin ) || ( sidecursorpos > position+length+hidingMargin ) ) )  // cursor is outside the panel
			{
				if( sidecursordist > 0 )  // cursor is not touching the edge
				{
					if( ! hidingTimer->isActive() )
						hidingTimer->start( hideTime );
				}
				else if( ! useActivationRanges )  // the cursor is outside the panel
				{
					if( ! hidingTimer->isActive() )
						hidingTimer->start( hideTime );
				}
				else if( useActivationRanges && ( ! isInActivationRanges( sidecursorpos ) ) )  // the activation ranges are being used and cursor is outside them
				{
					if( ! hidingTimer->isActive() )
						hidingTimer->start( hideTime );
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
}


void PanelKadu::showKadu()
{
	// set on all desktops
	unsigned int desktop = X11_ALLDESKTOPS;
	Atom win_desktop = XInternAtom( QX11Info::display(), "_NET_WM_DESKTOP", False );
	XChangeProperty( QX11Info::display(), Core::instance()->kaduWindow()->window()->winId(), win_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&desktop, 1 );
	// show panel and raise it
	animate();
	Core::instance()->kaduWindow()->window()->show();
	Core::instance()->kaduWindow()->window()->raise();
	// install the event filter
	qApp->installEventFilter( eventfilter );
}


void PanelKadu::hideKadu()
{
	// remove the event filter
	qApp->removeEventFilter( eventfilter );
	// hide panel
	animate();
	Core::instance()->kaduWindow()->window()->hide();
}


void PanelKadu::animate()
{
	#ifdef Q_WS_X11
		// set atom
		Atom kdeslide = XInternAtom( QX11Info::display(), "_KDE_SLIDE", False );
		QVarLengthArray<long, 1024> data(2);
		switch( side )
		{
			case PANELKADU_SIDE_LEFT:
					data[0] = Core::instance()->kaduWindow()->window()->geometry().left();
					data[1] = 0;
					break;
			case PANELKADU_SIDE_TOP:
					data[0] = Core::instance()->kaduWindow()->window()->geometry().top();
					data[1] = 1;
					break;
			case PANELKADU_SIDE_RIGHT:
					data[0] = Core::instance()->kaduWindow()->window()->geometry().right();
					data[1] = 2;
					break;
			case PANELKADU_SIDE_BOTTOM:
					data[0] = Core::instance()->kaduWindow()->window()->geometry().bottom();
					data[1] = 3;
			default:
					break;
		}
		if( isCompositingManagerRunning() )
			XChangeProperty( QX11Info::display(), Core::instance()->kaduWindow()->window()->winId(), kdeslide, kdeslide, 32, PropModeReplace, reinterpret_cast<unsigned char *>(data.data()), data.size() );
		else
			XDeleteProperty( QX11Info::display(), Core::instance()->kaduWindow()->window()->winId(), kdeslide);
	#endif
}


bool PanelKadu::isCompositingManagerRunning()
{
	#ifdef Q_WS_X11
		return X11_isCompositingManagerRunning( QX11Info::display() );
	#endif
	return false;
}


void PanelKadu::kaduParentChanged( QWidget *oldParent )
{
	if( oldParent != NULL )
		depanelize( oldParent->window() );
	else
		depanelize( Core::instance()->kaduWindow() );
	QMetaObject::invokeMethod( this, "panelize", Qt::QueuedConnection, Q_ARG( QWidget*, Core::instance()->kaduWindow()->window() ) );
}


void PanelKadu::panelize( QWidget *window )
{
	// hide Kadu
	window->hide();
	// change window type to dock
	Atom atomproperty = XInternAtom( QX11Info::display(), "_NET_WM_WINDOW_TYPE", False );
	Atom atomvalue    = XInternAtom( QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False );
	XChangeProperty( QX11Info::display(), window->winId(), atomproperty, XA_ATOM, 32, PropModeReplace, (unsigned char *)&atomvalue, 1 );
	// remove window's decoration, set always-on-top and force taskbar and pager skipping
	window->setWindowFlags( window->windowFlags() | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
	Atom win_state = XInternAtom( QX11Info::display(), "_NET_WM_STATE", False );
	Atom win_state_setting[] =
	{
		XInternAtom( QX11Info::display(), "_NET_WM_STATE_ABOVE"       , False ),
		XInternAtom( QX11Info::display(), "_NET_WM_STATE_SKIP_TASKBAR", False ),
		XInternAtom( QX11Info::display(), "_NET_WM_STATE_SKIP_PAGER"  , False )
	};
	XChangeProperty( QX11Info::display(), window->winId(), win_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)&win_state_setting, 3 );
	// set on all desktops
	unsigned int desktop = X11_ALLDESKTOPS;
	Atom win_desktop = XInternAtom( QX11Info::display(), "_NET_WM_DESKTOP", False );
	XChangeProperty( QX11Info::display(), window->winId(), win_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&desktop, 1 );
	// create the event filter
	eventfilter = new EventFilter();
	// read the configuration and force its usage
	configurationUpdated();
	// create activation timer
	activationTimer = new QTimer( this );
	activationTimer->setSingleShot( true );
	connect( activationTimer, SIGNAL(timeout()), this, SLOT(showKadu()) );
	// create hiding timer
	hidingTimer = new QTimer( this );
	hidingTimer->setSingleShot( true );
	connect( hidingTimer, SIGNAL(timeout()), this, SLOT(hideKadu()) );
	// start the mouse timer
	mouseTimer = new QTimer( this );
	connect( mouseTimer, SIGNAL(timeout()), this, SLOT(checkMouse()) );
	mouseTimer->start( PANELKADU_MOUSEITMERINTERVAL );
	// update panel configuration
	configurationUpdated();
	// hide panel at module startup
	QTimer::singleShot( 0, this, SLOT(hideKadu()) );
}


void PanelKadu::depanelize( QWidget *window )
{
	// hide Kadu (and remove StaysOnTop flag)
	window->hide();
	// set on the current desktop only
	unsigned int desktop = X11_getCurrentDesktop( QX11Info::display() );
	Atom win_desktop = XInternAtom( QX11Info::display(), "_NET_WM_DESKTOP", False );
	XChangeProperty( QX11Info::display(), window->winId(), win_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&desktop, 1 );
	// restore window's decoration, revoke always-on-top and remove taskbar and pager skipping
	window->setWindowFlags( window->windowFlags() & ~Qt::X11BypassWindowManagerHint & ~Qt::FramelessWindowHint & ~Qt::WindowStaysOnTopHint );
	Atom win_state = XInternAtom( QX11Info::display(), "_NET_WM_STATE", False );
	XChangeProperty( QX11Info::display(), window->winId(), win_state, XA_ATOM, 32, PropModeReplace, NULL, 0 );
	// change window type back to normal
	Atom atomproperty = XInternAtom( QX11Info::display(), "_NET_WM_WINDOW_TYPE", False );
	Atom atomvalue    = XInternAtom( QX11Info::display(), "_NET_WM_WINDOW_TYPE_NORMAL", False );
	XChangeProperty( QX11Info::display(), window->winId(), atomproperty, XA_ATOM, 32, PropModeReplace, (unsigned char *)&atomvalue, 1 );
	// disable window's size limits
	window->setMinimumSize( 0, 0 );
	window->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
	// restore old window's geometry
	window->setGeometry( oldGeometry );
	if( ! Core::instance()->isClosing() )  // if the module is being unloaded
	{
		// show Kadu normally
		window->show();
	}
}


bool EventFilter::eventFilter( QObject *o, QEvent *e )
{
	Q_UNUSED( o );
	if( ( e->type() == QEvent::MouseButtonPress ) )
	{
		QMouseEvent *me = (QMouseEvent*)e;
		if( Core::instance()->kaduWindow()->window()->geometry().contains( me->globalPos() ) )
		{
			// remove the event filter
			qApp->removeEventFilter( this );
			// activate Kadu's window
			_activateWindow( Core::instance()->kaduWindow()->window() );
		}
	}
	// forward the event
	return false;
}
