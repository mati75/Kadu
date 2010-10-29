/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QHash>

#include "buddies/buddy-set.h"
#include "buddies/buddy-shared.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "status/status-group.h"
#include "status/status-type.h"

#include "debug.h"

#include "helpers/gadu-formatter.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-chat-service.h"

GaduChatService::GaduChatService(GaduProtocol *protocol)
	: ChatService(protocol), Protocol(protocol)
{
	// TODO: 0.6.6
// 	connect(protocol->socketNotifiers(), SIGNAL(ackReceived(int, uin_t, int)),
// 		this, SLOT(ackReceived(int, uin_t, int)));
}

bool GaduChatService::sendMessage(Chat chat, FormattedMessage &message, bool silent)
{
	kdebugf();

	QString plain = message.toPlain();
	QList<Contact> contacts = chat.contacts().toContactList();

	unsigned int uinsCount = 0;
	unsigned int formatsSize = 0;
	unsigned char *formats = GaduFormater::createFormats(Protocol->account(), message, formatsSize);
	bool stop = false;

	plain.replace("\r\n", "\n");
	plain.replace('\r', '\n');
	plain.replace(QChar::LineSeparator, "\n");

	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(plain));

	QByteArray data = plain.toUtf8();

	emit filterOutgoingMessage(chat, data, stop);

	if (stop)
	{
		if (formats)
			delete[] formats;

		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return false;
	}

	if (data.length() >= 2000)
	{
		if (formats)
			delete[] formats;

		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Filtered message too long (%1>=%2)").arg(data.length()).arg(2000));
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return false;
	}

	uinsCount = contacts.count();

	int messageId = -1;
	if (uinsCount > 1)
	{
		UinType uins[uinsCount];
		unsigned int i = 0;

		foreach (const Contact &contact, contacts)
			uins[i++] = Protocol->uin(contact);

		if (formatsSize)
			messageId = gg_send_message_confer_richtext(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data(),
					formats, formatsSize);
		else
			messageId = gg_send_message_confer(
					Protocol->gaduSession(), GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data());
	}
	else
		foreach (const Contact &contact, contacts)
		{
			if (formatsSize)
				messageId = gg_send_message_richtext(
						Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contact), (unsigned char *)data.data(),
						formats, formatsSize);
			else
				messageId = gg_send_message(
						Protocol->gaduSession(), GG_CLASS_CHAT, Protocol->uin(contact), (unsigned char *)data.data());

			break;
		}

	if (-1 == messageId)
		return false;

	if (formats)
		delete[] formats;


	if (!silent)
	{
		Message msg = Message::create();
		msg.setMessageChat(chat);
		msg.setType(Message::TypeSent);
		msg.setMessageSender(Protocol->account().accountContact());
		msg.setStatus(Message::StatusSent);
		msg.setContent(message.toHtml());
		msg.setSendDate(QDateTime::currentDateTime());
		msg.setReceiveDate(QDateTime::currentDateTime());
		msg.setId(messageId);

		UndeliveredMessages.insert(messageId, msg);
		emit messageSent(msg);
	}

	kdebugf2();
	return true;
}

bool GaduChatService::isSystemMessage(gg_event *e)
{
	if (0 == e->event.msg.sender)
	{
		kdebugmf(KDEBUG_INFO, "Ignored system message.\n");
	}

	return 0 == e->event.msg.sender;
}

Contact GaduChatService::getSender(gg_event *e)
{
	return ContactManager::instance()->byId(Protocol->account(), QString::number(e->event.msg.sender), ActionCreateAndAdd);
}

bool GaduChatService::ignoreSender(gg_event *e, Buddy sender)
{
	bool ignore =
			sender.isAnonymous() &&
			config_file.readBoolEntry("Chat", "IgnoreAnonymousUsers") &&
			(
				(e->event.msg.recipients_count == 0) ||
				config_file.readBoolEntry("Chat", "IgnoreAnonymousUsersInConferences")
			);

	if (ignore)
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", sender.id(Protocol->account()).toUInt());
	}

	return ignore;
}

ContactSet GaduChatService::getRecipients(gg_event *e)
{
	ContactSet recipients;
	for (int i = 0; i < e->event.msg.recipients_count; ++i)
		recipients.insert(ContactManager::instance()->byId(Protocol->account(), QString::number(e->event.msg.recipients[i]), ActionCreateAndAdd));

	return recipients;
}

QString GaduChatService::getContent(gg_event *e)
{
	QString content = QString::fromUtf8((const char *)e->event.msg.message);

	content.replace(QLatin1String("\r\n"), QString(QChar::LineSeparator));
	content.replace(QLatin1String("\n"),   QString(QChar::LineSeparator));
	content.replace(QLatin1String("\r"),   QString(QChar::LineSeparator));

	return content;
}

