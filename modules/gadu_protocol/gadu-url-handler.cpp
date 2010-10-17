/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QCursor>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-widget-manager.h"
#include "misc/misc.h"

#include "gadu-url-handler.h"

GaduUrlHandler::GaduUrlHandler()
{
	GaduRegExp = QRegExp("gg:(/){0,3}[0-9]{1,8}");
}

bool GaduUrlHandler::isUrlValid(const QString &url)
{
	return GaduRegExp.exactMatch(url);
}

void GaduUrlHandler::convertUrlsToHtml(HtmlDocument &document)
{
	for (int i = 0; i < document.countElements(); ++i)
	{
		if (document.isTagElement(i))
			continue;

		QString text = document.elementText(i);
		int index = GaduRegExp.indexIn(text);
		if (index < 0)
			continue;

		unsigned int length = GaduRegExp.matchedLength();
		QString gg = text.mid(index, length);

		document.splitElement(i, index, length);
		document.setElementValue(i, "<a href=\"" + gg + "\">" + gg + "</a>", true);
	}
}

void GaduUrlHandler::openUrl(const QString &url, bool disableMenu)
{
	QList<Account> gaduAccounts = AccountManager::instance()->byProtocolName("gadu");
	if (!gaduAccounts.count())
		return;

	QString gaduId = url;
	if (gaduId.startsWith("gg:"))
	{
		gaduId.remove(0, 3);
		gaduId.remove(QRegExp("/*"));
	}

	if (gaduAccounts.count() == 1 || disableMenu)
	{
		Contact contact = ContactManager::instance()->byId(gaduAccounts[0], gaduId, ActionCreateAndAdd);
		Chat chat = ChatManager::instance()->findChat(ContactSet(contact));
		if (chat)
		{
			ChatWidgetManager::instance()->openPendingMsgs(chat, true);
			return;
		}
	}
	else
	{
		QMenu *menu = new QMenu;

		QStringList ids;
		foreach (Account account, gaduAccounts)
		{
			ids.clear();
			ids.append(account.id());
			ids.append(gaduId);

			menu->addAction(account.statusContainer()->statusIcon(), account.id())->setData(ids);
		}

		connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(accountSelected(QAction *)));

		menu->exec(QCursor::pos());
	}
}

void GaduUrlHandler::accountSelected(QAction *action)
{
	QStringList ids = action->data().toStringList();

	if (ids.count() != 2)
		return;

	Account account = AccountManager::instance()->byId("gadu", ids[0]);
	if (!account)
		return;

	Contact contact = ContactManager::instance()->byId(account, ids[1], ActionCreateAndAdd);
	Chat chat = ChatManager::instance()->findChat(ContactSet(contact));
	if (chat)
		ChatWidgetManager::instance()->openPendingMsgs(chat, true);
}
