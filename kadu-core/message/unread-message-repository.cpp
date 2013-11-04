/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "chat/chat-details-buddy.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget.h"

#include "unread-message-repository.h"

UnreadMessageRepository::UnreadMessageRepository(QObject *parent) :
		QObject(parent)
{
	setState(StateNotLoaded);
	ConfigurationManager::instance()->registerStorableObject(this);
}

UnreadMessageRepository::~UnreadMessageRepository()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

bool UnreadMessageRepository::importFromPendingMessages()
{
	auto pendingMessagesNode = xml_config_file->getNode("PendingMessages", XmlConfigFile::ModeFind);
	if (pendingMessagesNode.isNull())
		return false;

	auto messageElements = xml_config_file->getNodes(pendingMessagesNode, "Message");
	foreach (const auto &messageElement, messageElements)
	{
		auto storagePoint = QSharedPointer<StoragePoint>(new StoragePoint(storage()->storage(), messageElement));
		auto uuid = QUuid(storagePoint->point().attribute("uuid"));
		if (!uuid.isNull())
		{
			auto message = Message::loadStubFromStorage(storagePoint);
			addUnreadMessage(message);

			// reset storage for message as it will be stored in other place
			message.data()->setStorage(QSharedPointer<StoragePoint>());
			message.data()->setState(StateNew);
		}
	}

	// PendingMessages is no longer needed
	pendingMessagesNode.parentNode().removeChild(pendingMessagesNode);

	return true;
}

void UnreadMessageRepository::load()
{
	StorableObject::load();

	if (importFromPendingMessages())
	{
		loaded();
		return;
	}

	auto itemsNode = storage()->point();
	if (itemsNode.isNull())
		return;

	auto itemElements = storage()->storage()->getNodes(itemsNode, "Message");
	UnreadMessages.reserve(itemElements.count());

	foreach (const auto &itemElement, itemElements)
	{
		auto storagePoint = QSharedPointer<StoragePoint>(new StoragePoint(storage()->storage(), itemElement));
		auto uuid = QUuid(storagePoint->point().attribute("uuid"));
		if (!uuid.isNull())
		{
			auto item = Message::loadStubFromStorage(storagePoint);
			addUnreadMessage(item);
		}
	}

	loaded();
}

void UnreadMessageRepository::store()
{
	ensureLoaded();

	foreach (auto message, UnreadMessages)
		message.ensureStored();
}

void UnreadMessageRepository::addUnreadMessage(const Message &message)
{
	// just ensure that owner buddy is managed - we need it to be shown on contact list
	// todo: rethink this one
	BuddyManager::instance()->byContact(message.messageSender(), ActionCreateAndAdd);

	auto chatWidget = ChatWidgetManager::instance()->byChat(message.messageChat(), false);
	// message is pending if chat widget is not open
	if (!chatWidget)
		message.setPending(true);
	else if (chatWidget->isActive())
		return;

	UnreadMessages.append(message);
	emit unreadMessageAdded(message);
}

void UnreadMessageRepository::removeUnreadMessage(const Message &message)
{
	UnreadMessages.removeAll(message);

	message.setPending(false);
	message.data()->removeFromStorage();

	emit unreadMessageRemoved(message);
}

const QList<Message> & UnreadMessageRepository::allUnreadMessages() const
{
	return UnreadMessages;
}

QVector<Message> UnreadMessageRepository::unreadMessagesForChat(const Chat &chat) const
{
	auto result = QVector<Message>();
	auto chats = QSet<Chat>();

	auto details = chat.details();
	auto chatDetailsBuddy = qobject_cast<ChatDetailsBuddy *>(details);

	if (chatDetailsBuddy)
		foreach (const auto &ch, chatDetailsBuddy->chats())
			chats.insert(ch);
	else
		chats.insert(chat);

	foreach (const auto &message, UnreadMessages)
		if (chats.contains(message.messageChat()))
			result.append(message);

	return result;
}

bool UnreadMessageRepository::hasUnreadMessages() const
{
	return !UnreadMessages.isEmpty();
}

quint16 UnreadMessageRepository::unreadMessagesCount() const
{
	return UnreadMessages.count();
}

void UnreadMessageRepository::markMessagesAsRead(const QVector<Message> &messages)
{
	foreach (const auto &message, messages)
		if (UnreadMessages.removeAll(message) > 0)
		{
			message.setStatus(MessageStatusRead);
			message.setPending(false);
			message.data()->removeFromStorage();

			emit unreadMessageRemoved(message);
		}
}

Message UnreadMessageRepository::unreadMessage() const
{
	if (UnreadMessages.empty())
		return Message::null;
	else
		return UnreadMessages.at(0);
}

Message UnreadMessageRepository::unreadMessageForBuddy(const Buddy &buddy) const
{
	auto contacts = buddy.contacts();
	foreach (const auto &message, UnreadMessages)
		if (contacts.contains(message.messageSender()))
			return message;

	return Message::null;
}

Message UnreadMessageRepository::unreadMessageForContact(const Contact &contact) const
{
	foreach (const auto &message, UnreadMessages)
		if (contact == message.messageSender())
			return message;

	return Message::null;
}

#include "moc_unread-message-repository.cpp"