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




#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QStyleOption>

#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "chat/type/chat-type-manager.h"
#include "chat/chat-manager.h"
#include "chat/recent-chat-manager.h"
#include "contacts/model/contact-data-extractor.h"
#include "contacts/contact-set.h"
#include "contacts/contact-shared.h"
#include "gui/widgets/chat-widget-manager.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "status/status-container-manager.h"
#include "activate.h"

#include "buddiesmenu.h"

#include "wideiconsmenu.h"




BuddiesMenuActionData::BuddiesMenuActionData()
{
	SORTSTATELESS         = false;
	SORTSTATELESSBYSTATUS = false;
	CHATSTATE    = ChatStateNone;
	INITIALORDER = INT_MAX;
}


BuddiesMenuActionData::BuddiesMenuActionData( const BuddiesMenuActionData &other )
{
	SORTSTATELESS         = other.SORTSTATELESS;
	SORTSTATELESSBYSTATUS = other.SORTSTATELESSBYSTATUS;
	CONTACTSET   = other.CONTACTSET;
	CHATSTATE    = other.CHATSTATE;
	INITIALORDER = other.INITIALORDER;
}


BuddiesMenuActionData::~BuddiesMenuActionData() {}


bool BuddiesMenuActionData::operator<( const BuddiesMenuActionData &other ) const
{
	// returning 'true' puts the current item above the &other one in the list
	if( CHATSTATE == other.CHATSTATE )
	{
		Contact cc = CONTACTSET.toContact();
		Contact oc = other.CONTACTSET.toContact();
		if( cc && oc )
		{
			int cs = 0x0;
			if( cc.isBlocking()             ) cs += 0x1;
			if( cc.ownerBuddy().isBlocked() ) cs += 0x2;
			int os = 0x0;
			if( oc.isBlocking()             ) os += 0x1;
			if( oc.ownerBuddy().isBlocked() ) os += 0x2;
			if( cs != os )
				return ( cs > os );
			if( ( CHATSTATE == ChatStateNone ) && SORTSTATELESS ) // stateless sorting
			{
				if( SORTSTATELESSBYSTATUS )
					if( cc.currentStatus() != oc.currentStatus() )
						return oc.currentStatus() < cc.currentStatus();
				return cc.ownerBuddy().display().toLower() > oc.ownerBuddy().display().toLower();
			}
		}
		return ( INITIALORDER > other.INITIALORDER );
	}
	return ( CHATSTATE < other.CHATSTATE );
}


Contact BuddiesMenuActionData::contact()
{
	return CONTACTSET.toContact();
}


bool BuddiesMenuActionData::isConference()
{
	return ( CONTACTSET.count() > 1 );
}




BuddiesMenu::BuddiesMenu() : GlobalMenu()
{
	MENUTYPE = BuddiesMenuTypeBuddies;
	CONTACTSSUBMENU = true;
	ONEITEMPERBUDDY              = false;
	SORTSTATELESSBUDDIES         = false;
	SORTSTATELESSBUDDIESBYSTATUS = false;
	opensubmenuaction = NULL;
	int wideiconwidth =
		GLOBALHOTKEYS_BUDDIESMENUICONMARGINLEFT +
		GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE +
		GLOBALHOTKEYS_BUDDIESMENUICONSPACING +
		GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE +
		GLOBALHOTKEYS_BUDDIESMENUICONMARGINRIGHT;
	setStyle( new WideIconsMenu( wideiconwidth ) );
}


void BuddiesMenu::add( ContactSet contactset )
{
	if( contactset.isEmpty() )
		return;
	if( contains( contactset ) )
		return;
	foreach( Contact contact, contactset )
		if( ! contact.contactAccount() )
			return;
	// chatstate
	ChatState chatstate = ChatStateNone;
	Chat chat = ChatManager::instance()->findChat( contactset, false );
	if( ! chat.isNull() )
	{
		if( RecentChatManager::instance()->recentChats().contains( chat ) )
			chatstate |= ChatStateRecent;
		if( chat.unreadMessagesCount() > 0 )
			chatstate |= ChatStatePending;
		ChatWidget *chatwidget = ChatWidgetManager::instance()->byChat( chat, false );
		if( chatwidget != NULL )
		{
			chatstate |= ChatStateCurrent;
			if( chat.unreadMessagesCount() > 0 )
				chatstate |= ChatStatePending;
			if( _isActiveWindow( chatwidget ) )
				chatstate |= ChatStateActive;
		}
	}
	// data
	BuddiesMenuActionData data;
	data.setContactSet( contactset );
	data.setChatState( chatstate );
	// add data to buddiesmenuactiondatalist
	BUDDIESMENUACTIONDATALIST.append( data );
}


