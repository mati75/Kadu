/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "network-manager.h"
#include "network-aware-object.h"

#ifndef Q_OS_WIN
#include "network-manager-ntrack.h"
#else
#include "network-manager-qt.h"
#endif

NetworkManager *NetworkManager::Instance = 0;

NetworkManager * NetworkManager::instance()
{
	if (!Instance)
#ifndef Q_OS_WIN
		Instance = new NetworkManagerNTrack();
#else
		Instance = new NetworkManagerQt();
#endif
	return Instance;
}

NetworkManager::NetworkManager()
{
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::onlineStateChanged(bool isOnline)
{
	NetworkAwareObject::notifyOnlineStateChanged(isOnline);
	if (isOnline)
		emit online();
	else
		emit offline();
}
