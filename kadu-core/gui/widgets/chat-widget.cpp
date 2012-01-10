/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2002, 2003, 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2005, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2002, 2003, 2004, 2005, 2006, 2007, 2008 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
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

#include <QtCore/QFileInfo>
#include <QtGui/QIcon>
#include <QtGui/QInputDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/chat-geometry-data.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/chat-configuration-holder.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "contacts/model/contact-data-extractor.h"
#include "contacts/model/contact-list-model.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/hot-key.h"
#include "gui/widgets/chat-edit-box-size-manager.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-container.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "message/message-manager.h"
#include "message/message-render-info.h"
#include "model/model-chain.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"

#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "activate.h"
#include "chat-edit-box.h"
#include "custom-input.h"
#include "debug.h"

#include "chat-widget.h"

ChatWidget::ChatWidget(const Chat &chat, QWidget *parent) :
		QWidget(parent), CurrentChat(chat), Container(0),
		BuddiesWidget(0), ProxyModel(0), InputBox(0), HorizontalSplitter(0),
		IsComposing(false), CurrentContactActivity(ChatStateService::StateNone),
		SplittersInitialized(false)
{
	kdebugf();

	setAcceptDrops(true);

	createGui();
	configurationUpdated();

	ComposingTimer.setInterval(2 * 1000);
	connect(&ComposingTimer, SIGNAL(timeout()), this, SLOT(checkComposing()));

	connect(edit(), SIGNAL(textChanged()), this, SLOT(updateComposing()));

	foreach (const Contact &contact, CurrentChat.contacts())
	{
		connect(contact, SIGNAL(updated()), this, SLOT(refreshTitle()));

		if (contact.ownerBuddy())
			connect(contact.ownerBuddy(), SIGNAL(updated()), this, SLOT(refreshTitle()));
	}

	// icon for conference never changes
	if (CurrentChat.contacts().count() == 1)
	{
		foreach (const Contact &contact, CurrentChat.contacts())
		{
			// Actually we only need to send iconChanged() on CurrentStatus update,
			// but we don't have a signal for that in ContactShared.
			// TODO 0.12.0: Consider adding currentStatusChanged() signal to ContactShared.
			// We need QueuedConnection for this scenario: 1. The user opens chat with unread
			// messages in tabs. 2. Contact is updated to not have unread messages and we emit
			// iconChanged(). 3. Tab catches it before Chat is updated and incorrectly sets
			// tab icon to the envelope. This could be fixed correctly if we would fix the above TODO.
			connect(contact, SIGNAL(updated()), this, SIGNAL(iconChanged()), Qt::QueuedConnection);

			if (contact.ownerBuddy())
				connect(contact.ownerBuddy(), SIGNAL(buddySubscriptionChanged()), this, SIGNAL(iconChanged()));
		}

		if (currentProtocol() && currentProtocol()->chatStateService())
			connect(currentProtocol()->chatStateService(), SIGNAL(peerStateChanged(const Contact &, ChatStateService::State)),
					this, SLOT(contactActivityChanged(const Contact &, ChatStateService::State)));
	}

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SIGNAL(iconChanged()));

	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();
	ComposingTimer.stop();

	emit widgetDestroyed();

	if (currentProtocol() && currentProtocol()->chatStateService())
		currentProtocol()->chatStateService()->sendState(chat(), ChatStateService::StateGone);

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed\n");
}

void ChatWidget::setContainer(ChatWidgetContainer *container)
{
	Container = container;
}

ChatWidgetContainer * ChatWidget::container() const
{
	return Container;
}

void ChatWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	VerticalSplitter = new QSplitter(Qt::Vertical, this);

#ifdef Q_OS_MAC
	/* Dorr: workaround for mac tabs issue */
	VerticalSplitter->setAutoFillBackground(true);
