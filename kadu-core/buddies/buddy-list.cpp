/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddy-list.h"

BuddyList::BuddyList()
{
}

BuddyList::BuddyList(QList<Buddy> list)
{
	*this += list;
}

BuddyList::BuddyList(Buddy buddy)
{
	append(buddy);
}

bool BuddyList::operator == (const BuddyList &compare) const
{
	if (size() != compare.size())
		return false;

	foreach (Buddy buddy, compare)
		if (!contains(buddy))
			return false;
	return true;
}
