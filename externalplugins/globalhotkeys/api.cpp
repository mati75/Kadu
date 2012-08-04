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


#include "buddies/buddy-preferred-manager.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-contact-set.h"

#include "api.h"


Account Api::commonAccount( BuddySet buddyset, ContactSet contactset )
{
	QList<Account> list = Api::commonAccounts( buddyset, contactset, 1 );
	if( ! list.isEmpty() )
		return list[0];
	else
		return Account::null;
}


QList<Account> Api::commonAccounts( BuddySet buddyset, ContactSet contactset, int limit )
{
	if( buddyset.isEmpty() && contactset.isEmpty() )
		return QList<Account>();
	Account account;
	if( contactset.count() > 0 )
	{
		account = (*contactset.begin()).contactAccount();
		foreach( Contact contact, contactset )
		{
			if( contact.contactAccount() != account )
				return QList<Account>();
		}
	}
	if( buddyset.count() == 0 )
	{
		QList<Account> list;
		list.append( account );
		return list;
	}
	QList<Account> possibleaccounts;
	if( ! account.isNull() )
	{
		possibleaccounts.append(account);
	}
	else
	{
		// check all preferred accounts...
		foreach( Buddy buddy, buddyset )
		{
			Account account = BuddyPreferredManager::instance()->preferredAccount( buddy );
			if( ! possibleaccounts.contains( account ) )
				possibleaccounts.append( account );
		}
		// ...and all accounts for the first buddy
		foreach( Account account, Api::accountsOfBuddy( *buddyset.begin() ) )
		{
			if( ! possibleaccounts.contains( account ) )
				possibleaccounts.append( account );
		}
	}
	QList<Account> list;
	foreach( Account account, possibleaccounts )
	{
		bool ok = true;
		foreach( Buddy buddy, buddyset )
		{
			if( buddy.contacts(account).isEmpty() )
			{
				ok = false;
				break;
			}
		}
		if (ok)
		{
			list.append( account );
			if( ( limit > 0 ) && ( list.count() >= limit ) )
				break;
		}
	}
	return list;
}


QList<Account> Api::accountsOfBuddy( Buddy buddy )
{
	QList<Account> accountslist;
	Account preferredaccount = BuddyPreferredManager::instance()->preferredAccount( buddy );
	if( preferredaccount != Account::null )
		accountslist.append(preferredaccount);
	foreach( Contact contact, buddy.contacts() )
	{
		if( ! accountslist.contains( contact.contactAccount() ) )
			accountslist.append( contact.contactAccount() );
	}
	return accountslist;
}


Chat Api::findChatForContactOrContactSet( ContactSet contactset, NotFoundAction notfoundaction )
{
	if( contactset.count() < 1 )
		return Chat::null;

	if( contactset.count() == 1 )
		return ChatTypeContact::findChat( contactset.toContact(), notfoundaction );

	return ChatTypeContactSet::findChat( contactset, notfoundaction );
}
