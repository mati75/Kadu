/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SUBSCRIPTION_SERVICE_H
#define SUBSCRIPTION_SERVICE_H

#include <QtCore/QObject>

#include "contacts/contact.h"

class SubscriptionService : public QObject
{
	Q_OBJECT

public:
	explicit SubscriptionService(QObject *parent) : QObject(parent) {}

	virtual void resendSubscription(const Contact &contact) = 0;
	virtual void removeSubscription(const Contact &contact) = 0;
	virtual void requestSubscription(const Contact &contact) = 0;

public slots:
	virtual void authorizeContact(Contact contact, bool authorized) = 0;

};

#endif // SUBSCRIPTION_SERVICE_H