#endif

	mainLayout->addWidget(VerticalSplitter);

	HorizontalSplitter = new QSplitter(Qt::Horizontal, this);
	HorizontalSplitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	HorizontalSplitter->setMinimumHeight(10);

	MessagesView = new ChatMessagesView(CurrentChat);

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageUp()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageDown()));
	HorizontalSplitter->addWidget(MessagesView);

	InputBox = new ChatEditBox(CurrentChat, this);
	InputBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
	InputBox->setMinimumHeight(10);

	if (CurrentChat.contacts().count() > 1)
		createContactsList();

	VerticalSplitter->addWidget(HorizontalSplitter);
	VerticalSplitter->setStretchFactor(0, 1);
	VerticalSplitter->addWidget(InputBox);
	VerticalSplitter->setStretchFactor(1, 0);

	connect(VerticalSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(verticalSplitterMoved(int, int)));
	connect(InputBox->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(InputBox->inputBox(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
			this, SLOT(keyPressedSlot(QKeyEvent *, CustomInput *, bool &)));
}

void ChatWidget::createContactsList()
{
	QWidget *contactsListContainer = new QWidget(HorizontalSplitter);

	QVBoxLayout *layout = new QVBoxLayout(contactsListContainer);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	BuddiesWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, this);
	BuddiesWidget->setMinimumSize(QSize(30, 30));

	TalkableTreeView *view = new TalkableTreeView(BuddiesWidget);
	view->setItemsExpandable(false);

	ModelChain *chain = new ModelChain(new ContactListModel(CurrentChat.contacts().toContactVector(), this), this);
	ProxyModel = new TalkableProxyModel(chain);

	NameTalkableFilter *nameFilter = new NameTalkableFilter(NameTalkableFilter::UndecidedMatching, ProxyModel);
	connect(BuddiesWidget, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));

	ProxyModel->addFilter(nameFilter);
	chain->addProxyModel(ProxyModel);

	view->setChain(chain);
	view->setRootIsDecorated(false);
	view->setShowIdentityNameIfMany(false);
	view->setContextMenuEnabled(true);

	connect(view, SIGNAL(talkableActivated(Talkable)),
			Core::instance()->kaduWindow(), SLOT(talkableActivatedSlot(Talkable)));

	BuddiesWidget->setTreeView(view);

	QToolBar *toolBar = new QToolBar(contactsListContainer);
	toolBar->addAction(Actions::instance()->createAction("editUserAction", InputBox->actionContext(), toolBar));
	toolBar->addAction(Actions::instance()->createAction("leaveChatAction", InputBox->actionContext(), toolBar));

	layout->addWidget(toolBar);
	layout->addWidget(BuddiesWidget);

	QList<int> sizes;
	sizes.append(3);
	sizes.append(1);
	HorizontalSplitter->setSizes(sizes);
}

void ChatWidget::configurationUpdated()
{
	InputBox->inputBox()->setFont(ChatConfigurationHolder::instance()->chatFont());
	QString style;
	QColor color = qApp->palette().text().color();
	if (ChatConfigurationHolder::instance()->chatTextCustomColors())
	{
		style = QString("background-color:%1;").arg(ChatConfigurationHolder::instance()->chatTextBgColor().name());
		color = ChatConfigurationHolder::instance()->chatTextFontColor();
	}
	InputBox->inputBox()->viewport()->setStyleSheet(style);
	QPalette palette = InputBox->inputBox()->palette();
	palette.setBrush(QPalette::Text, color);
	InputBox->inputBox()->setPalette(palette);

	refreshTitle();
}

bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Copy) && !MessagesView->selectedText().isEmpty())
	{
		// Do not use triggerPageAction(), see bug #2345.
		MessagesView->pageAction(QWebPage::Copy)->trigger();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
	{
		clearChatWindow();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
	{
		emit closed();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
	{
		Actions::instance()->createAction("lookupUserInfoAction", InputBox->actionContext(), InputBox)->activate(QAction::Trigger);
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_openchatwith"))
	{
		Actions::instance()->createAction("openChatWithAction", InputBox->actionContext(), InputBox)->activate(QAction::Trigger);
		return true;
	}

	return false;
}

void ChatWidget::keyPressEvent(QKeyEvent *e)
{
	kdebugf();
	if (keyPressEventHandled(e))
		e->accept();
	else
		QWidget::keyPressEvent(e);
	kdebugf2();
}

void ChatWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	if (ChatEditBoxSizeManager::instance()->initialized())
		commonHeightChanged(ChatEditBoxSizeManager::instance()->commonHeight());
}

void ChatWidget::refreshTitle()
{
	kdebugf();
	QString title;

	if (!chat().display().isEmpty())
	{
		title = chat().display();
		setTitle(title);
		return;
	}

	int contactsCount = chat().contacts().count();
	kdebugmf(KDEBUG_FUNCTION_START, "chat().contacts().size() = %d\n", contactsCount);
	if (contactsCount > 1)
	{
		title = ChatConfigurationHolder::instance()->conferencePrefix();
		if (title.isEmpty())
			title = tr("Conference with ");

		QString conferenceContents = ChatConfigurationHolder::instance()->conferenceContents();
		QStringList contactslist;
		foreach (Contact contact, chat().contacts())
			contactslist.append(Parser::parse(conferenceContents.isEmpty() ? "%a" : conferenceContents, Talkable(contact), false));

		title.append(contactslist.join(", "));
	}
	else if (contactsCount == 1)
	{
		Contact contact = chat().contacts().toContact();

		if (ChatConfigurationHolder::instance()->chatContents().isEmpty())
		{
			if (contact.isAnonymous())
				title = Parser::parse(tr("Chat with ") + "%a", Talkable(contact), false);
			else
				title = Parser::parse(tr("Chat with ") + "%a (%s[: %d])", Talkable(contact), false);
		}
		else
			title = Parser::parse(ChatConfigurationHolder::instance()->chatContents(), Talkable(contact), false);

		if (ChatConfigurationHolder::instance()->contactStateWindowTitle())
		{
			QString message;
			if (CurrentContactActivity == ChatStateService::StateComposing)
			{
				if (ChatConfigurationHolder::instance()->contactStateWindowTitleComposingSyntax().isEmpty())
					message = tr("(Composing...)");
				else
					message = ChatConfigurationHolder::instance()->contactStateWindowTitleComposingSyntax();
			}
			else if (CurrentContactActivity == ChatStateService::StateInactive)
				message = tr("(Inactive)");
			title = ChatConfigurationHolder::instance()->contactStateWindowTitlePosition() == 0 ? message + " " + title : title + " "  + message;
		}
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setTitle(title);

	kdebugf2();
}

void ChatWidget::setTitle(const QString &title)
{
	if (title != Title)
	{
		Title = title;
		emit titleChanged(this, title);
	}
}

QIcon ChatWidget::icon()
{
	int contactsCount = chat().contacts().count();
	if (contactsCount == 1)
	{
		Contact contact = chat().contacts().toContact();
		if (contact)
			return ContactDataExtractor::data(contact, Qt::DecorationRole, false).value<QIcon>();
	}
	else if (contactsCount > 1)
		return ChatTypeManager::instance()->chatType("Conference")->icon().icon();

	return KaduIcon("internet-group-chat").icon();
}

void ChatWidget::appendMessages(const QList<Message> &messages)
{
	if (messages.isEmpty())
		return;

	bool unread = false;

	QList<MessageRenderInfo *> messageRenderInfos;
	foreach (const Message &message, messages)
	{
		messageRenderInfos.append(new MessageRenderInfo(message));
		unread = unread || message.status() == MessageStatusReceived;
	}

	MessagesView->appendMessages(messageRenderInfos);
	if (unread)
		LastReceivedMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendMessage(const Message &message)
{
	MessagesView->appendMessage(new MessageRenderInfo(message));

	if (message.type() != MessageTypeReceived)
		return;

	LastReceivedMessageTime = QDateTime::currentDateTime();
	if (Container)
		Container->alertChatWidget(this);
}

void ChatWidget::appendSystemMessage(const QString &content)
{
	Message message = Message::create();
	message.setMessageChat(CurrentChat);
	message.setType(MessageTypeSystem);
	message.setContent(content);
	message.setReceiveDate(QDateTime::currentDateTime());
	message.setSendDate(QDateTime::currentDateTime());
	message.setStatus(MessageStatusReceived);

	MessagesView->appendMessage(message);
}

void ChatWidget::resetEditBox()
{
	InputBox->inputBox()->clear();

	Action *action;
	action = ChatWidgetManager::instance()->actions()->bold()->action(InputBox->actionContext());
	if (action)
		InputBox->inputBox()->setFontWeight(action->isChecked() ? QFont::Bold : QFont::Normal);

	action = ChatWidgetManager::instance()->actions()->italic()->action(InputBox->actionContext());
	if (action)
		InputBox->inputBox()->setFontItalic(action->isChecked());

	action = ChatWidgetManager::instance()->actions()->underline()->action(InputBox->actionContext());
	if (action)
		InputBox->inputBox()->setFontUnderline(action->isChecked());
}

void ChatWidget::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageDialog::ask(KaduIcon("dialog-question"), tr("Kadu"), tr("Chat window will be cleared. Continue?")))
	{
		MessagesView->clearMessages();
		activateWindow();
	}
	kdebugf2();
}

/* sends the message typed */
void ChatWidget::sendMessage()
{
	kdebugf();
	if (InputBox->inputBox()->toPlainText().isEmpty())
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	if (!currentProtocol())
		return;

	if (!currentProtocol()->isConnected())
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Cannot send message while being offline.") + tr("Account:") + chat().chatAccount().id(),
				QMessageBox::Ok, this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	FormattedMessage message = FormattedMessage::parse(InputBox->inputBox()->document());
	ChatService *chatService = currentProtocol()->chatService();
	if (!chatService || !chatService->sendMessage(CurrentChat, message))
		return;

	resetEditBox();

	// We sent the message and reseted the edit box, so composing of that message is done.
	// Note that if ComposingTimer is not active, it means that we already reported
	// composing had stopped.
	if (ComposingTimer.isActive())
		composingStopped();

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	kdebugf2();
}

CustomInput * ChatWidget::edit() const
{
	return InputBox ? InputBox->inputBox() : 0;
}

TalkableProxyModel * ChatWidget::talkableProxyModel() const
{
	return ProxyModel;
}

unsigned int ChatWidget::countMessages() const
{
	return MessagesView ? MessagesView->countMessages() : 0;
}

bool ChatWidget::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
	if (!event->mimeData()->hasUrls() || event->source() == MessagesView)
		return false;

	QList<QUrl> urls = event->mimeData()->urls();

	foreach (const QUrl &url, urls)
	{
		QString file = url.toLocalFile();
		if (!file.isEmpty())
		{
			//is needed to check if file refer to local file?
			QFileInfo fileInfo(file);
			if (fileInfo.exists())
				files.append(file);
		}
	}
	return !files.isEmpty();

}

void ChatWidget::dragEnterEvent(QDragEnterEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();

}

void ChatWidget::dragMoveEvent(QDragMoveEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();
}

void ChatWidget::dropEvent(QDropEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
	{
		e->acceptProposedAction();

		QStringList::const_iterator i = files.constBegin();
		QStringList::const_iterator end = files.constEnd();

		for (; i != end; ++i)
			emit fileDropped(CurrentChat, *i);
	}
}

Protocol *ChatWidget::currentProtocol() const
{
	return CurrentChat.chatAccount().protocolHandler();
}

void ChatWidget::activate()
{
	if (Container)
		Container->activateChatWidget(this);
}

bool ChatWidget::isActive()
{
	return Container && Container->isChatWidgetActive(this);
}

void ChatWidget::verticalSplitterMoved(int pos, int index)
{
	Q_UNUSED(pos)
	Q_UNUSED(index)

	if (SplittersInitialized)
		ChatEditBoxSizeManager::instance()->setCommonHeight(VerticalSplitter->sizes().at(1));
}

void ChatWidget::kaduRestoreGeometry()
{
	if (!HorizontalSplitter)
		return;

	if (!chat())
		return;

	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry", ChatWidgetManager::instance(), false);
	if (!cgd)
		return;

	QList<int> horizSizes = cgd->widgetHorizontalSizes();
	if (!horizSizes.isEmpty())
		HorizontalSplitter->setSizes(horizSizes);
}

void ChatWidget::kaduStoreGeometry()
{
	if (!HorizontalSplitter)
		return;

	if (!chat())
		return;

	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry", ChatWidgetManager::instance(), true);
	cgd->setWidgetHorizontalSizes(HorizontalSplitter->sizes());
	cgd->ensureStored();
}

void ChatWidget::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	if (!SplittersInitialized)
		QMetaObject::invokeMethod(this, "setUpVerticalSizes", Qt::QueuedConnection);
}

