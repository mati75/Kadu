/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/chat.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action.h"

#include "encryption-manager.h"
#include "encryption-provider-manager.h"

#include "enable-encryption-action-description.h"

EnableEncryptionActionDescription::EnableEncryptionActionDescription(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("encryptionAction");
	setIcon(KaduIcon("security-high"));
	setText(tr("Encrypt"));
	setCheckable(true);

	registerAction();

	connect(EncryptionProviderManager::instance(), SIGNAL(canEncryptChanged(Chat)),
			this, SLOT(canEncryptChanged(Chat)));
}

EnableEncryptionActionDescription::~EnableEncryptionActionDescription()
{
	disconnect(EncryptionProviderManager::instance(), SIGNAL(canEncryptChanged(Chat)),
			this, SLOT(canEncryptChanged(Chat)));
}

void EnableEncryptionActionDescription::actionTriggered(QAction *sender, bool toggled)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	if (!action->context()->chat())
		return;

	if (!EncryptionManager::instance()->setEncryptionEnabled(action->context()->chat(), toggled, true) && toggled)
	{
		// disable it, we could not enable encryption for this contact
		sender->setEnabled(false);
		sender->setChecked(false);
	}
}

void EnableEncryptionActionDescription::updateActionState(Action *action)
{
	Chat chat = action->context()->chat();
	action->setEnabled(chat && EncryptionProviderManager::instance()->canEncrypt(chat));
}

void EnableEncryptionActionDescription::canEncryptChanged(const Chat &chat)
{
	// there is only as much actions as chat windows, so this is not really N^2 when
	// this slot is called for each chat when new encryption implementation is loaded/unloaded
	// so no need to optimize it
	foreach (Action *action, actions())
		if (action->context()->chat() == chat)
			action->checkState();
}
