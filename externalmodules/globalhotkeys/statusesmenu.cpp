/****************************************************************************
*                                                                           *
*   GlobalHotkeys module for Kadu                                           *
*   Copyright (C) 2008-2011  Piotr Dąbrowski ultr@ultr.pl                   *
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

#include "activate.h"
#include "gui/widgets/status-menu.h"
#include "status/status-container-manager.h"

#include "statusesmenu.h"




StatusesMenuActionData::StatusesMenuActionData()
{
	STATUSCONTAINER = NULL;
}


StatusesMenuActionData::StatusesMenuActionData( StatusContainer* statuscontainer )
{
	STATUSCONTAINER = statuscontainer;
}


StatusesMenuActionData::StatusesMenuActionData( const StatusesMenuActionData &other )
{
	STATUSCONTAINER = other.STATUSCONTAINER;
}


StatusesMenuActionData::~StatusesMenuActionData() {}




StatusesMenu::StatusesMenu() : GlobalMenu()
{
	createMenu();
}


void StatusesMenu::createMenu()
{
	clear();
	if( StatusContainerManager::instance()->statusContainers().count() == 0 )
	{
		QAction *action = new QAction( QIcon(), qApp->translate( "@default", "no accounts defined" ), this );
		action->setEnabled( false );
		addAction( action );
	}
	else if( StatusContainerManager::instance()->statusContainers().count() == 1 )
	{
		new StatusMenu( StatusContainerManager::instance()->statusContainers()[0], this );
	}
	else
	{
		foreach( StatusContainer *statuscontainer, StatusContainerManager::instance()->statusContainers() )
		{
			QAction *action = new QAction( statuscontainer->statusIcon(), statuscontainer->statusContainerName(), this );
			QVariant variant;
			variant.setValue( StatusesMenuActionData( statuscontainer ) );
			action->setData( variant );
			action->setIconVisibleInMenu( true );
			addAction( action );
		}
		addSeparator();
		new StatusMenu( StatusContainerManager::instance(), this );
	}
}


void StatusesMenu::popup( QPoint p )
{
	// set active item
	if( ACTIONTOACTIVATE == NULL )
	{
		if( actions().count() > 0 )
		{
			QList<QAction*> actionslist = actions();
			actionslist.removeLast();
			foreach( QAction *action, actionslist )
			{
				if( action->isChecked() )
				{
					setActionToActivate( action );
					break;
				}
			}
		}
	}
	// popup
	GlobalMenu::popup( p );
}


void StatusesMenu::keyPressEvent( QKeyEvent *event )
{
	QAction *action = activeAction();
	if( activeAction() != NULL )
	{
		QVariant variant = action->data();
		if( ( ! variant.isNull() ) && ( variant.canConvert<StatusesMenuActionData>() ) )
		{
			StatusesMenuActionData data = variant.value<StatusesMenuActionData>();
			if( ( event->key() == Qt::Key_Right ) || ( event->key() == Qt::Key_Return ) )
			{
				timerStop();
				timerLock();
				if( ! SUBMENU.isNull() )
				{
					_activateWindow( this );
					SUBMENU->close();
				}
				openSubmenu( action );
				timerStart();
				return;
			}
		}
	}
	GlobalMenu::keyPressEvent( event );
}


void StatusesMenu::mousePressEvent( QMouseEvent *event )
{
	QAction *action = actionAt( event->pos() );
	if( action != NULL )
	{
		setActiveAction( action );
		QVariant variant = action->data();
		if( ( ! variant.isNull() ) && ( variant.canConvert<StatusesMenuActionData>() ) )
		{
			StatusesMenuActionData data = variant.value<StatusesMenuActionData>();
			timerStop();
			timerLock();
			if( ! SUBMENU.isNull() )
			{
				_activateWindow( this );
				SUBMENU->close();
			}
			openSubmenu( action );
			timerStart();
			return;
		}
	}
	GlobalMenu::mousePressEvent( event );
}


void StatusesMenu::openSubmenu( QAction *action )
{
	// data
	StatusesMenuActionData data = action->data().value<StatusesMenuActionData>();
	// submenu
	GlobalMenu *submenu = new GlobalMenu();
	SUBMENU = submenu;
	submenu->setParentMenu( this );
	new StatusMenu( data.statusContainer(), submenu );
	// set active item
	QList<QAction*> actions = submenu->actions();
	actions.removeLast();
	foreach( QAction *action, actions )
	{
		if( action->isChecked() )
		{
			submenu->setActionToActivate( action );
			break;
		}
	}
	// close this menu when submenu's action is triggered
	connect( submenu, SIGNAL(triggered(QAction*)), this, SLOT(close()) );
	// popup
	submenu->popup( pos() + actionGeometry( action ).topRight() );
}
