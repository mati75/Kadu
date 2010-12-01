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




#include "activate.h"

#include "globalwidgetmanager.h"




GlobalWidgetManager::GlobalWidgetManager( QWidget *widget, bool autostart )
{
	setParent( widget );
	WIDGET = widget->window();
	connect( WIDGET, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()) );
	INACTIVITYTIMER.setInterval( GLOBALHOTKEYS_GLOBALWIDGETINACTIVITYTIMERINTERVAL );
	INACTIVITYTIMER.setSingleShot( true );
	connect( &INACTIVITYTIMER, SIGNAL(timeout()), this, SLOT(inactivitytimerTimeout()) );
		FIRSTRUN = true;
	if( autostart )
		start();
}


GlobalWidgetManager::~GlobalWidgetManager()
{
}


void GlobalWidgetManager::start()
{
	if( FIRSTRUN )
	{
		if( ! WIDGET->isVisible() )
			WIDGET->show();
		_activateWindow( WIDGET );
		FIRSTRUN = false;
	}
	INACTIVITYTIMER.start();
}


void GlobalWidgetManager::stop()
{
	INACTIVITYTIMER.stop();
}


void GlobalWidgetManager::inactivitytimerTimeout()
{
	if( ! _isActiveWindow( WIDGET ) )
	{
		WIDGET->close();
		return;
	}
	INACTIVITYTIMER.start();
}


void GlobalWidgetManager::widgetDestroyed()
{
	stop();
	disconnect( &INACTIVITYTIMER, SIGNAL(timeout()), this, SLOT(inactivitytimerTimeout()) );
	WIDGET = NULL;
	deleteLater();
}
