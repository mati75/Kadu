/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#include <QtGui/QDateEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QItemDelegate>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

#include "buddies/model/buddies-model-base.h"
#include "chat/chat-details-aggregate.h"
#include "chat/chat-manager.h"
#include "chat/filter/chat-name-filter.h"
#include "chat/message/message.h"
#include "chat/type/chat-type.h"
#include "chat/type/chat-type-manager.h"
#include "chat/aggregate-chat-builder.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/delayed-line-edit.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "model/buddy-status-dates-model.h"
#include "model/chat-dates-model.h"
#include "model/history-chats-model.h"
#include "model/history-chats-model-proxy.h"
#include "model/sms-dates-model.h"
#include "storage/history-storage.h"
#include "history-tree-item.h"
#include "timed-status.h"

#include "history-window.h"

HistoryWindow::HistoryWindow(QWidget *parent) :
		MainWindow(parent)
{
	kdebugf();

	setWindowRole("kadu-history");

	setWindowTitle(tr("History"));
	setWindowIcon(IconsManager::instance()->iconByPath("kadu_icons/kadu-history"));

	createGui();
	connectGui();

	loadWindowGeometry(this, "History", "HistoryWindowGeometry", 200, 200, 750, 500);
//TODO 0.6.6:
	DetailsPopupMenu = new QMenu(this);
//	DetailsPopupMenu->addAction(IconsManager::instance()->loadIcon("ClearHistory"), tr("&Remove entries"), this, SLOT(removeHistoryEntriesPerDate()));

	kdebugf2();
}

HistoryWindow::~HistoryWindow()
{
	kdebugf();

	saveWindowGeometry(this, "History", "HistoryDialogGeometry");

	//writeToolBarsToConfig("history");
	kdebugf2();
}

void HistoryWindow::createGui()
{
	QWidget *mainWidget = new QWidget(this);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout *layout = new QVBoxLayout(mainWidget);
	layout->setMargin(5);
	layout->setSpacing(5);

	QSplitter *splitter = new QSplitter(Qt::Horizontal, mainWidget);
	layout->addWidget(splitter);

	createChatTree(splitter);
	QSplitter *rightSplitter = new QSplitter(Qt::Vertical, splitter);

	QWidget *rightWidget = new QWidget(rightSplitter);
	QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setSpacing(0);
	rightLayout->setMargin(0);

	QWidget *filterWidget = new QWidget(rightWidget);
	rightLayout->addWidget(filterWidget);

	createFilterBar(filterWidget);

	DetailsListView = new QTreeView(rightWidget);
	rightLayout->addWidget(DetailsListView);

	MyChatDatesModel = new ChatDatesModel(Chat::null, QList<QDate>(), this);
	MyBuddyStatusDatesModel = new BuddyStatusDatesModel(Buddy::null, QList<QDate>(), this);
	MySmsDatesModel = new SmsDatesModel(QString::null, QList<QDate>(), this);

	DetailsListView->setRootIsDecorated(false);
	DetailsListView->setUniformRowHeights(true);

	ContentBrowser = new ChatMessagesView(Chat::null, false, rightSplitter);
	ContentBrowser->setForcePruneDisabled(true);

	QList<int> sizes;
	sizes.append(150);
	sizes.append(300);
	splitter->setSizes(sizes);

	setCentralWidget(mainWidget);
}

void HistoryWindow::createChatTree(QWidget *parent)
{
	QWidget *chatsWidget = new QWidget(parent);
	chatsWidget->setMinimumWidth(150);
	QVBoxLayout *layout = new QVBoxLayout(chatsWidget);

	QLineEdit *filterLineEdit = new QLineEdit(chatsWidget);
	connect(filterLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(filterLineChanged(const QString &)));
	layout->addWidget(filterLineEdit);

	ChatsTree = new QTreeView(parent);
	ChatsTree->header()->hide();
	layout->addWidget(ChatsTree);

	ChatsModel = new HistoryChatsModel(this);

	ChatsModelProxy = new HistoryChatsModelProxy(this);
	ChatsModelProxy->setSourceModel(ChatsModel);

	NameFilter = new ChatNameFilter(this);
	ChatsModelProxy->addFilter(NameFilter);

	ChatsTree->setModel(ChatsModelProxy);
	ChatsModelProxy->sort(1);
	ChatsModelProxy->sort(0); // do the sorting
	ChatsTree->setRootIsDecorated(true);
}

