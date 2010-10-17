/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "socket-notifiers/gadu-token-socket-notifiers.h"

#include "gadu-server-connector.h"

void GaduServerConnector::finished(bool result)
{
	Result = result;
	emit finished(this);
}

void GaduServerConnector::perform()
{
	H = gg_token(1);
	if (!H || H->fd <= 0)
	{
		tokenFetched(QString::null, QPixmap());
		return;
	}

	GaduTokenSocketNotifiers *sn = new GaduTokenSocketNotifiers();
	connect(sn, SIGNAL(done(const QString &, const QPixmap &)),
			this, SLOT(tokenFetched(const QString &, const QPixmap &)));
	sn->watchFor(H);
}

void GaduServerConnector::tokenFetched(const QString &tokenId, const QPixmap &tokenPixmap)
{
	if (Reader && !tokenId.isEmpty())
	{
		QString tokenValue = Reader->readToken(tokenPixmap);
		performAction(tokenId, tokenValue);
	}
	else
		finished(false);

	if (H)
	{
		delete H;
		H = 0;
	}
}
