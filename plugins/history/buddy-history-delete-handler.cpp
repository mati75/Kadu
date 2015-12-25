/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QCoreApplication>

#include "talkable/talkable.h"

#include "history.h"

#include "buddy-history-delete-handler.h"

BuddyHistoryDeleteHandler * BuddyHistoryDeleteHandler::Instance = 0;

void BuddyHistoryDeleteHandler::createInstance()
{
	if (!Instance)
		Instance = new BuddyHistoryDeleteHandler();
}

void BuddyHistoryDeleteHandler::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

BuddyHistoryDeleteHandler * BuddyHistoryDeleteHandler::instance()
{
	return Instance;
}

BuddyHistoryDeleteHandler::BuddyHistoryDeleteHandler()
{
}

BuddyHistoryDeleteHandler::~BuddyHistoryDeleteHandler()
{
}

QString BuddyHistoryDeleteHandler::name()
{
	return QLatin1String("history-delete-handler");
}

QString BuddyHistoryDeleteHandler::displayName()
{
	return QCoreApplication::translate("BuddyHistoryDeleteHandler", "Chat history");
}

void BuddyHistoryDeleteHandler::deleteBuddyAdditionalData(Buddy buddy)
{
	if (History::instance()->currentStorage())
		History::instance()->currentStorage()->deleteHistory(buddy);
}
