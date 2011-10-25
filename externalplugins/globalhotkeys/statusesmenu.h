/****************************************************************************
*                                                                           *
*   GlobalHotkeys plugin for Kadu                                           *
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


#ifndef STATUSESMENU_H
	#define STATUSESMENU_H


#include <QKeyEvent>
#include <QPointer>

#include "contacts/contact.h"
#include "gui/widgets/chat-widget.h"

#include "globalmenu.h"
#include "property.h"

#include "defines.h"


class StatusesMenuActionData
{
	public:
		StatusesMenuActionData();
		StatusesMenuActionData( StatusContainer* statuscontainer );
		StatusesMenuActionData( const StatusesMenuActionData &other );
		~StatusesMenuActionData();
		PROPERTY_RW( StatusContainer*, STATUSCONTAINER, statusContainer, setStatusContainer );
};
Q_DECLARE_METATYPE( StatusesMenuActionData );


class StatusesMenu : public GlobalMenu
{
	Q_OBJECT
	public:
		StatusesMenu();
		void popup( QPoint p = QPoint() );
	protected:
		virtual void keyPressEvent( QKeyEvent *event );
		virtual void mousePressEvent( QMouseEvent *event );
	private:
		void createMenu();
		void openSubmenu( QAction *action );
};


#endif