void BuddiesMenu::add( Contact contact )
{
	add( ContactSet( contact ) );
}


void BuddiesMenu::add( QVector<Contact> contacts )
{
	ContactSet contactset;
	contactset.unite( contacts.toList().toSet() );
	add( contactset );
}


bool BuddiesMenu::contains( ContactSet contactset )
{
	foreach( BuddiesMenuActionData data, BUDDIESMENUACTIONDATALIST )
	{
		if( data.contactSet() == contactset )
			return true;
	}
	return false;
}


bool BuddiesMenu::contains( Contact contact )
{
	return contains( ContactSet( contact ) );
}


bool BuddiesMenu::contains( QVector<Contact> contacts )
{
	ContactSet contactset;
	contactset.unite( contacts.toList().toSet() );
	return contains( contactset );
}


bool BuddiesMenu::contains( Buddy buddy )
{
	foreach( BuddiesMenuActionData data, BUDDIESMENUACTIONDATALIST )
	{
		Contact contact = data.contactSet().toContact();
		if( contact )
			if( contact.ownerBuddy() == buddy )
				return true;
	}
	return false;
}


bool BuddiesMenu::contains( QString buddydisplay )
{
	foreach( BuddiesMenuActionData data, BUDDIESMENUACTIONDATALIST )
	{
		Contact contact = data.contactSet().toContact();
		if( contact )
		{
			if( contact.ownerBuddy().display() == buddydisplay )
				return true;
		}
	}
	return false;
}


void BuddiesMenu::remove( ContactSet contactset )
{
	int i = 0;
	while( i < BUDDIESMENUACTIONDATALIST.size() )
	{
		if( BUDDIESMENUACTIONDATALIST[i].contactSet() == contactset )
		{
			BUDDIESMENUACTIONDATALIST.removeAt( i );
			continue;
		}
		++i;
	}
}


void BuddiesMenu::remove( Contact contact )
{
	remove( ContactSet( contact ) );
}


void BuddiesMenu::remove( QVector<Contact> contacts )
{
	ContactSet contactset;
	contactset.unite( contacts.toList().toSet() );
	remove( contactset );
}


void BuddiesMenu::remove( Buddy buddy )
{
	int i = 0;
	while( i < BUDDIESMENUACTIONDATALIST.size() )
	{
		Contact contact = BUDDIESMENUACTIONDATALIST[i].contactSet().toContact();
		if( contact )
		{
			if( contact.ownerBuddy() == buddy )
			{
				BUDDIESMENUACTIONDATALIST.removeAt( i );
				continue;
			}
		}
		++i;
	}
}


void BuddiesMenu::remove( QString buddydisplay )
{
	int i = 0;
	while( i < BUDDIESMENUACTIONDATALIST.size() )
	{
		Contact contact = BUDDIESMENUACTIONDATALIST[i].contactSet().toContact();
		if( contact )
		{
			if( contact.ownerBuddy().display() == buddydisplay )
			{
				BUDDIESMENUACTIONDATALIST.removeAt( i );
				continue;
			}
		}
		++i;
	}
}


void BuddiesMenu::clear()
{
	BUDDIESMENUACTIONDATALIST.clear();
	QMenu::clear();
}


int BuddiesMenu::count()
{
	return BUDDIESMENUACTIONDATALIST.count();
}


void BuddiesMenu::popup( QPoint p )
{
	if( count() == 0 )
	{
		QAction *action = new QAction( QIcon(), qApp->translate( "@default", "no buddies to display" ), this );
		action->setEnabled( false );
		addAction( action );
	}
	else
	{
		prepareActions();
	}
	GlobalMenu::popup( p );
}


void BuddiesMenu::setContactToActivate( Contact contact )
{
	setContactSetToActivate( ContactSet( contact ) );
}


