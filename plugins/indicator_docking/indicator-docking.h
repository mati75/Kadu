/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>

class ChatManager;
class ChatWidgetManager;
class Message;
class QMessagingMenuApp;
class StatusContainer;
class UnreadMessageRepository;
enum class QMessagingMenuStatus;

class IndicatorDocking : public QObject
{
	Q_OBJECT

public:
	explicit IndicatorDocking(QObject *parent = nullptr);
	virtual ~IndicatorDocking();

	void setChatManager(ChatManager *chatManager);
	void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	void setStatusContainer(StatusContainer *statusContainer);
	void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<StatusContainer> m_statusContainer;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

	QMessagingMenuApp *m_messagingMenuApp;

private slots:
	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

	void sourceActivated(const QString &id);
	void statusChanged(QMessagingMenuStatus status);
	void statusContainerUpdated(StatusContainer *statusContainer);

};
