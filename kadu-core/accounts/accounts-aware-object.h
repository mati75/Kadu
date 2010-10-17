/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACCOUNTS_AWARE_OBJECT
#define ACCOUNTS_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class Account;

class KADUAPI AccountsAwareObject : public AwareObject<AccountsAwareObject>
{

protected:
	virtual void accountAdded(Account account);
	virtual void accountRemoved(Account account);
	virtual void accountRegistered(Account account) = 0;
	virtual void accountUnregistered(Account account) = 0;

public:
	static KADUAPI void notifyAccountAdded(Account account);
	static KADUAPI void notifyAccountRemoved(Account account);
	static KADUAPI void notifyAccountRegistered(Account account);
	static KADUAPI void notifyAccountUnregistered(Account account);
	
	void triggerAllAccountsAdded();
	void triggerAllAccountsRemoved();
	void triggerAllAccountsRegistered();
	void triggerAllAccountsUnregistered();

};

#endif // ACCOUNTS_AWARE_OBJECT
