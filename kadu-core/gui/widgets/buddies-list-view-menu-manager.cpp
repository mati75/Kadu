/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMenu>

#include "accounts/account.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"

#include "buddies-list-view-menu-manager.h"

BuddiesListViewMenuManager * BuddiesListViewMenuManager::Instance = 0;

BuddiesListViewMenuManager * BuddiesListViewMenuManager::instance()
{
	if (!Instance)
		Instance = new BuddiesListViewMenuManager();

	return Instance;
}

BuddiesListViewMenuManager::BuddiesListViewMenuManager() :
		BuddiesContexMenuSorted(true), BuddyListActionsSorted(true)
{
}

void BuddiesListViewMenuManager::sortBuddiesContexMenu()
{
	if (!BuddiesContexMenuSorted)
	{
		qSort(BuddiesContexMenu);
		BuddiesContexMenuSorted = true;
	}
}

void BuddiesListViewMenuManager::sortBuddyListActions()
{
	if (!BuddyListActionsSorted)
	{
		qSort(BuddyListActions);
		BuddyListActionsSorted = true;
	}
}

void BuddiesListViewMenuManager::addActionDescription(ActionDescription *actionDescription, BuddiesListViewMenuItem::BuddiesListViewMenuCategory category, int priority)
{
	BuddiesContexMenu.append(BuddiesListViewMenuItem(actionDescription, category, priority));
	BuddiesContexMenuSorted = false;
}

void BuddiesListViewMenuManager::removeActionDescription(ActionDescription *actionDescription)
{
	QList<BuddiesListViewMenuItem>::iterator i = BuddiesContexMenu.begin();

	while (i != BuddiesContexMenu.end())
	{
		if ((*i).actionDescription() == actionDescription)
			i = BuddiesContexMenu.erase(i);
		else
			++i;
	}
}

void BuddiesListViewMenuManager::addListActionDescription(ActionDescription *actionDescription, BuddiesListViewMenuItem::BuddiesListViewMenuCategory category, int priority)
{
	BuddyListActions.append(BuddiesListViewMenuItem(actionDescription, category, priority));
	BuddyListActionsSorted = false;
}

void BuddiesListViewMenuManager::removeListActionDescription(ActionDescription *actionDescription)
{
	QList<BuddiesListViewMenuItem>::iterator i = BuddyListActions.begin();

	while (i != BuddyListActions.end())
	{
		if ((*i).actionDescription() == actionDescription)
			i = BuddyListActions.erase(i);
		else
			++i;
	}
}

QMenu * BuddiesListViewMenuManager::menu(QWidget *parent, ActionDataSource *actionDataSource, QList<Contact> contacts)
{
	sortBuddiesContexMenu();
	sortBuddyListActions();

	QMenu *menu = new QMenu(parent);

	QMenu *actions = new QMenu(tr("More Actions..."));

	BuddiesListViewMenuItem::BuddiesListViewMenuCategory lastCategory = BuddiesListViewMenuItem::MenuCategoryChat;
	bool first = true;
	foreach (BuddiesListViewMenuItem menuItem, BuddyListActions)
	{
		if (!first && lastCategory != menuItem.category())
			actions->addSeparator();

		Action *action = menuItem.actionDescription()->createAction(actionDataSource, parent);
		actions->addAction(action);
		action->checkState();

		lastCategory = menuItem.category();
		first = false;
	}

	lastCategory = BuddiesListViewMenuItem::MenuCategoryChat;
	first = true;
	foreach (BuddiesListViewMenuItem menuItem, BuddiesContexMenu)
	{
		if (!first && lastCategory != menuItem.category())
		{
			if (menuItem.category() > BuddiesListViewMenuItem::MenuCategoryActions)
				menu->addMenu(actions);
			menu->addSeparator();
		}

		Action *action = menuItem.actionDescription()->createAction(actionDataSource, parent);
		menu->addAction(action);
		action->checkState();

		lastCategory = menuItem.category();
		first = false;
	}

	foreach (Contact contact, contacts)
	{
		if (!contact.contactAccount() || !contact.contactAccount().protocolHandler())
			continue;

		Account account = contact.contactAccount();
		ProtocolFactory *protocolFactory = account.protocolHandler()->protocolFactory();

		if (!account.protocolHandler()->protocolFactory() || !protocolFactory->protocolMenuManager())
			continue;

		QMenu *account_menu = menu->addMenu(account.accountIdentity().name());
		if (!protocolFactory->icon().isNull())
			account_menu->setIcon(protocolFactory->icon());

		if (protocolFactory->protocolMenuManager()->protocolActions(account, contact.ownerBuddy()).size() == 0)
			continue;

		foreach (ActionDescription *actionDescription, protocolFactory->protocolMenuManager()->protocolActions(account, contact.ownerBuddy()))
			if (actionDescription)
			{
				Action *action = actionDescription->createAction(actionDataSource, parent);
				account_menu->addAction(action);
				action->checkState();
			}
			else
				account_menu->addSeparator();
	}

	return menu;
}
