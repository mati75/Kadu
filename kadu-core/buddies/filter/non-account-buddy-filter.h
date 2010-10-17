/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NON_ACCOUNT_BUDDY_FILTER_H
#define NON_ACCOUNT_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "accounts/account.h"

#include "abstract-buddy-filter.h"

class NonAccountBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	Account CurrentAccount;

public:
	explicit NonAccountBuddyFilter(QObject *parent = 0);
	virtual ~NonAccountBuddyFilter();

	void setAccount(Account account);
	virtual bool acceptBuddy(Buddy buddy);

};

Q_DECLARE_METATYPE(NonAccountBuddyFilter *)

#endif // NON_ACCOUNT_BUDDY_FILTER_H
