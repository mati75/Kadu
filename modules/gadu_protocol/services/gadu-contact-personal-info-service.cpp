/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "misc/misc.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-contact-personal-info-service.h"

GaduContactPersonalInfoService::GaduContactPersonalInfoService(GaduProtocol *protocol) :
		ContactPersonalInfoService(protocol), Protocol(protocol), FetchSeq(0)
{
}

void GaduContactPersonalInfoService::handleEventPubdir50Read(struct gg_event *e)
{
	gg_pubdir50_t res = e->event.pubdir50;

	if (FetchSeq != res->seq)
		return;

	int count = gg_pubdir50_count(res);
	if (1 != count)
	{
		emit personalInfoAvailable(Buddy::null);
		return;
	}

	Buddy result = Protocol->searchResultToBuddy(res, 0);
	emit personalInfoAvailable(result);
}

void GaduContactPersonalInfoService::fetchPersonalInfo(Contact contact)
{
	Id = contact.id();
	gg_pubdir50_t req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);
	gg_pubdir50_add(req, GG_PUBDIR50_UIN, qPrintable(Id));
	FetchSeq = gg_pubdir50(Protocol->gaduSession(), req);
	//gg_pubdir50_free(req);
}
