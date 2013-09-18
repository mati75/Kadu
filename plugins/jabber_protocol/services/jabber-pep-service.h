/* *
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006 Remko Troncon
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

#ifndef JABBER_PEP_SERVICE_H
#define JABBER_PEP_SERVICE_H

#include <QtCore/QWeakPointer>

#include "iris/xmpp_message.h"

class ServerInfoManager;

namespace XMPP
{
	class Client;

	class JabberProtocol;
}

class JabberPepService : public QObject
{
	Q_OBJECT

	typedef XMPP::Message Message;

	QWeakPointer<XMPP::Client> XmppClient;
	bool Enabled;

protected slots:
	void messageReceived(const Message &message);
	void getFinished();
	void publishFinished();

public:
	enum Access {
		DefaultAccess,
		PresenceAccess,
		PublicAccess
	};

	explicit JabberPepService(XMPP::JabberProtocol *protocol);
	virtual ~JabberPepService();

	XMPP::Client * xmppClient() const;

	void setEnabled(bool enabled);
	bool enabled() const { return Enabled; }

	void publish(const QString &node, const XMPP::PubSubItem &item, Access = DefaultAccess);
	void retract(const QString &node, const QString &id);
	void get(const XMPP::Jid &jid, const QString &node, const QString &id);

signals:
	void publishSuccess(const QString &ns, const XMPP::PubSubItem &item);
	void publishError(const QString &ns, const XMPP::PubSubItem &item);

	void itemPublished(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubItem &item);
	void itemRetracted(const XMPP::Jid &jid, const QString &node, const XMPP::PubSubRetraction &retraction);

};

#endif // JABBER_PEP_SERVICE_H