QIcon BuddiesMenu::createIcon( ContactSet contactset, ChatState chatstate )
{
	QIcon chatstateicon;
	if( ( chatstate & ChatStatePending ) > 0x0 )
		chatstateicon = IconsManager::instance()->iconByPath( "protocols/common/message" );
	else if( ( chatstate & ChatStateCurrent ) > 0x0 )
		chatstateicon = IconsManager::instance()->iconByPath( "internet-group-chat" );
	else if( ( chatstate & ChatStateRecent ) > 0x0 )
		chatstateicon = IconsManager::instance()->iconByPath( "kadu_icons/history" );
	else
	{
		QPixmap emptypixmap( GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE, GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE );
		emptypixmap.fill( Qt::transparent );
		chatstateicon = QIcon( emptypixmap );
	}
	QIcon statusicon;
	if( contactset.count() == 1 )
	{
		Contact contact = *contactset.begin();
		statusicon = ContactDataExtractor::data( contact, Qt::DecorationRole, false ).value<QIcon>();
	}
	else
	{
		statusicon = ChatTypeManager::instance()->chatType( "Conference" )->icon().icon();
	}
	int wideiconwidth =
		GLOBALHOTKEYS_BUDDIESMENUICONMARGINLEFT +
		GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE +
		GLOBALHOTKEYS_BUDDIESMENUICONSPACING +
		GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE +
		GLOBALHOTKEYS_BUDDIESMENUICONMARGINRIGHT;
	int smalliconsize = GLOBALHOTKEYS_BUDDIESMENUSMALLICONSIZE;
	int left          = GLOBALHOTKEYS_BUDDIESMENUICONMARGINLEFT;
	int spacing       = GLOBALHOTKEYS_BUDDIESMENUICONSPACING;
	QPixmap pixmap( wideiconwidth, smalliconsize );
	pixmap.fill( Qt::transparent );
	QPainter painter( &pixmap );
	painter.drawPixmap(
			QRect( left, 0, smalliconsize, smalliconsize ),
			chatstateicon.pixmap( smalliconsize, smalliconsize ),
			QRect( 0, 0, smalliconsize, smalliconsize )
		);
	painter.drawPixmap(
			QRect( left+smalliconsize+spacing, 0, smalliconsize, smalliconsize ),
			statusicon.pixmap( smalliconsize, smalliconsize ),
			QRect( 0, 0, smalliconsize, smalliconsize )
		);
	return QIcon( pixmap );
}


void BuddiesMenu::openChat()
{
	// data
	BuddiesMenuActionData data = ((QAction*)sender())->data().value<BuddiesMenuActionData>();
	// close the topmost parent menu (it will close all it's submenus)
	closeTopMostMenu();
	// (re)open the chat with selected user(s)
	Chat chat = ChatManager::instance()->findChat( data.contactSet(), true );
	ChatWidgetManager::instance()->byChat( chat, true );
}


void BuddiesMenu::prepareActions()
{
	// sort buddiesmenuactiondatalist (only if it's not a contacts menu)
	if( MENUTYPE != BuddiesMenuTypeContacts )
	{
		for( int k1 = 0; k1 < BUDDIESMENUACTIONDATALIST.size(); ++k1 )
		{
			BUDDIESMENUACTIONDATALIST[k1].setSortStateless( SORTSTATELESSBUDDIES );
			BUDDIESMENUACTIONDATALIST[k1].setSortStatelessByStatus( SORTSTATELESSBUDDIESBYSTATUS );
			BUDDIESMENUACTIONDATALIST[k1].setInitialOrder( k1 );
		}
		qSort( BUDDIESMENUACTIONDATALIST.begin(), BUDDIESMENUACTIONDATALIST.end(), qGreater<BuddiesMenuActionData>() );
	}
	// remove duplicated buddies if requested
	if( ONEITEMPERBUDDY )
	{
		for( int k1 = 0; k1 < BUDDIESMENUACTIONDATALIST.size(); ++k1 )
		{
			ContactSet contactset1 = BUDDIESMENUACTIONDATALIST[k1].contactSet();
			BuddySet buddyset1 = contactset1.toBuddySet();
			for( int k2 = 0; k2 < BUDDIESMENUACTIONDATALIST.size(); ++k2 )
			{
				if( k2 == k1 )
					continue;
				ContactSet contactset2 = BUDDIESMENUACTIONDATALIST[k2].contactSet();
				BuddySet buddyset2 = contactset2.toBuddySet();
				if( buddyset1 == buddyset2 )
				{
					BUDDIESMENUACTIONDATALIST.removeAt( k2 );
					--k1;
					break;
				}
			}
		}
	}
	// search contactsets for contacts with repeated ownerBuddies and mark differences in contacts
	QList<ContactSet> unique_contacts_of_repeated_buddies_list;
	for( int k1 = 0; k1 < BUDDIESMENUACTIONDATALIST.size(); ++k1 )
	{
		ContactSet unique_contacts_of_repeated_buddies;
		ContactSet contactset1 = BUDDIESMENUACTIONDATALIST[k1].contactSet();
		BuddySet buddyset1 = contactset1.toBuddySet();
		for( int k2 = 0; k2 < BUDDIESMENUACTIONDATALIST.size(); ++k2 )
		{
			if( k2 == k1 )
				continue;
			ContactSet contactset2 = BUDDIESMENUACTIONDATALIST[k2].contactSet();
			BuddySet buddyset2 = contactset2.toBuddySet();
			if( buddyset1 == buddyset2 )
			{
				unique_contacts_of_repeated_buddies.unite( contactset1.subtract( contactset2 ) );
			}
		}
		unique_contacts_of_repeated_buddies_list.insert( k1, unique_contacts_of_repeated_buddies );
	}
	// remove old actions
	QMenu::clear(); // this way!
	// create actions
	for( int k = 0; k < BUDDIESMENUACTIONDATALIST.size(); ++k )
	{
		BuddiesMenuActionData data = BUDDIESMENUACTIONDATALIST.at( k );
		// icon
		QIcon icon = createIcon( data.contactSet(), data.chatState() );
		// caption
		QString caption = "";
		if( MENUTYPE == BuddiesMenuTypeBuddies )
		{
			QStringList names;
			foreach( Contact contact, data.contactSet() )
			{
				QString name = contact.ownerBuddy().display();
				if( unique_contacts_of_repeated_buddies_list.at( k ).contains( contact ) )
					name += " (" + contact.id() + ")";
				names << name;
			}
			qSort( names.begin(), names.end(), BuddiesMenu::caseInsensitiveLessThan );
			caption = QStringList( names.mid( 0, 5 ) ).join(", ");
			if( names.count() > 5 )
				caption += ", ...";
		}
		else if( MENUTYPE == BuddiesMenuTypeContacts )
		{
			Contact contact = data.contact();
			caption = contact.id();
		}
		// action
		QAction *action = new QAction( icon, caption, this );
		action->setIconVisibleInMenu( true );
		QVariant variant;
		variant.setValue( data );
		action->setData( variant );
		// mark blocked contact
		if( ! data.isConference() )
		{
			if( data.contact().ownerBuddy().isBlocked() )
			{
				QFont font = action->font();
				font.setItalic( true );
				action->setFont( font );
			}
		}
		// connect action
		connect( action, SIGNAL(triggered(bool)), this, SLOT(openChat()) );
		// add action
		addAction( action );
		// set action to activate if requested
		if( ( ! CONTACTSETTOACTIVATE.isEmpty() ) && ( data.contactSet() == CONTACTSETTOACTIVATE ) )
		{
			setActionToActivate( action );
		}
	}
}


