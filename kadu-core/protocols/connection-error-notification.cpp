/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "notify/notification-manager.h"
#include "notify/notify-event.h"
#include "parser/parser.h"
#include "debug.h"
#include "icons-manager.h"

#include "connection-error-notification.h"

NotifyEvent *ConnectionErrorNotification::ConnectionErrorNotifyEvent = 0;
QMap<Account, QStringList> ConnectionErrorNotification::ActiveErrors;

static QString getErrorMessage(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorMessage();
	else
		return "";
}

static QString getErrorServer(const QObject * const object)
{
	const ConnectionErrorNotification * const connectionErrorNotification = dynamic_cast<const ConnectionErrorNotification * const>(object);
	if (connectionErrorNotification)
		return connectionErrorNotification->errorServer();
	else
		return "";
}

void ConnectionErrorNotification::registerEvent()
{
	if (ConnectionErrorNotifyEvent)
		return;

	ConnectionErrorNotifyEvent = new NotifyEvent("ConnectionError", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Connection error"));
	NotificationManager::instance()->registerNotifyEvent(ConnectionErrorNotifyEvent);
	Parser::registerObjectTag("error", getErrorMessage);
	Parser::registerObjectTag("errorServer", getErrorServer);
}

void ConnectionErrorNotification::unregisterEvent()
{
	if (!ConnectionErrorNotifyEvent)
		return;

	Parser::unregisterObjectTag("errorServer", getErrorServer);
	Parser::unregisterObjectTag("error", getErrorMessage);

	NotificationManager::instance()->unregisterNotifyEvent(ConnectionErrorNotifyEvent);
	delete ConnectionErrorNotifyEvent;
	ConnectionErrorNotifyEvent = 0;
}

bool ConnectionErrorNotification::activeError(Account account, const QString &errorMessage)
{
	return ActiveErrors.contains(account) && ActiveErrors[account].contains(errorMessage);
}

ConnectionErrorNotification::ConnectionErrorNotification(Account account, const QString &errorServer, const QString &errorMessage) :
		AccountNotification(account, "ConnectionError", "dialog-error"),
		ErrorServer(errorServer), ErrorMessage(errorMessage)
{
	setTitle(tr("Connection error"));
	setText(tr("<b>Error:</b> (%1) %2").arg(ErrorServer).arg(ErrorMessage));

	ActiveErrors[account].append(ErrorMessage);
}

ConnectionErrorNotification::~ConnectionErrorNotification()
{
	ActiveErrors[account()].removeAll(ErrorMessage);
}

QString ConnectionErrorNotification::errorMessage() const
{
	kdebugf();

	return ErrorMessage;
}

QString ConnectionErrorNotification::errorServer() const
{
	kdebugf();

	return ErrorServer;
}
