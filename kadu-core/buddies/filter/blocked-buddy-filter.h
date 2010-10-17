/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef BLOCKED_BUDDY_FILTER_H
#define BLOCKED_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-buddy-filter.h"

class BlockedBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	bool Enabled;

public:
	explicit BlockedBuddyFilter(QObject *parent = 0);
	virtual ~BlockedBuddyFilter();

	void setEnabled(bool enabled);
	virtual bool acceptBuddy(Buddy buddy);

};

Q_DECLARE_METATYPE(BlockedBuddyFilter *)

#endif // BLOCKED_BUDDY_FILTER_H
