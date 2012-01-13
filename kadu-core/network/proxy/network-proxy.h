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

#ifndef NETWORK_PROXY_H
#define NETWORK_PROXY_H

#include "network/proxy/network-proxy-shared.h"
#include "storage/shared-base.h"
#include "exports.h"

class KADUAPI NetworkProxy : public SharedBase<NetworkProxyShared>
{
	KaduSharedBaseClass(NetworkProxy)

public:
	static NetworkProxy create();
	static NetworkProxy loadStubFromStorage(const QSharedPointer<StoragePoint> &storage);
	static NetworkProxy loadFromStorage(const QSharedPointer<StoragePoint> &storage);
	static NetworkProxy null;

	NetworkProxy();
	NetworkProxy(NetworkProxyShared *data);
	NetworkProxy(QObject *data);
	NetworkProxy(const NetworkProxy &copy);

	virtual ~NetworkProxy();

	KaduSharedBase_PropertyCRW(QString, type, Type)
	KaduSharedBase_PropertyCRW(QString, address, Address)
	KaduSharedBase_Property(int, port, Port)
	KaduSharedBase_PropertyCRW(QString, user, User)
	KaduSharedBase_PropertyCRW(QString, password, Password)
	KaduSharedBase_PropertyCRW(QString, pollingUrl, PollingUrl)
	KaduSharedBase_PropertyRead(QString, displayName, DisplayName)

};

Q_DECLARE_METATYPE(NetworkProxy)

#endif // NETWORK_PROXY_H
