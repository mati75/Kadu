/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_ADDITIONAL_DATA_DELETE_HANDLER_MANAGER_H
#define BUDDY_ADDITIONAL_DATA_DELETE_HANDLER_MANAGER_H

#include <QtCore/QObject>

class BuddyAdditionalDataDeleteHandler;

class BuddyAdditionalDataDeleteHandlerManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyAdditionalDataDeleteHandlerManager)

	static BuddyAdditionalDataDeleteHandlerManager * Instance;

	QList<BuddyAdditionalDataDeleteHandler *> Items;

	explicit BuddyAdditionalDataDeleteHandlerManager(QObject *parent = 0);
	virtual ~BuddyAdditionalDataDeleteHandlerManager();

public:
	static BuddyAdditionalDataDeleteHandlerManager * instance();

	QList<BuddyAdditionalDataDeleteHandler *> items() { return Items; }

	void registerAdditionalDataDeleteHandler(BuddyAdditionalDataDeleteHandler *handler);
	void unregisterAdditionalDataDeleteHandler(BuddyAdditionalDataDeleteHandler *handler);

signals:
	void additionalDataDeleteHandlerRegistered(BuddyAdditionalDataDeleteHandler *handler);
	void additionalDataDeleteHandlerUnregistered(BuddyAdditionalDataDeleteHandler *handler);

};

#endif // BUDDY_ADDITIONAL_DATA_DELETE_HANDLER_MANAGER_H
