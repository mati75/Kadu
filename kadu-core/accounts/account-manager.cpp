/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-shared.h"
#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "notify/notification-manager.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "debug.h"

#include "account-manager.h"

AccountManager * AccountManager::Instance = 0;

KADUAPI AccountManager * AccountManager::instance()
{
	if (0 == Instance)
		Instance = new AccountManager();

	return Instance;
}

AccountManager::AccountManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);
}

AccountManager::~AccountManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

void AccountManager::itemAdded(Account item)
{
	QMutexLocker(&mutex());

	if (item.data())
		item.data()->ensureLoaded();
	AccountsAwareObject::notifyAccountAdded(item);
}

void AccountManager::itemAboutToBeRemoved(Account item)
{
	QMutexLocker(&mutex());

	Manager<Account>::itemAboutToBeRemoved(item);

	item.setAccountIdentity(Identity::null);
}

void AccountManager::itemRemoved(Account item)
{
	QMutexLocker(&mutex());

	AccountsAwareObject::notifyAccountRemoved(item);
}

void AccountManager::itemAboutToBeRegistered(Account item)
{
	QMutexLocker(&mutex());

	connect(item, SIGNAL(updated()), this, SLOT(accountDataUpdated()));
	emit accountAboutToBeRegistered(item);
}

void AccountManager::itemRegistered(Account item)
{
	QMutexLocker(&mutex());

	AccountsAwareObject::notifyAccountRegistered(item);
	connect(item.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));

	emit accountRegistered(item);
}

void AccountManager::itemAboutToBeUnregisterd(Account item)
{
	QMutexLocker(&mutex());

	AccountsAwareObject::notifyAccountUnregistered(item);
	disconnect(item.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));

	emit accountAboutToBeUnregistered(item);
}

void AccountManager::itemUnregistered(Account item)
{
	QMutexLocker(&mutex());

	disconnect(item, SIGNAL(updated()), this, SLOT(accountDataUpdated()));
	emit accountUnregistered(item);
}

void AccountManager::detailsLoaded(Account account)
{
	QMutexLocker(&mutex());

	if (!account.isNull())
		registerItem(account);
}

void AccountManager::detailsUnloaded(Account account)
{
	QMutexLocker(&mutex());

	if (!account.isNull())
		unregisterItem(account);
}

Account AccountManager::defaultAccount()
{
	QMutexLocker(&mutex());

	ensureLoaded();
	return byIndex(0);
}

const QList<Account> AccountManager::byIdentity(Identity identity)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	QList<Account> list;
	foreach (Account account, allItems())
		if (account.accountIdentity() == identity)
			list.append(account);

	return list;
}

Account AccountManager::byId(const QString& protocolName, const QString& id)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	QList<Account> list;
	foreach (Account account, allItems())
		if (account.protocolName() == protocolName && account.id() == id)
			return account;

	return Account::null;
}

const QList<Account> AccountManager::byProtocolName(const QString &name)
{
	QMutexLocker(&mutex());

	ensureLoaded();

	QList<Account> list;
	foreach (Account account, allItems())
		if (account.protocolName() == name)
			list.append(account);

	return list;
}

Status AccountManager::status()
{
	QMutexLocker(&mutex());

	Account account = defaultAccount();
	return !account.isNull()
			? account.statusContainer()->status()
			: Status();
}

void AccountManager::accountDataUpdated()
{
	QMutexLocker(&mutex());

	Account account(sender());
	if (account)
		emit accountUpdated(account);
}

void AccountManager::connectionError(Account account, const QString &server, const QString &message)
{
	QMutexLocker(&mutex());


	if (!ConnectionErrorNotification::activeError(account, message))
	{
		ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(account,
				server, message);
		NotificationManager::instance()->notify(connectionErrorNotification);
	}

}
