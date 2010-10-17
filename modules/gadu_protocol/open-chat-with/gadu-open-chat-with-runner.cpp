/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QIntValidator>

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"

#include "contacts/contact.h"
#include "contacts/contact-manager.h"

#include "debug.h"

#include "gadu-contact-details.h"
#include "gadu-open-chat-with-runner.h"

GaduOpenChatWithRunner::GaduOpenChatWithRunner(Account account) :
		ParentAccount(account)
{
}

BuddyList GaduOpenChatWithRunner::matchingContacts(const QString &query)
{
	kdebugf();

	BuddyList matchedContacts;
	if (!validateUserID(query))
		return matchedContacts;

	Contact contact = ContactManager::instance()->byId(ParentAccount, query, ActionCreate);
	Buddy buddy = contact.ownerBuddy();
	if (buddy.isNull())
	{
		Buddy buddy = Buddy::create();
		buddy.setDisplay(QString("%1: %2").arg(ParentAccount.accountIdentity().name()).arg(query));
		contact.setOwnerBuddy(buddy);
	}
	matchedContacts.append(buddy);

	return matchedContacts;
}

bool GaduOpenChatWithRunner::validateUserID(const QString &uid)
{
	QIntValidator v(1, 999999999, 0);
	int pos = 0;
	QString text = uid;
	return v.validate(text, pos) == QValidator::Acceptable;
}
