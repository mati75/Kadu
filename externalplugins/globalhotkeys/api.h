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


#ifndef API_H
	#define API_H


#include <QList>

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "contacts/contact-set.h"


class Api
{
	public:
		static Account        commonAccount(  BuddySet buddyset, ContactSet contactset = ContactSet()                );
		static QList<Account> commonAccounts( BuddySet buddyset, ContactSet contactset = ContactSet(), int limit = 0 );
		static QList<Account> accountsOfBuddy( Buddy buddy );
};


#endif