void HistoryWindow::createFilterBar(QWidget *parent)
{
	QGridLayout *layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);

	QLabel *filterLabel = new QLabel(tr("Filter") + ": ", parent);
	layout->addWidget(filterLabel, 0, 0, 1, 1);

	DelayedLineEdit *searchLineEdit = new DelayedLineEdit(parent);
	layout->addWidget(searchLineEdit, 0, 1, 1, 4);

	QCheckBox *filterByDate = new QCheckBox(tr("by date"), parent);
	filterByDate->setChecked(false);
	layout->addWidget(filterByDate, 1, 0, 1, 1);

	FromDateLabel = new QLabel(tr("From") + ": ", parent);
	FromDateLabel->setEnabled(false);
	layout->addWidget(FromDateLabel, 1, 1, 1, 1, Qt::AlignRight);

	FromDate = new QDateEdit(parent);
	FromDate->setEnabled(false);
	FromDate->setCalendarPopup(true);
	FromDate->setDate(QDateTime::currentDateTime().addDays(-7).date());
	layout->addWidget(FromDate, 1, 2, 1, 1);

	ToDateLabel = new QLabel(tr("To") + ": ", parent);
	ToDateLabel->setEnabled(false);
	layout->addWidget(ToDateLabel, 1, 3, 1, 1, Qt::AlignRight);

	ToDate = new QDateEdit(parent);
	ToDate->setEnabled(false);
	ToDate->setCalendarPopup(true);
	ToDate->setDate(QDateTime::currentDateTime().date());
	layout->addWidget(ToDate, 1, 4, 1, 1);

	connect(filterByDate, SIGNAL(stateChanged(int)),
			this, SLOT(dateFilteringEnabled(int)));

	connect(searchLineEdit, SIGNAL(delayedTextChanged(const QString &)),
			this, SLOT(searchTextChanged(const QString &)));
	connect(FromDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(fromDateChanged(const QDate &)));
	connect(ToDate, SIGNAL(dateChanged(const QDate &)),
			this, SLOT(toDateChanged(const QDate &)));
}

void HistoryWindow::connectGui()
{
	connect(ChatsTree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(treeCurrentChanged(QModelIndex,QModelIndex)));

	ChatsTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ChatsTree, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showMainPopupMenu(QPoint)));

	DetailsListView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(DetailsListView, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(showDetailsPopupMenu(QPoint)));
}

void HistoryWindow::updateData()
{
	kdebugf();

	QModelIndex index = ChatsTree->selectionModel()->currentIndex();
	HistoryTreeItem treeItem = index.data(HistoryItemRole).value<HistoryTreeItem>();

	QList<Chat> usedChats;
	QList<Chat> chatsList = History::instance()->chatsList(Search);
	QList<Chat> result;

	foreach (Chat chat, chatsList)
	{
		if (usedChats.contains(chat))
			continue;
		Chat aggregate = AggregateChatBuilder::buildAggregateChat(chat.contacts().toBuddySet());
		if (aggregate)
		{
			ChatDetailsAggregate *details = dynamic_cast<ChatDetailsAggregate *>(aggregate.details());

			if (details)
				foreach (Chat usedChat, details->chats())
					usedChats.append(usedChat);

			result.append(aggregate);
		}
		else
		{
			result.append(chat);
			usedChats.append(chat);
		}
	}

	ChatsModel->setChats(result);

	selectHistoryItem(treeItem);

	ChatsModel->setStatusBuddies(History::instance()->statusBuddiesList(Search));
	ChatsModel->setSmsRecipients(History::instance()->smsRecipientsList(Search));
}

