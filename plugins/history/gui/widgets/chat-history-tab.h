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

#ifndef CHAT_HISTORY_TAB_H
#define CHAT_HISTORY_TAB_H

#include <QtCore/QModelIndex>

#include "talkable/talkable.h"
#include "exports.h"

#include "gui/widgets/history-tab.h"

class BuddyListModel;
class ChatsListModel;
class HistoryDatesModel;
class ModelChain;
class TalkableTreeView;
class TimelineChatMessagesView;

class KADUAPI ChatHistoryTab : public HistoryTab
{
	Q_OBJECT

	TalkableTreeView *ChatsTalkableTree;
	ModelChain *ChatsModelChain;
	ChatsListModel *ChatsModel;
	BuddyListModel *ChatsBuddiesModel;

	HistoryDatesModel *MyChatDatesModel;

	void chatActivated(const Chat &chat);

private slots:
	void clearChatHistory();
	void currentChatChanged(const Talkable &talkable);
	void chatDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	void showChatsPopupMenu(const QPoint &pos);

protected:
	virtual void createTreeView(QWidget *parent);
	virtual void removeEntriesPerDate(const QDate &date);

public:
	explicit ChatHistoryTab(QWidget *parent = 0);
	virtual ~ChatHistoryTab();

	virtual void updateData();

	void selectChat(const Chat &chat);

};

#endif // CHAT_HISTORY_TAB_H