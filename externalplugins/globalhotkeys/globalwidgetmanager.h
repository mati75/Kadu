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


#ifndef GLOBALWIDGETMANAGER_H
	#define GLOBALWIDGETMANAGER_H


#include <QObject>
#include <QTimer>
#include <QWidget>

#include "property.h"

#include "defines.h"


class GlobalWidgetManager : public QObject
{
	Q_OBJECT;
	public:
		GlobalWidgetManager( QWidget *widget, bool autostart = true );
		~GlobalWidgetManager();
		PROPERTY_RO( QWidget*, WIDGET, widget );
	private slots:
		void inactivitytimerTimeout();
		void widgetDestroyed();
		void start();
		void stop();
	private:
		QTimer INACTIVITYTIMER;
		bool FIRSTRUN;
};


#endif