void HistoryWindow::selectChat(Chat chat)
{
	QString typeName = chat.type();
	ChatType *type = ChatTypeManager::instance()->chatType(typeName);

	if (!type)
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	QModelIndex chatTypeIndex = ChatsModelProxy->chatTypeIndex(type);
	if (!chatTypeIndex.isValid())
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(chatTypeIndex);

	QModelIndex chatIndex = ChatsModelProxy->chatIndex(chat);
	ChatsTree->selectionModel()->select(chatIndex, QItemSelectionModel::ClearAndSelect);

	chatActivated(chat);
}

void HistoryWindow::selectStatusBuddy(Buddy buddy)
{
	QModelIndex statusIndex = ChatsModelProxy->statusIndex();
	if (!statusIndex.isValid())
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(statusIndex);

	QModelIndex statusBuddyIndex = ChatsModelProxy->statusBuddyIndex(buddy);
	ChatsTree->selectionModel()->select(statusBuddyIndex, QItemSelectionModel::ClearAndSelect);

	statusBuddyActivated(buddy);
}

void HistoryWindow::selectSmsRecipient(const QString& recipient)
{
	QModelIndex smsIndex = ChatsModelProxy->smsIndex();
	if (!smsIndex.isValid())
	{
		treeItemActivated(HistoryTreeItem());
		return;
	}

	ChatsTree->collapseAll();
	ChatsTree->expand(smsIndex);

	QModelIndex smsRecipientIndex = ChatsModelProxy->smsRecipientIndex(recipient);
	ChatsTree->selectionModel()->select(smsRecipientIndex, QItemSelectionModel::ClearAndSelect);

	smsRecipientActivated(recipient);
}

void HistoryWindow::selectHistoryItem(HistoryTreeItem item)
{
	switch (item.type())
	{
		case HistoryTypeNone:
			// do nothing
			break;

		case HistoryTypeChat:
			selectChat(item.chat());
			break;

		case HistoryTypeStatus:
			selectStatusBuddy(item.buddy());
			break;

		case HistoryTypeSms:
			selectSmsRecipient(item.smsRecipient());
			break;
	}
}

void HistoryWindow::chatActivated(Chat chat)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->selectionModel()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();
	QDate date = selectedIndex.data(DateRole).toDate();

	QList<QDate> chatDates = History::instance()->datesForChat(chat, Search);
	MyChatDatesModel->setChat(chat);
	MyChatDatesModel->setDates(chatDates);

	QModelIndex select = MyChatDatesModel->indexForDate(date);
	if (!select.isValid())
	{
		int lastRow = MyChatDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			select = MyChatDatesModel->index(lastRow);
	}

	DetailsListView->setModel(MyChatDatesModel);

	connect(DetailsListView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(dateCurrentChanged(QModelIndex,QModelIndex)));

	DetailsListView->selectionModel()->setCurrentIndex(select, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::statusBuddyActivated(Buddy buddy)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->model()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();

	QDate date = selectedIndex.data(DateRole).toDate();

	QList<QDate> statusDates = History::instance()->datesForStatusBuddy(buddy, Search);
	MyBuddyStatusDatesModel->setBuddy(buddy);
	MyBuddyStatusDatesModel->setDates(statusDates);

	if (date.isValid())
		selectedIndex = MyBuddyStatusDatesModel->indexForDate(date);
	if (!selectedIndex.isValid())
	{
		int lastRow = MyBuddyStatusDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			selectedIndex = MyBuddyStatusDatesModel->index(lastRow);
	}

	DetailsListView->setModel(MyBuddyStatusDatesModel);
	DetailsListView->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::smsRecipientActivated(const QString& recipient)
{
	kdebugf();

	QModelIndex selectedIndex = DetailsListView->model()
			? DetailsListView->selectionModel()->currentIndex()
			: QModelIndex();

	QDate date = selectedIndex.data(DateRole).toDate();

	QList<QDate> smsDates = History::instance()->datesForSmsRecipient(recipient, Search);
	MySmsDatesModel->setRecipient(recipient);
	MySmsDatesModel->setDates(smsDates);

	if (date.isValid())
		selectedIndex = MySmsDatesModel->indexForDate(date);
	if (!selectedIndex.isValid())
	{
		int lastRow = MySmsDatesModel->rowCount(QModelIndex()) - 1;
		if (lastRow >= 0)
			selectedIndex = MySmsDatesModel->index(lastRow);
	}

	DetailsListView->setModel(MySmsDatesModel);
	DetailsListView->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	kdebugf2();
}

void HistoryWindow::treeItemActivated(HistoryTreeItem item)
{
	switch (item.type())
	{
		case HistoryTypeNone:
			// do nothing
			break;

		case HistoryTypeChat:
			chatActivated(item.chat());
			break;

		case HistoryTypeStatus:
			statusBuddyActivated(item.buddy());
			break;

		case HistoryTypeSms:
			smsRecipientActivated(item.smsRecipient());
			break;
	}
}

void HistoryWindow::treeCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current == previous)
		return;

	kdebugf();

	treeItemActivated(current.data(HistoryItemRole).value<HistoryTreeItem>());

	kdebugf2();
}

