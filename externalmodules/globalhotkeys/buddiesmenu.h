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


#ifndef BUDDIESMENU_H
	#define BUDDIESMENU_H


#include <QKeyEvent>
#include <QPointer>

#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget.h"

#include "globalmenu.h"
#include "property.h"

#include "defines.h"


enum BuddiesMenuType { BuddiesMenuTypeBuddies, BuddiesMenuTypeContacts };
enum ChatStateFlag
{
	ChatStateNone    = 0x0,
	ChatStateRecent  = 0x1,
	ChatStatePending = 0x2,
	ChatStateCurrent = 0x4,
	ChatStateActive  = 0x8
};
Q_DECLARE_FLAGS( ChatState, ChatStateFlag );
Q_DECLARE_OPERATORS_FOR_FLAGS( ChatState );


class BuddiesMenuActionData
{
	public:
		BuddiesMenuActionData();
		BuddiesMenuActionData( ContactSet contactset, ChatState chatstate, int initialorder = INT_MAX );
		BuddiesMenuActionData( const BuddiesMenuActionData &other );
		~BuddiesMenuActionData();
		bool operator<( const BuddiesMenuActionData &other ) const;
		Contact contact();
		bool    isConference();
		PROPERTY_RW( ContactSet, CONTACTSET  , contactSet  , setContactSet   );
		PROPERTY_RW( ChatState , CHATSTATE   , chatState   , setChatState    );
		PROPERTY_RW( int       , INITIALORDER, initialOrder, setInitialOrder );
};
Q_DECLARE_METATYPE( BuddiesMenuActionData );


class BuddiesMenu : public GlobalMenu
{
	Q_OBJECT
	public:
		BuddiesMenu();
		void add( ContactSet contactset );
		void add( Contact contact );
		void add( QList<Contact> contacts );
		bool contains( ContactSet contactset );
		bool contains( Contact contact );
		bool contains( QList<Contact> contacts );
		bool contains( Buddy buddy );
		bool contains( QString buddydisplay );
		void remove( ContactSet contactset );
		void remove( Contact contact );
		void remove( QList<Contact> contacts );
		void remove( Buddy buddy );
		void remove( QString buddydisplay );
		void clear();
		int count();
		void popup( QPoint p = QPoint() );
		void setContactToActivate( Contact contact );
		PROPERTY_RW( BuddiesMenuType, MENUTYPE            , menuType            , setMenuType             );
		PROPERTY_RW( bool           , CONTACTSSUBMENU     , contactsSubmenu     , setContactsSubmenu      );
		PROPERTY_RW( ContactSet     , CONTACTSETTOACTIVATE, contactSetToActivate, setContactSetToActivate );
	protected:
		virtual void keyPressEvent( QKeyEvent *event );
		virtual void mousePressEvent( QMouseEvent *event );
	private slots:
		void openChat();
	private:
		static bool caseInsensitiveLessThan( const QString &s1, const QString &s2 );
		QIcon createIcon( ContactSet contacts, ChatState chatstate );
		void openSubmenu( QAction *action );
		void prepareActions();
		QList<BuddiesMenuActionData> BUDDIESMENUACTIONDATALIST;
};


#endif
