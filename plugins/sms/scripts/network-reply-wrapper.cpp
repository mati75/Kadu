/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtNetwork/QNetworkReply>

#include "network-reply-wrapper.h"

NetworkReplyWrapper::NetworkReplyWrapper(QNetworkReply *reply) :
		QObject(reply), Reply(reply)
{
	connect(Reply, SIGNAL(finished()), this, SIGNAL(finished()));
}

NetworkReplyWrapper::~NetworkReplyWrapper()
{
}

bool NetworkReplyWrapper::ok()
{
	return Reply->error() == QNetworkReply::NoError;
}

QString NetworkReplyWrapper::content()
{
	return QString::fromUtf8(Reply->readAll());
}

QString NetworkReplyWrapper::redirect()
{
	return Reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().toString();
}

#include "moc_network-reply-wrapper.cpp"