/****************************************************************************
*                                                                           *
*   GlobalHotkeys plugin for Kadu                                           *
*   Copyright (C) 2008-2012  Piotr Dąbrowski ultr@ultr.pl                   *
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
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

#include "activate.h"

#include "globalmenu.h"

#include "wideiconsmenu.h"




QTimer *GlobalMenu::INACTIVITYTIMER = NULL;


bool GlobalMenu::INACTIVITYTIMERLOCK = false;


GlobalMenu::GlobalMenu() : QMenu()
{
	if( INACTIVITYTIMER == NULL )
	{
		INACTIVITYTIMER = new QTimer();
		INACTIVITYTIMER->setInterval( GLOBALHOTKEYS_GLOBALWIDGETINACTIVITYTIMERINTERVAL );
		INACTIVITYTIMER->setSingleShot( true );
	}
	connect( INACTIVITYTIMER, SIGNAL(timeout()), this, SLOT(inactivitytimerTimeout()) );
	PARENTMENU = NULL;
	SUBMENU = NULL;
	ACTIONTOACTIVATE = NULL;
	setAttribute( Qt::WA_DeleteOnClose );
	setParent( 0, Qt::Window | Qt::X11BypassWindowManagerHint );
	setFocusPolicy( Qt::WheelFocus );
	connect( this, SIGNAL(triggered(QAction*)), this, SLOT(closeTopMostMenu()) );
}


void GlobalMenu::closeEvent( QCloseEvent *event )
{
	if( PARENTMENU.isNull() )
		timerStop();
	QMenu::closeEvent( event );
	if( ! SUBMENU.isNull() )
		SUBMENU->close();
}


void GlobalMenu::closeTopMostMenu()
{
	GlobalMenu *menu = this;
	while( ! menu->parentMenu().isNull() )
		menu = menu->parentMenu();
	menu->close();
}


void GlobalMenu::closeAllSubmenus()
{
	if( SUBMENU.isNull() )
		return;
	bool timeractive = INACTIVITYTIMER->isActive();
	if( timeractive )
	{
		// stop timer
		timerStop();
	}
	SUBMENU->closeAllSubmenus();
	_activateWindow( this );
	SUBMENU->close();
	if( timeractive )
	{
		// start timer
		timerLock();
		timerStart();
	}
}


void GlobalMenu::timerStart()
{
	INACTIVITYTIMER->start();
}


void GlobalMenu::timerStop()
{
	INACTIVITYTIMER->stop();
}


void GlobalMenu::timerLock()
{
	INACTIVITYTIMERLOCK = true;
}


void GlobalMenu::timerUnlock()
{
	INACTIVITYTIMERLOCK = false;
}


void GlobalMenu::hideEvent( QHideEvent *event )
{
	QMenu::hideEvent( event );
	close();
}


void GlobalMenu::focusInEvent( QFocusEvent *event )
{
	timerUnlock();
	QMenu::focusInEvent( event );
}


void GlobalMenu::popup( QPoint p )
{
	timerStop();
	LASTMOUSEPOS = QCursor::pos();
	if( ! p.isNull() )
		QMenu::popup( p );
	else
		QMenu::popup( QPoint(
			( qApp->desktop()->screenGeometry().width()  - sizeHint().width()  ) / 2,
			( qApp->desktop()->screenGeometry().height() - sizeHint().height() ) / 2
			) );
	if( ACTIONTOACTIVATE != NULL )
	{
		setActiveAction( ACTIONTOACTIVATE );
	}
	else
	{
		foreach( QAction *action, actions() )
		{
			if( action->isEnabled() )
			{
				setActiveAction( action );
				break;
			}
		}
	}
	QApplication::processEvents();
	_activateWindow( this );
	QApplication::processEvents();
	timerLock();
	timerStart();
}


void GlobalMenu::keyPressEvent( QKeyEvent *event )
{
	if( ! SUBMENU.isNull() )
	{
		_activateWindow( SUBMENU );
		return;
	}
	if( event->key() == Qt::Key_Left )
	{
		if( ! PARENTMENU.isNull() )
		{
			timerLock();
			_activateWindow( PARENTMENU );
			close();
			QApplication::processEvents();
		}
		return;
	}
	if( event->key() == Qt::Key_Escape )
	{
		// close the topmost parent menu (it will close all it's submenus)
		closeTopMostMenu();
		return;
	}
	QMenu::keyPressEvent( event );
}


void GlobalMenu::keyReleaseEvent( QKeyEvent *event )
{
	if( ! SUBMENU.isNull() )
	{
		_activateWindow( SUBMENU );
		return;
	}
	QMenu::keyReleaseEvent( event );
}


void GlobalMenu::mouseMoveEvent( QMouseEvent *event )
{
	event->ignore();
	if( event->globalPos() != LASTMOUSEPOS )
	{
		if( ! SUBMENU.isNull() )
			return;
		QAction *action = actionAt( event->pos() );
		if( action != NULL )
			setActiveAction( action );
	}
	LASTMOUSEPOS = event->globalPos();
}


void GlobalMenu::leaveEvent( QEvent *event )
{
	event->ignore();
}


void GlobalMenu::inactivitytimerTimeout()
{
	if( ! PARENTMENU.isNull() )
		return;
	GlobalMenu *menu = this;
	bool active = false;
	while( menu != NULL )
	{
		if( _isActiveWindow( menu ) )
		{
			active = true;
			break;
		}
		menu = menu->subMenu();
	}
	if( INACTIVITYTIMERLOCK )
	{
		if( ! active )
			_activateWindow( this );
		else
			timerUnlock();
	}
	else
	{
		if( ! active )
		{
			close();
			return;
		}
	}
	timerStart();
}
