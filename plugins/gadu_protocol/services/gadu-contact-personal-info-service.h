/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_CONTACT_PERSONAL_INFO_SERVICE_H
#define GADU_CONTACT_PERSONAL_INFO_SERVICE_H

#include <libgadu.h>

#include "protocols/services/contact-personal-info-service.h"

class GaduConnection;

class GaduContactPersonalInfoService : public ContactPersonalInfoService
{
	Q_OBJECT

	QPointer<GaduConnection> Connection;
	unsigned int FetchSeq;
	QString Id;

	friend class GaduProtocolSocketNotifiers;
	void handleEventPubdir50Read(struct gg_event *e);

public:
	GaduContactPersonalInfoService(Account account, QObject *parent);

	void setConnection(GaduConnection *connection);

	virtual void fetchPersonalInfo(Contact contact);

};

#endif // GADU_CONTACT_PERSONAL_INFO_SERVICE_H

