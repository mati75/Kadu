/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QTextCodec>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/message/message-manager.h"
#include "chat/chat-details.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"

#include "debug.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "pending-messages-manager.h"

PendingMessagesManager * PendingMessagesManager::Instance = 0;

PendingMessagesManager * PendingMessagesManager::instance()
{
	if (0 == Instance)
	{
		Instance = new PendingMessagesManager();
		Instance->init();
	}

	return Instance;
}

PendingMessagesManager::PendingMessagesManager()
{
}

PendingMessagesManager::~PendingMessagesManager()
{
}

void PendingMessagesManager::init()
{
	connect(MessageManager::instance(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded(Message)));
	connect(MessageManager::instance(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved(Message)));
}

void PendingMessagesManager::unreadMessageAdded(const Message &message)
{
	if (items().contains(message))
		return;

	// message is not pending if it is in chat widget
	if (ChatWidgetManager::instance()->byChat(message.messageChat(), false))
		return;

	addItem(message);
}

void PendingMessagesManager::unreadMessageRemoved(const Message &message)
{
	removeItem(message);
}

void PendingMessagesManager::deletePendingMessagesForChat(const Chat &chat)
{
	QMutexLocker locker(&mutex());

	const QVector<Message> &messages = pendingMessagesForChat(chat);
	foreach (Message message, messages)
		removeItem(message);
}

QVector<Message> PendingMessagesManager::pendingMessagesForChat(const Chat &chat)
{
	QMutexLocker locker(&mutex());

	QVector<Message> result;
	QSet<Chat> chats;

	ChatDetails *details = chat.details();
	ChatDetailsAggregate *aggregateDetails = qobject_cast<ChatDetailsAggregate *>(details);
	if (aggregateDetails)
		foreach (const Chat &ch, aggregateDetails->chats())
			chats.insert(ch);
	else
		chats.insert(chat);

	foreach (const Message &message, items())
		if (chats.contains(message.messageChat()))
			result.append(message);

	return result;
}

void PendingMessagesManager::itemAboutToBeAdded(Message message)
{
	// just ensure that owner buddy is managed - we need it to be shown on contact list
	BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

	message.setPending(true);
}

void PendingMessagesManager::itemAboutToBeRemoved(Message message)
{
	message.setPending(false);
}

void PendingMessagesManager::loaded()
{
	SimpleManager<Message>::loaded();

	foreach (const Message &message, items())
	{
		// just ensure that all owner buddies are managed - we need them to be shown on contact list
		BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

		// each pending message is unread message of its chat
		MessageManager::instance()->addUnreadMessage(message);
	}
}