void ChatWidget::setUpVerticalSizes()
{
	// now we can accept this signal
	connect(ChatEditBoxSizeManager::instance(), SIGNAL(commonHeightChanged(int)), this, SLOT(commonHeightChanged(int)));

	// already set up by other window, so we use this window setting
	if (ChatEditBoxSizeManager::instance()->initialized())
	{
		commonHeightChanged(ChatEditBoxSizeManager::instance()->commonHeight());
		SplittersInitialized = true;
		return;
	}

	QList<int> vertSizes;
	int h = height();
	vertSizes.append(h / 3 * 2 + h % 3);
	vertSizes.append(h / 3);

	VerticalSplitter->setSizes(vertSizes);
	SplittersInitialized = true;
	ChatEditBoxSizeManager::instance()->setCommonHeight(vertSizes.at(1));
}

void ChatWidget::commonHeightChanged(int commonHeight)
{
	QList<int> sizes = VerticalSplitter->sizes();

	int sum = 0;
	if (2 == sizes.count())
	{
		if (sizes.at(1) == commonHeight)
			return;
		sum = sizes.at(0) + sizes.at(1);
	}
	else
		sum = height();

	if (sum < commonHeight)
		commonHeight = sum / 3;

	sizes.clear();
	sizes.append(sum - commonHeight);
	sizes.append(commonHeight);
	VerticalSplitter->setSizes(sizes);
}