void HistoryWindow::dateCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	if (current == previous)
		return;

	Q_UNUSED(previous)

	kdebugf();

	HistoryTreeItem treeItem = current.data(HistoryItemRole).value<HistoryTreeItem>();
	QDate date = current.data(DateRole).value<QDate>();

	ContentBrowser->clearMessages();

	switch (treeItem.type())
	{
		case HistoryTypeNone:
			// do nothing
			break;

		case HistoryTypeChat:
		{
			Chat chat = treeItem.chat();
			QList<Message> messages;
			if (chat && date.isValid())
				messages = History::instance()->messages(chat, date);
			ContentBrowser->setChat(chat);
			ContentBrowser->appendMessages(messages);
			QTimer::singleShot(500, this, SLOT(selectQueryText()));
			break;
		}

		case HistoryTypeStatus:
		{
			Buddy buddy = treeItem.buddy();
			QList<TimedStatus> statuses;
			if (buddy && date.isValid())
				statuses = History::instance()->statuses(buddy, date);
			if (buddy.contacts().size() > 0)
				ContentBrowser->setChat(ChatManager::instance()->findChat(ContactSet(buddy.contacts()[0]), true));
			ContentBrowser->appendMessages(statusesToMessages(statuses));
			QTimer::singleShot(500, this, SLOT(selectQueryText()));
			break;
		}

		case HistoryTypeSms:
		{
			QString recipient = treeItem.smsRecipient();
			QList<QString> sms;
			if (!recipient.isEmpty() && date.isValid())
				sms = History::instance()->sms(recipient, date);
			ContentBrowser->appendMessages(smsToMessage(sms));
			QTimer::singleShot(500, this, SLOT(selectQueryText()));
			break;
		}
	}

	kdebugf2();
}

QList<Message> HistoryWindow::statusesToMessages(QList<TimedStatus> statuses)
{
	QList<Message> messages;

	foreach (TimedStatus timedStatus, statuses)
	{
		Message message = Message::create();
		message.setStatus(Message::StatusReceived);
		message.setType(Message::TypeReceived);

		if (timedStatus.status().description().isEmpty())
			message.setContent(timedStatus.status().type());
		else
			message.setContent(QString("%1 with description: %2")
					.arg(timedStatus.status().type())
					.arg(timedStatus.status().description()));

		message.setReceiveDate(timedStatus.dateTime());
		message.setSendDate(timedStatus.dateTime());

		messages.append(message);
	}

	return messages;
}

QList<Message> HistoryWindow::smsToMessage(QList<QString> sms)
{
	QList<Message> messages;

	foreach (QString oneSms, sms)
	{
		Message message = Message::create();
		message.setStatus(Message::StatusSent);
		message.setType(Message::TypeReceived);
		message.setContent(oneSms);

		messages.append(message);
	}

	return messages;
}

