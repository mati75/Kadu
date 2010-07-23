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

#include <libgadu.h>

#include "protocols/protocol.h"
#include "debug.h"

#include "gadu-protocol.h"

#include "gadu-contact-list-handler.h"

int GaduContactListHandler::notifyTypeFromContact(const Contact &contact)
{
	Buddy buddy = contact.ownerBuddy();

	return buddy.isOfflineTo()
		? GG_USER_OFFLINE
		: buddy.isBlocked()
			? GG_USER_BLOCKED
			: GG_USER_NORMAL;
}

GaduContactListHandler::GaduContactListHandler(GaduProtocol *protocol, QObject *parent) :
		QObject(parent), Protocol(protocol)
{
}

GaduContactListHandler::~GaduContactListHandler()
{
}

void GaduContactListHandler::setUpContactList(QList<Contact> contacts)
{
	/*
	 * it looks like gadu-gadu now stores contact list mask (offlineto, blocked, normal)
	 * on server, so need to remove this mask and send a new one for each contact, so
	 * server has up-to-date information about our contact list
	 */

	// send empty list
	gg_notify_ex(Protocol->gaduSession(), 0, 0, 0);

	// send all items
	foreach (const Contact &contact, contacts)
		addContactEntry(contact);

/*
	UinType *uins;
	char *types;

	if (contacts.isEmpty())
	{
		debugMessage("  contact list is empty");

		gg_notify_ex(Protocol->gaduSession(), 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	int count = contacts.count();
	debugMessage(QString("  will send %1 elements").arg(count));

	uins = new UinType[count];
	types = new char[count];

	int i = 0;

	foreach (const Contact &contact, contacts)
	{
		debugMessage(QString("  sending %1 with %2").arg(Protocol->uin(contact)).arg(notifyTypeFromContact(contact)));

		uins[i] = Protocol->uin(contact);
		types[i] = notifyTypeFromContact(contact);
		++i;
	}

	gg_notify_ex(Protocol->gaduSession(), uins, types, count);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;
*/
}

void GaduContactListHandler::updateContactEntry(Contact contact)
{
	addContactEntry(contact);
}

void GaduContactListHandler::addContactEntry(int uin, int type)
{
	if (!uin)
		return;

	gg_session *session = Protocol->gaduSession();
	if (!session)
		return;

	gg_remove_notify_ex(session, uin, GG_USER_NORMAL);
	gg_remove_notify_ex(session, uin, GG_USER_BLOCKED);
	gg_remove_notify_ex(session, uin, GG_USER_OFFLINE);

	gg_add_notify_ex(session, uin, type);
}

void GaduContactListHandler::addContactEntry(Contact contact)
{
	addContactEntry(Protocol->uin(contact), notifyTypeFromContact(contact));
}

void GaduContactListHandler::removeContactEntry(int uin)
{
	if (!uin)
		return;

	gg_session *session = Protocol->gaduSession();
	if (!session)
		return;

	gg_remove_notify_ex(session, uin, GG_USER_NORMAL);
	gg_remove_notify_ex(session, uin, GG_USER_BLOCKED);
	gg_remove_notify_ex(session, uin, GG_USER_OFFLINE);
}

void GaduContactListHandler::removeContactEntry(Contact contact)
{
	removeContactEntry(Protocol->uin(contact));
}
