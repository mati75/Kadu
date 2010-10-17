/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROTOCOL_MENU_MANAGER_H
#define PROTOCOL_MENU_MANAGER_H

#include <QtCore/QList>
#include "exports.h"

class Account;
class ActionDescription;
class Buddy;

class KADUAPI ProtocolMenuManager
{
	// TODO : remove or use it somewhere or extend to kadu menu
	QList<ActionDescription *> ContactsListActions;
	QList<ActionDescription *> ManagementActions;
	QList<ActionDescription *> ProtocolActions;

	ProtocolMenuManager();

public:
	const QList<ActionDescription *> & contactsListActions() const { return ContactsListActions; }
	const QList<ActionDescription *> & managementActions() const { return ManagementActions; }
	QList<ActionDescription *> protocolActions(Account account, Buddy buddy);
};

#endif // PROTOCOL_MENU_MANAGER_H
