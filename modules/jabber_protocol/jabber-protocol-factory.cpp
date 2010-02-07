/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "misc/misc.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"

#include "gui/widgets/jabber-contact-widget.h"
#include "gui/widgets/jabber-add-account-widget.h"
#include "gui/widgets/jabber-create-account-widget.h"
#include "gui/widgets/jabber-edit-account-widget.h"
#include "jabber-account-details.h"
#include "jabber-contact-details.h"
#include "jabber-protocol.h"
#include "jabber-protocol-factory.h"

JabberProtocolFactory * JabberProtocolFactory::Instance = 0;

JabberProtocolFactory * JabberProtocolFactory::instance()
{
	if (!Instance)
		Instance = new JabberProtocolFactory();

	return Instance;
}

JabberProtocolFactory::JabberProtocolFactory()
{
	StatusTypeManager *statusTypeManager = StatusTypeManager::instance();
	SupportedStatusTypes.append(statusTypeManager->statusType("Online"));
	SupportedStatusTypes.append(statusTypeManager->statusType("FreeForChat"));
	SupportedStatusTypes.append(statusTypeManager->statusType("DoNotDisturb"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Away"));
	SupportedStatusTypes.append(statusTypeManager->statusType("NotAvailable"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Invisible"));
	SupportedStatusTypes.append(statusTypeManager->statusType("Offline"));

	qSort(SupportedStatusTypes.begin(), SupportedStatusTypes.end(), StatusType::lessThan);

	IdRegularExpression.setPattern("[a-zA-Z0-9\\._-]+@[a-zA-Z0-9\\._-]+");
}

QIcon JabberProtocolFactory::icon()
{
	return IconsManager::instance()->iconByPath("tmp-protocols/xmpp/48x48/online.png");
}

QString JabberProtocolFactory::iconPath()
{
	return IconsManager::instance()->iconPath("tmp-protocols/xmpp/48x48/online.png");
}

Protocol * JabberProtocolFactory::createProtocolHandler(Account account)
{
	return new JabberProtocol(account, this);
}

AccountDetails * JabberProtocolFactory::createAccountDetails(AccountShared *accountShared)
{
	return new JabberAccountDetails(accountShared);
}

ContactDetails * JabberProtocolFactory::createContactDetails(ContactShared *contactShared)
{
	return new JabberContactDetails(contactShared);
}

AccountAddWidget * JabberProtocolFactory::newAddAccountWidget(QWidget *parent)
{
	return new JabberAddAccountWidget(parent);
}

QWidget * JabberProtocolFactory::newCreateAccountWidget(QWidget *parent)
{
	return new JabberCreateAccountWidget(parent);
}

AccountEditWidget * JabberProtocolFactory::newEditAccountWidget(Account account, QWidget *parent)
{
	return new JabberEditAccountWidget(account, parent);
}

QList<StatusType *> JabberProtocolFactory::supportedStatusTypes()
{
	return SupportedStatusTypes;
}

QString JabberProtocolFactory::idLabel()
{
	return tr("Jabber ID:");
}

QRegExp JabberProtocolFactory::idRegularExpression()
{
	return IdRegularExpression;
}

ContactWidget * JabberProtocolFactory::newContactWidget(Contact contact, QWidget *parent)
{
	JabberContactDetails *jabberContactDetails = dynamic_cast<JabberContactDetails *>(contact.details());

	return 0 != jabberContactDetails
			? new JabberContactWidget(contact, parent)
			: 0;
}