void HistoryWindow::filterLineChanged(const QString &filterText)
{
	NameFilter->setName(filterText);
}

void HistoryWindow::searchTextChanged(const QString &searchText)
{
	Search.setQuery(searchText);
	updateData();
}

void HistoryWindow::fromDateChanged(const QDate &date)
{
	Search.setFromDate(date);
	if (ToDate->date() < date)
		ToDate->setDate(date);
	else
		updateData();
}

void HistoryWindow::toDateChanged(const QDate &date)
{
	Search.setToDate(date);
	if (FromDate->date() > date)
		FromDate->setDate(date);
	else
		updateData();
}

void HistoryWindow::showMainPopupMenu(const QPoint &pos)
{
	Chat chat = ChatsTree->indexAt(pos).data(ChatRole).value<Chat>();
	if (!chat)
		return;

	QMenu *menu = BuddiesListViewMenuManager::instance()->menu(this, this, chat.contacts().toContactList());
	menu->addAction(IconsManager::instance()->iconByPath("kadu_icons/history-clear"), tr("&Clear history"), this, SLOT(clearHistory()));
	menu->exec(QCursor::pos());

	delete menu;
}

void HistoryWindow::showDetailsPopupMenu(const QPoint &pos)
{
	bool isValid = true;
	Chat chat = DetailsListView->indexAt(pos).data(ChatRole).value<Chat>();
	QDate date = DetailsListView->indexAt(pos).data(DateRole).value<QDate>();

	if (!chat || !date.isValid())
		isValid = false;

	foreach (QAction *action, DetailsPopupMenu->actions())
		action->setEnabled(isValid);

	DetailsPopupMenu->exec(QCursor::pos());
}

void HistoryWindow::show(Chat chat)
{
	if (!History::instance()->currentStorage())
	{
		MessageDialog::msg(tr("There is no history storage module loaded!"), false, "dialog-warning");
		return;
	}

	Chat aggregate = AggregateChatBuilder::buildAggregateChat(chat.contacts().toBuddySet());
	if (aggregate)
		chat = aggregate;

	updateData();
	selectChat(chat);

	QWidget::show();
	_activateWindow(this);
}

void HistoryWindow::openChat()
{
	kdebugf();
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return;

	ChatWidgetManager::instance()->openChatWidget(chat, true);

	kdebugf2();
}

void HistoryWindow::clearHistory()
{
	kdebugf();
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return;

	History::instance()->currentStorage()->clearChatHistory(chat);
	updateData();
	kdebugf2();
}

void HistoryWindow::selectQueryText()
{
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
	ContentBrowser->findText(""); // clear old selection
	ContentBrowser->findText(Search.query(), QWebPage::HighlightAllOccurrences);
#else
	ContentBrowser->findText(Search.query());
#endif
}

bool HistoryWindow::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeHistory);
}

void HistoryWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		hide();
	}
	else if (e == QKeySequence::Copy)
	{
		ContentBrowser->pageAction(QWebPage::Copy)->trigger();
	}
	else
		QWidget::keyPressEvent(e);
}

ContactSet HistoryWindow::contacts()
{
	Chat chat = ChatsTree->currentIndex().data(ChatRole).value<Chat>();
	if (!chat)
		return ContactSet();
	return chat.contacts();
}

BuddySet HistoryWindow::buddies()
{
	return contacts().toBuddySet();
}

void HistoryWindow::dateFilteringEnabled(int state)
{
	bool enabled = state == 2;
	FromDateLabel->setEnabled(enabled);
	FromDate->setEnabled(enabled);
	ToDateLabel->setEnabled(enabled);
	ToDate->setEnabled(enabled);

	if (enabled)
	{
		Search.setFromDate(FromDate->date());
		Search.setToDate(ToDate->date());
		updateData();
	}
	else
	{
		Search.setFromDate(QDate());
		Search.setToDate(QDate());
		updateData();
	}
}

HistoryWindow *historyDialog = 0;
