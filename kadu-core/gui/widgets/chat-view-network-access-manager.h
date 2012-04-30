/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_VIEW_NETWORK_ACCESS_MANAGER
#define CHAT_VIEW_NETWORK_ACCESS_MANAGER

#include <QtNetwork/QNetworkAccessManager>

// taken from "Adding New Protocols to QtWebKit" article found in Qt Quarterly
// http://doc.trolltech.com/qq/32/qq32-webkit-protocols.html
class ChatViewNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT

public:
	explicit ChatViewNetworkAccessManager(QNetworkAccessManager *oldManager, QObject *parent = 0);

protected:
	virtual QNetworkReply * createRequest(Operation operation, const QNetworkRequest &request, QIODevice *device);

};

#endif // CHAT_VIEW_NETWORK_ACCESS_MANAGER