void ChatWidget::composingStopped()
{
	ComposingTimer.stop();
	IsComposing = false;

	if (currentProtocol() && currentProtocol()->chatStateService())
		currentProtocol()->chatStateService()->sendState(chat(), ChatStateService::StatePaused);
}

void ChatWidget::checkComposing()
{
	if (!IsComposing)
		composingStopped();
	else
		// Reset IsComposing to false, so if updateComposing() method doesn't set it to true
		// before ComposingTimer hits this method again, we will call composingStopped().
		IsComposing = false;
}

void ChatWidget::updateComposing()
{
	if (!currentProtocol() || !currentProtocol()->chatStateService())
		return;

	if (!ComposingTimer.isActive())
	{
		// If the text was deleted either by sending a message or explicitly by the user,
		// let's not report it as composing.
		if (edit()->toPlainText().isEmpty())
			return;

		currentProtocol()->chatStateService()->sendState(chat(), ChatStateService::StateComposing);

		ComposingTimer.start();
	}
	IsComposing = true;
}

void ChatWidget::contactActivityChanged(const Contact &contact, ChatStateService::State state)
{
	if (CurrentContactActivity == state)
		return;

	if (!CurrentChat.contacts().contains(contact))
		return;

	CurrentContactActivity = state;

	if (ChatConfigurationHolder::instance()->contactStateChats())
		MessagesView->contactActivityChanged(contact, state);

	if (ChatConfigurationHolder::instance()->contactStateWindowTitle())
		refreshTitle();

	if (CurrentContactActivity == ChatStateService::StateGone)
	{
		QString msg = "[ " + tr("%1 ended the conversation").arg(contact.ownerBuddy().display()) + " ]";
		Message message = Message::create();
		message.setMessageChat(CurrentChat);
		message.setType(MessageTypeSystem);
		message.setMessageSender(contact);
		message.setStatus(MessageStatusReceived);
		message.setContent(msg);
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		MessagesView->appendMessage(message);
	}
}

void ChatWidget::close()
{
	emit closed();
}

void ChatWidget::keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled)
{
	Q_UNUSED(input)

	if (handled)
		return;

	handled = keyPressEventHandled(e);
}