void BuddiesMenu::keyPressEvent( QKeyEvent *event )
{
	if( event->key() == Qt::Key_Right )
	{
		if( ( MENUTYPE == BuddiesMenuTypeBuddies ) && ( activeAction() != NULL ) )
		{
			QAction *action = activeAction();
			openSubmenu( action );
		}
		return;
	}
	GlobalMenu::keyPressEvent( event );
}


void BuddiesMenu::mousePressEvent( QMouseEvent *event )
{
	if( event->button() == Qt::RightButton )
	{
		if( MENUTYPE == BuddiesMenuTypeBuddies )
		{
			QAction *action = actionAt( event->pos() );
			if( action != NULL )
				setActiveAction( action );
			openSubmenu( action );
		}
		return;
	}
	GlobalMenu::mousePressEvent( event );
}


void BuddiesMenu::openSubmenu( QAction *action )
{
	if( action == NULL )
		return;
	// data
	BuddiesMenuActionData data = action->data().value<BuddiesMenuActionData>();
	if( data.isConference() || CONTACTSSUBMENU )
	{
		// stop timer
		timerStop();
		timerLock();
		// check current submenu
		if( ( action == opensubmenuaction ) && ( ! SUBMENU.isNull() ) && SUBMENU->isVisible() )
		{
			_activateWindow( SUBMENU );
			timerStart();
			return;
		}
		// close other submenus
		if( ! SUBMENU.isNull() )
		{
			_activateWindow( this );
			SUBMENU->close();
		}
		// submenu
		opensubmenuaction = action;
		BuddiesMenu *buddiessubmenu = new BuddiesMenu();
		buddiessubmenu->setContactsSubmenu( CONTACTSSUBMENU );
		SUBMENU = buddiessubmenu;
		buddiessubmenu->setParentMenu( this );
		if( ! data.isConference() )
		{
			buddiessubmenu->setMenuType( BuddiesMenuTypeContacts );
			buddiessubmenu->add( BuddyPreferredManager::instance()->preferredContact( data.contact().ownerBuddy() ) );
			foreach( Contact contact, data.contact().ownerBuddy().contacts() )
				buddiessubmenu->add( contact );
			buddiessubmenu->setContactToActivate( data.contact() );
		}
		else
		{
			foreach( Contact contact, data.contactSet() )
				buddiessubmenu->add( contact );
		}
		buddiessubmenu->popup( pos() + actionGeometry( action ).topRight() );
		// start timer
		timerStart();
	}
}


bool BuddiesMenu::caseInsensitiveLessThan( const QString &s1, const QString &s2 )
{
	return s1.toLower() < s2.toLower();
}
