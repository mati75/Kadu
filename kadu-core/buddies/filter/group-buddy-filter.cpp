/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"

#include "group-buddy-filter.h"

GroupBuddyFilter::GroupBuddyFilter(QObject *parent) :
		AbstractBuddyFilter(parent), CurrentGroup(Group::null), AllGroupShown(true)
{
	// TODO: 0.6.6 hack, it should go thought the model itself
	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SIGNAL(filterChanged()));
}

void GroupBuddyFilter::setGroup(Group group)
{
	if (CurrentGroup == group)
		return;

	CurrentGroup = group;
	emit filterChanged();
}

bool GroupBuddyFilter::acceptBuddy(Buddy buddy)
{
	return !CurrentGroup // use AllGroupor UngroupedGroup
			? ((AllGroupShown && buddy.showInAllGroup()) || (!AllGroupShown && buddy.groups().isEmpty()))
			: buddy.isInGroup(CurrentGroup);
}

void GroupBuddyFilter::refresh()
{
	emit filterChanged();
}

void GroupBuddyFilter::setAllGroupShown(bool shown)
{
	AllGroupShown = shown;
}
