/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_SIMLITE_PROVIDER_H
#define ENCRYPTION_NG_SIMLITE_PROVIDER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"

#include "modules/encryption_ng/encryption-provider.h"

class EncryptioNgSimliteProvider : public EncryptionProvider, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EncryptioNgSimliteProvider)

	static EncryptioNgSimliteProvider *Instance;

	EncryptioNgSimliteProvider();
	virtual ~EncryptioNgSimliteProvider();

private slots:
	void filterRawIncomingMessage(Chat chat, Contact sender, QByteArray &message, bool &ignore);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();

	static EncryptioNgSimliteProvider * instance() { return Instance; }

	virtual bool canDecrypt(const Chat &chat);
	virtual bool canEncrypt(const Chat &chat);

	virtual Decryptor * decryptor(const Chat &chat);
	virtual Encryptor * encryptor(const Chat &chat);

};

#endif // ENCRYPTION_NG_SIMLITE_PROVIDER_H