bool GaduChatService::ignoreRichText(gg_event *e, Contact sender)
{
	Q_UNUSED(e)

	bool ignore = sender.ownerBuddy().isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext");

	if (ignore)
	{
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");
	}

	return ignore;
}

bool GaduChatService::ignoreImages(gg_event *e, Contact sender)
{
	Q_UNUSED(e)

	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());

	return sender.ownerBuddy().isAnonymous() ||
		(
			"Offline" == Protocol->status().group() ||
			(
				("Invisible" == Protocol->status().group()) &&
				!gaduAccountDetails->receiveImagesDuringInvisibility()
			)
		);
}

FormattedMessage GaduChatService::createFormattedMessage(gg_event *e, Chat chat, Contact sender)
{
	QString content = getContent(e);

	bool ignore = false;
	emit filterRawIncomingMessage(chat, sender, content, ignore); //TODO: 0.6.6 + xhtml?
	if (ignore)
		return FormattedMessage();

	if (ignoreRichText(e, sender))
		return GaduFormater::createMessage(Protocol->account(), sender.id().toUInt(), content, 0, 0, false);
	else
		return GaduFormater::createMessage(Protocol->account(), sender.id().toUInt(), content,
				(unsigned char *)e->event.msg.formats, e->event.msg.formats_length, !ignoreImages(e, sender));
}

void GaduChatService::handleEventMsg(struct gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "recipients_count: %d\n", e->event.msg.recipients_count);

	if (isSystemMessage(e))
		return;

	Contact sender = getSender(e);
	if (ignoreSender(e, sender.ownerBuddy()))
		return;

	ContactSet recipients = getRecipients(e);

	ContactSet conference = recipients;
	conference += sender;

	ContactSet chatContacts = conference;
	chatContacts.remove(Protocol->account().accountContact());

	Chat chat = ChatManager::instance()->findChat(chatContacts);
	if (chat.isIgnoreAllMessages())
		return;

	FormattedMessage message = createFormattedMessage(e, chat, sender);
	if (message.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			sender.id().toUInt(), qPrintable(message.toPlain()));

	QDateTime time = QDateTime::fromTime_t(e->event.msg.time);

	bool ignore = false;
	emit filterIncomingMessage(chat, sender, message.toPlain(), time.toTime_t(), ignore);
	if (ignore)
		return;

	Message msg = Message::create();
	msg.setMessageChat(chat);
	msg.setType(Message::TypeReceived);
	msg.setMessageSender(sender);
	msg.setStatus(Message::StatusReceived);
	msg.setContent(message.toHtml());
	msg.setSendDate(time);
	msg.setReceiveDate(QDateTime::currentDateTime());
	emit messageReceived(msg);
}

void GaduChatService::handleEventAck(struct gg_event *e)
{
	kdebugf();

	int messageId = e->event.ack.seq;
	if (!UndeliveredMessages.contains(messageId))
		return;

	int uin = e->event.ack.recipient;
	Q_UNUSED(uin) // only in debug mode

	Message::Status status = Message::StatusUnknown;
	switch (e->event.ack.status)
	{
		case GG_ACK_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message delivered (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedDelivered);
			status = Message::StatusDelivered;
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusAcceptedQueued);
			status = Message::StatusDelivered;
			break;
		case GG_ACK_BLOCKED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message blocked (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBlocked);
			status = Message::StatusWontDeliver;
			break;
		case GG_ACK_MBOXFULL:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message box full (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedBoxFull);
			status = Message::StatusWontDeliver;
			break;
		case GG_ACK_NOT_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message not delivered (uin: %d, seq: %d)\n", uin, messageId);
			emit messageStatusChanged(messageId, StatusRejectedUnknown);
			status = Message::StatusWontDeliver;
			break;
		default:
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %d, seq: %d, status:%d)\n", uin, messageId, e->event.ack.status);
			break;
	}

	UndeliveredMessages[messageId].setStatus(status);
	UndeliveredMessages.remove(messageId);

	removeTimeoutUndeliveredMessages();

	kdebugf2();
}

void GaduChatService::removeTimeoutUndeliveredMessages()
{
// TODO: move to const or something
	#define MAX_DELIVERY_TIME 60

	QDateTime now = QDateTime();
	QList<int> toRemove;

	QHash<int, Message>::const_iterator message = UndeliveredMessages.constBegin();
	QHash<int, Message>::const_iterator end = UndeliveredMessages.constEnd();
	for (; message != end; message++)
	{
		if (message.value().sendDate().addSecs(MAX_DELIVERY_TIME) < now)
		{
			toRemove.append(message.key());
			UndeliveredMessages[message.key()].setStatus(Message::StatusWontDeliver);
		}
	}

	foreach (int messageId, toRemove)
		UndeliveredMessages.remove(messageId);
}
