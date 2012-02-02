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


#ifndef GLOBALMENU_H
	#define GLOBALMENU_H


#include <QMenu>
#include <QPointer>
#include <QTimer>

#include "property.h"

#include "defines.h"


class GlobalMenu : public QMenu
{
	Q_OBJECT
	public:
		GlobalMenu();
		void popup( QPoint p = QPoint() );
		void closeAllSubmenus();
		PROPERTY_RW( QPointer<GlobalMenu>, PARENTMENU      , parentMenu      , setParentMenu       );
		PROPERTY_RO( QPointer<GlobalMenu>, SUBMENU         , subMenu                               );
		PROPERTY_RW( QAction*            , ACTIONTOACTIVATE, actionToActivate, setActionToActivate );
	public slots:
		void closeTopMostMenu();
	protected:
		static void timerLock();
		static void timerStart();
		static void timerStop();
		static void timerUnlock();
		static bool INACTIVITYTIMERLOCK;
		static QTimer *INACTIVITYTIMER;
	protected:
		virtual void closeEvent( QCloseEvent *event );
		virtual void focusInEvent( QFocusEvent *event );
		virtual void hideEvent( QHideEvent *event );
		virtual void keyPressEvent( QKeyEvent *event );
		virtual void keyReleaseEvent( QKeyEvent *event );
		virtual void leaveEvent( QEvent *event );
		virtual void mouseMoveEvent( QMouseEvent *event );
	private slots:
		void inactivitytimerTimeout();
	private:
		QPoint LASTMOUSEPOS;
};


#endif
