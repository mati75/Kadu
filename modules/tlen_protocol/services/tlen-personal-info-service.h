/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef TLEN_PERSONAL_INFO_SERVICE_H
#define TLEN_PERSONAL_INFO_SERVICE_H

#include <QDomNodeList>

#include "protocols/services/personal-info-service.h"

class TlenProtocol;
class tlen;

class TlenPersonalInfoService : public PersonalInfoService
{
	Q_OBJECT

	TlenProtocol *Protocol;
	tlen * client;

private slots:
	void handlePubdirReceived(QDomNodeList node);

public:
	TlenPersonalInfoService(TlenProtocol *protocol);

	virtual void fetchPersonalInfo();
	virtual void updatePersonalInfo(Buddy buddy);

};

#endif // TLEN_PERSONAL_INFO_SERVICE_H

