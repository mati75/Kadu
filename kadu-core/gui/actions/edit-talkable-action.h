/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EDIT_TALKABLE_ACTION_H
#define EDIT_TALKABLE_ACTION_H

#include <QtGui/QAction>

#include "model/roles.h"

#include "gui/actions/action-description.h"

class Buddy;
class Chat;

class EditTalkableAction : public ActionDescription
{
	Q_OBJECT

	int actionRole(ActionContext *context) const;
	Chat actionChat(ActionContext *context) const;
	Buddy actionBuddy(ActionContext *context) const;

	void setChatActionTitleAndIcon(Action *action);
	void setBuddyActionTitleAndIcon(Action *action);

	void updateChatActionState(Action *action);
	void updateBuddyActionState(Action *action);

	void chatActionTriggered(ActionContext *context);
	void buddyActionTriggered(ActionContext *context);

protected:
	virtual void actionInstanceCreated(Action *action);
	virtual void updateActionState(Action *action);
	virtual void triggered(QWidget *widget, ActionContext *context, bool toggled);

public:
	explicit EditTalkableAction(QObject *parent);
	virtual ~EditTalkableAction();

	void trigger(ActionContext *context);

};

#endif // EDIT_TALKABLE_ACTION_H