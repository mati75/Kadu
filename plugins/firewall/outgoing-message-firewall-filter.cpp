/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "firewall.h"

#include "outgoing-message-firewall-filter.h"

OutgoingMessageFirewallFilter::OutgoingMessageFirewallFilter(Firewall *firewall) :
		MessageFilter(firewall), CurrentFirewall(firewall)
{
}

OutgoingMessageFirewallFilter::~OutgoingMessageFirewallFilter()
{
}

bool OutgoingMessageFirewallFilter::acceptMessage(const Chat &chat, const Contact &sender, const QString &message)
{
	Q_UNUSED(sender);

	return CurrentFirewall->acceptOutgoingMessage(chat, message);
}