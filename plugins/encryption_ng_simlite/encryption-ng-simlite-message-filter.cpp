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

#include "message/message.h"

#include "encryption-ng-simlite-message-filter.h"

#define RSA_PUBLIC_KEY_BEGIN "-----BEGIN RSA PUBLIC KEY-----"

EncryptionNgSimliteMessageFilter::EncryptionNgSimliteMessageFilter(QObject *parent) :
		MessageFilter(parent)
{
}

EncryptionNgSimliteMessageFilter::~EncryptionNgSimliteMessageFilter()
{
}

bool EncryptionNgSimliteMessageFilter::acceptMessage(const Message &message)
{
	if (MessageTypeSent == message.type())
		return true;

	if (!message.plainTextContent().startsWith(RSA_PUBLIC_KEY_BEGIN))
		return true;

	emit keyReceived(message.messageSender(), "simlite", message.plainTextContent().toUtf8());
	return false;
}

#include "moc_encryption-ng-simlite-message-filter.cpp"