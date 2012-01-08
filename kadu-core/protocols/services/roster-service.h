/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef ROSTER_SERVICE_H
#define ROSTER_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "contacts/contact.h"

#include "exports.h"

#include "protocols/services/protocol-service.h"

class KADUAPI RosterService : public ProtocolService
{
	Q_OBJECT

public:
	enum RosterState {
		StateNonInitialized,
		StateInitializing,
		StateInitialized,
		StateProcessingRemoteUpdate,
		StateProcessingLocalUpdate
	};

private:
	RosterState State;
	QList<Contact> Contacts;

private slots:
	void disconnected();
	void contactUpdated();

protected:
	virtual bool canPerformLocalUpdate() const;
	void setState(RosterState state);

protected slots:
	virtual void updateContact(const Contact &contact) = 0;

public:
	explicit RosterService(Protocol *protocol);
	virtual ~RosterService();

	RosterState state() const { return State; }

	virtual void prepareRoster() = 0;

public slots:
	virtual bool addContact(const Contact &contact);
	virtual bool removeContact(const Contact &contact);

signals:
	void rosterReady(bool ok);

};

#endif // ROSTER_SERVICE_H
