/****************************************************************************
*                                                                           *
*   GlobalHotkeys module for Kadu                                           *
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
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

#include "activate.h"

#include "globalmenu.h"

#include "wideiconsmenu.h"




bool GlobalMenu::INACTIVITYTIMERLOCK = false;


GlobalMenu::GlobalMenu() : QMenu()
{
	PARENTMENU = NULL;
	SUBMENU = NULL;
	ACTIONTOACTIVATE = NULL;
	setAttribute( Qt::WA_DeleteOnClose );
	setParent( 0, Qt::Window | Qt::X11BypassWindowManagerHint );
	setFocusPolicy( Qt::WheelFocus );
	connect( this, SIGNAL(triggered(QAction*)), this, SLOT(close()) );
	INACTIVITYTIMER.setInterval( GLOBALHOTKEYS_GLOBALWIDGETINACTIVITYTIMERINTERVAL );
	INACTIVITYTIMER.setSingleShot( true );
	connect( &INACTIVITYTIMER, SIGNAL(timeout()), this, SLOT(inactivitytimerTimeout()) );
}


void GlobalMenu::closeEvent( QCloseEvent *event )
{
	timerStop();
	QMenu::closeEvent( event );
	if( ! SUBMENU.isNull() )
		SUBMENU->close();
	if( ! PARENTMENU.isNull() )
		_activateWindow( PARENTMENU );
}


void GlobalMenu::closeTopMostMenu()
{
	GlobalMenu *menu = this;
	while( menu->parentMenu() != NULL )
		menu = menu->parentMenu();
	menu->close();
}


void GlobalMenu::timerStart()
{
	INACTIVITYTIMER.start();
}


void GlobalMenu::timerStop()
{
	INACTIVITYTIMER.stop();
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
	close();
	QMenu::hideEvent( event );
}


void GlobalMenu::focusInEvent( QFocusEvent *event )
{
	timerUnlock();
	QMenu::focusInEvent( event );
}


void GlobalMenu::popup( QPoint p )
{
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
	_activateWindow( this );
	if( PARENTMENU.isNull() )
		timerStart();
}


void GlobalMenu::keyPressEvent( QKeyEvent *event )
{
	if( event->key() == Qt::Key_Left )
	{
		if( PARENTMENU != NULL )
		{
			timerStop();
			timerLock();
			close();
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


void GlobalMenu::mousePressEvent( QMouseEvent *event )
{
	QMenu::mousePressEvent( event );
}


void GlobalMenu::mouseMoveEvent( QMouseEvent *event )
{
	event->ignore();
	if( event->globalPos() != LASTMOUSEPOS )
	{
		if( ! SUBMENU.isNull() )
		{
			return;
		}
		QAction *action = actionAt( event->pos() );
		if( action != NULL )
		{
			setActiveAction( action );
		}
	}
	LASTMOUSEPOS = event->globalPos();
}


void GlobalMenu::leaveEvent( QEvent *event )
{
	event->ignore();
}


void GlobalMenu::inactivitytimerTimeout()
{
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
		if( active )
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
