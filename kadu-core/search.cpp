/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "chat/chat_manager.h"

#include "contacts/contact-manager.h"

#include "contacts/model/contact-list-model.h"

#include "gui/widgets/contacts-list-widget.h"

#include "gui/windows/contact-data-window.h"

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"
#include "toolbar.h"
#include "xml_config_file.h"

#include "../modules/gadu_protocol/gadu-protocol.h"

#include "search.h"

SearchActionsSlots *SearchDialog::searchActionsSlot;

ActionDescription *SearchDialog::firstSearchAction;
ActionDescription *SearchDialog::nextResultsAction;
ActionDescription *SearchDialog::stopSearchAction;
ActionDescription *SearchDialog::clearResultsAction;
ActionDescription *SearchDialog::addFoundAction;
ActionDescription *SearchDialog::chatFoundAction;

SearchDialog::SearchDialog(QWidget *parent)
	: KaduMainWindow(parent),
	only_active(0), e_uin(0), e_name(0), e_nick(0), e_byrFrom(0), e_byrTo(0), e_surname(0),
	c_gender(0), e_city(0), progress(0), r_uin(0), r_pers(0),
	seq(0), searchhidden(false), searching(false), workaround(false)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Search user in directory"));

	QWidget *centralWidget = new QWidget(this);

	QLabel *l_name;
	QLabel *l_nick;
	QLabel *l_byr;
	QLabel *l_byrFrom;
	QLabel *l_byrTo;
	QLabel *l_surname;
	QLabel *l_gender;
	QLabel *l_city;
	QLabel *l_uin;

	l_nick = new QLabel(tr("Nickname"), centralWidget);
	e_nick = new QLineEdit(centralWidget);
	connect(e_nick, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_nick, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_gender = new QLabel(tr("Gender"), centralWidget);
	c_gender = new QComboBox(centralWidget);
	c_gender->insertItem(0, " ");
	c_gender->insertItem(1, tr("Male"));
	c_gender->insertItem(2, tr("Female"));
	connect(c_gender, SIGNAL(activated(const QString &)), this, SLOT(personalDataTyped()));

	l_name = new QLabel(tr("Name"), centralWidget);
	e_name = new QLineEdit(centralWidget);
	connect(e_name, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_name, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_surname = new QLabel(tr("Surname"), centralWidget);
	e_surname = new QLineEdit(centralWidget);
	connect(e_surname, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_surname, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_byr = new QLabel(tr("Birthyear"), centralWidget);
	l_byrFrom = new QLabel(tr("from"), centralWidget);
	e_byrFrom = new QLineEdit(centralWidget);
	e_byrFrom->setMaxLength(4);
	e_byrFrom->setValidator(new QIntValidator(1, 2100, this));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_byrFrom, SIGNAL(textChanged(const QString &)), this, SLOT(byrFromDataTyped()));
	connect(e_byrFrom, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	l_byrTo = new QLabel(tr("to"), centralWidget);
	e_byrTo = new QLineEdit(centralWidget);
	e_byrTo->setEnabled(false);
	e_byrTo->setMaxLength(4);
	e_byrTo->setValidator(new QIntValidator(1, 2100, centralWidget));
	connect(e_byrTo, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_byrTo, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	l_city = new QLabel(tr("City"), centralWidget);
	e_city = new QLineEdit(centralWidget);
	connect(e_city, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(e_city, SIGNAL(returnPressed()), this, SLOT(firstSearch()));

	only_active = new QCheckBox(tr("Only active users"), centralWidget);
	connect(only_active, SIGNAL(clicked()), this, SLOT(personalDataTyped()));

	QGroupBox *qgrp1 = new QGroupBox(tr("Uin"), centralWidget);
	QHBoxLayout *uinLayout = new QHBoxLayout(qgrp1);
	l_uin = new QLabel(tr("Uin"),qgrp1);
	e_uin = new QLineEdit(qgrp1);
	e_uin->setMaxLength(8);
	e_uin->setValidator(new QIntValidator(1, 99999999, centralWidget));
	connect(e_uin, SIGNAL(textChanged(const QString &)), this, SLOT(uinTyped()));
	connect(e_uin, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	uinLayout->addWidget(l_uin);
	uinLayout->addWidget(e_uin);

	progress = new QLabel(centralWidget);

	resultsWidget = new ContactsListWidget(0, centralWidget);
	connect(resultsWidget, SIGNAL(contactActivated(Contact)), this, SLOT(selectionChanged(Contact)));

	QGroupBox * btngrp = new QGroupBox(tr("Search criteria"), centralWidget);
	QButtonGroup *buttonGroup = new QButtonGroup(btngrp);
	QHBoxLayout *btngrpLayout = new QHBoxLayout(btngrp);
	r_pers = new QRadioButton(tr("&Personal data"), btngrp);
	r_pers->setChecked(true);
	connect(r_pers, SIGNAL(toggled(bool)), this, SLOT(persClicked()));
	r_pers->setToolTip(tr("Search using the personal data typed above (name, nickname)..."));

	r_uin = new QRadioButton(tr("&Uin number"), btngrp);
	connect(r_uin, SIGNAL(toggled(bool)), this, SLOT(uinClicked()));
	r_uin->setToolTip(tr("Search for this UIN exclusively"));

	buttonGroup->addButton(r_pers);
	buttonGroup->addButton(r_uin);

	btngrpLayout->addWidget(r_pers);
	btngrpLayout->addWidget(r_uin);

	QGridLayout * grid = new QGridLayout(centralWidget);
	grid->addWidget(l_nick, 1, 0, Qt::AlignRight); grid->addWidget(e_nick, 1, 1);
	grid->addWidget(l_gender, 2, 0, Qt::AlignRight); grid->addWidget(c_gender, 2, 1);
	grid->addWidget(l_name, 1, 3, Qt::AlignRight); grid->addWidget(e_name, 1, 4);
	grid->addWidget(l_surname, 2, 3, Qt::AlignRight); grid->addWidget(e_surname, 2, 4);
	grid->addWidget(l_byr, 1, 6, Qt::AlignRight);
	grid->addWidget(l_byrFrom, 1, 7, Qt::AlignRight); grid->addWidget(e_byrFrom, 1, 8);
	grid->addWidget(l_byrTo, 2, 7, Qt::AlignRight); grid->addWidget(e_byrTo, 2, 8);
	grid->addWidget(l_city, 1, 10, Qt::AlignRight); grid->addWidget(e_city, 1, 11);

	grid->addWidget(only_active, 2, 10, 1, 2);
	grid->addWidget(qgrp1, 3, 0, 1, 4);
	grid->addWidget(btngrp, 3, 4, 1, 7);
	grid->addWidget(resultsWidget, 5, 0, 1, 12);
	grid->addWidget(progress, 6, 0, 1, 2);
// TODO: 0.6.6
// 	grid->setCol(2, 10);
// 	grid->addColSpacing(5, 10);
// 	grid->addColSpacing(9, 10);

	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	QStringList headers;
	headers << tr("Status") << tr("Uin") << tr("Name") << tr("City") << tr("Nickname") << tr("Birth year");
// TODO: 0.6.6
// 	results->setHeaderLabels(headers);
// 	results->setSortingEnabled(true);
// 	results->setAllColumnsShowFocus(true);
// 	results->setSelectionMode(QAbstractItemView::SingleSelection);
// 	results->setIndentation(false);
//	results->setResizeMode(Q3ListView::AllColumns);
//	for (int i = 1; i < 5; ++i)
//		results->setColumnWidthMode(i, Q3ListView::Maximum);

//	searchhidden = false;
// 	if (_whoisSearchUin)
// 		e_uin->insert(QString::number(_whoisSearchUin));

	setCentralWidget(centralWidget);

	if (loadToolBarsFromConfig("searchDockArea", Qt::BottomToolBarArea, true))
		writeToolBarsToConfig("search"); // port old config
	else
		loadToolBarsFromConfig("search"); // load new config

 	setActionState(stopSearchAction, false);
 	setActionState(firstSearchAction, false);
 	setActionState(nextResultsAction,false);
 	setActionState(clearResultsAction, false);
 	setActionState(addFoundAction, false);
 	setActionState(chatFoundAction, false);

	loadWindowGeometry(this, "General", "SearchDialogGeometry", 0, 50, 800, 350);

	SearchAccount = AccountManager::instance()->defaultAccount();
	SearchProtocol = SearchAccount->protocol();
	Service = SearchProtocol->searchService();

	connect(Service, SIGNAL(newResults(ContactList)),
			this, SLOT(newResults(ContactList)));

	kdebugf2();
}

SearchDialog::~SearchDialog()
{
	kdebugf();

	writeToolBarsToConfig("search");

 	saveWindowGeometry(this, "General", "SearchDialogGeometry");
	kdebugf2();
}

void SearchDialog::initModule()
{
	kdebugf();

	searchActionsSlot = new SearchActionsSlots();

	firstSearchAction = new ActionDescription(0,
		ActionDescription::TypeSearch, "firstSearchAction",
		searchActionsSlot, SLOT(firstSearchActionActivated(QAction *, bool)),
		"LookupUserInfo", tr("&Search")
	);
	QObject::connect(firstSearchAction, SIGNAL(actionCreated(KaduAction *)), searchActionsSlot, SLOT(firstSearchActionCreated(KaduAction *)));

	nextResultsAction = new ActionDescription(0,
		ActionDescription::TypeSearch, "nextResultsAction",
		searchActionsSlot, SLOT(nextResultsActionActivated(QAction *, bool)),
		"NextSearchResults", tr("&Next results")
	);
	QObject::connect(nextResultsAction, SIGNAL(actionCreated(KaduAction *)), searchActionsSlot, SLOT(nextResultsActionCreated(KaduAction *)));

	stopSearchAction = new ActionDescription(0,
		ActionDescription::TypeSearch, "stopSearchAction",
		searchActionsSlot, SLOT(stopSearchActionActivated(QAction *, bool)),
		"CloseWindow", tr("Stop")
	);
	QObject::connect(stopSearchAction, SIGNAL(actionCreated(KaduAction *)), searchActionsSlot, SLOT(stopSearchActionCreated(KaduAction *)));

	clearResultsAction = new ActionDescription(0,
		ActionDescription::TypeSearch, "clearSearchAction",
		searchActionsSlot, SLOT(clearResultsActionActivated(QAction *, bool)),
		"ClearSearchResults", tr("Clear results")
	);
	QObject::connect(clearResultsAction, SIGNAL(actionCreated(KaduAction *)), searchActionsSlot, SLOT(clearResultsActionCreated(KaduAction *)));

	addFoundAction = new ActionDescription(0,
		ActionDescription::TypeSearch, "addSearchedAction",
		searchActionsSlot, SLOT(addFoundActionActivated(QAction *, bool)),
		"AddUser", tr("Add selected user")
	);
	QObject::connect(addFoundAction, SIGNAL(actionCreated(KaduAction *)), searchActionsSlot, SLOT(actionsFoundActionCreated(KaduAction *)));

	chatFoundAction = new ActionDescription(0,
		ActionDescription::TypeSearch, "chatSearchedAction",
		searchActionsSlot, SLOT(chatFoundActionActivated(QAction *, bool)),
		"OpenChat", tr("&Chat")
	);
	QObject::connect(chatFoundAction, SIGNAL(actionCreated(KaduAction *)), searchActionsSlot, SLOT(actionsFoundActionCreated(KaduAction *)));

	kdebugf2();
}

void SearchDialog::closeModule()
{
	delete searchActionsSlot;
}

void SearchDialog::addFound()
{
	ContactList found = selected();

	foreach (Contact contact, found)
		(new ContactDataWindow(contact, kadu))->show();
}

void SearchDialog::chatFound()
{
	ContactList found = selected();
	if (found.size())
		chat_manager->openChatWidget(SearchAccount, found);
}

ContactList SearchDialog::selected()
{
	ContactList result;

// 	QTreeWidgetItem *selected = selectedItem();

// 	if (!selected)
// 		return result;
// 
// 	QString uin = selected->text(1);
// 	QString firstname = selected->text(2);
// 	QString nickname = selected->text(4);
// 
// 	bool ok;
// 	if (uin.toUInt(&ok) == 0 || !ok)
		return result;
/*
	QString display;
	if (!nickname.isEmpty()) // Build altnick. Trying user nick first.
		display = nickname;
	else if (!firstname.isEmpty()) // If nick is empty, try firstname.
		display = firstname;
	else
		display = uin; // If above are empty, use uin.

	Contact e = ContactManager::instance()->byId(SearchAccount, uin);*/
/*
	if (e.isAnonymous())
	{
		e.setFirstName(firstname);
		e.setNickName(nickname);
		e.setDisplay(display);
	}

	result.append(e);
	return result;*/
}

void SearchDialog::clearResults()
{
	results.clear();

 	setActionState(addFoundAction, false);
 	setActionState(clearResultsAction, false);
 	setActionState(chatFoundAction, false);
}

void SearchDialog::stopSearch()
{
	kdebugf();

	Service->stop();

	setActionState(stopSearchAction, false);

	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
		setActionState(firstSearchAction, true);
// 	if (!results->selectedItems().isEmpty())
// 	{
// 		if (r_pers->isChecked() && !isPersonalDataEmpty())
// 			setActionState(nextResultsAction, true);
// 
// 		setActionState(addFoundAction, true);
// 		setActionState(chatFoundAction, true);
// 	}
// 	if (results->topLevelItemCount() > 0)
		setActionState(clearResultsAction, true);

	kdebugf2();
}

void SearchDialog::firstSearch()
{
	kdebugf();

	if (r_pers->isChecked() && isPersonalDataEmpty())
		return;

	if (!Service)
	{
		MessageBox::msg(tr("Cannot search contacts in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}

	if (!results.isEmpty())
		results = ContactList();

	if (searching)
		Service->stop();

	Contact query;

	if (r_pers->isChecked())
	{
		query.setFirstName(e_name->text());
		query.setLastName(e_surname->text());
		query.setNickName(e_nick->text());
		query.setCity(e_city->text());

		if (((e_byrTo->text().isEmpty()) && (!e_byrFrom->text().isEmpty()))
		    || ((e_byrTo->text().toUShort()) < (e_byrFrom->text().toUShort())))
		{
			// TODo: 0.6.6
			e_byrTo->setText(e_byrFrom->text());
			query.setBirthYear(e_byrFrom->text().toUShort());
		}

		switch (c_gender->currentIndex())
		{
			case 1:
				query.setGender(ContactData::GenderMale);
				break;
			case 2:
				query.setGender(ContactData::GenderFemale);
				break;
		}

		// TODO: 0.6.6
// 		if (only_active->isChecked())
// 			searchRecord->reqActive();
	}
// 	else if (r_uin->isChecked()) TODO: 0.6.6
// 		searchRecord->reqUin(e_uin->text());

	searching = true;

 	setActionState(stopSearchAction, true);
 	setActionState(firstSearchAction, false);
 	setActionState(nextResultsAction, false);
 	setActionState(addFoundAction, false);
 	setActionState(chatFoundAction, false);

	Service->searchFirst(query);

	progress->setText(tr("Searching..."));

	kdebugf2();
}

void SearchDialog::nextSearch()
{
	kdebugf();

	if (!Service)
		return;

	searching = true;

 	setActionState(stopSearchAction, true);
 	setActionState(firstSearchAction, false);
 	setActionState(nextResultsAction, false);
 	setActionState(addFoundAction, false);
	setActionState(chatFoundAction, false);

	Service->searchNext();

	progress->setText(tr("Searching..."));

	kdebugf2();
}

void SearchDialog::newResults(ContactList contacts)
{
	kdebugf();

	results += contacts;
	resultsWidget->setModel(0);
	resultsModel = new ContactListModel(results);
	resultsWidget->setModel(resultsModel);

	progress->setText(tr("Done searching"));

//	if (results->topLevelItemCount() > 0)
//		results->topLevelItem(0)->setSelected(true);

//	searchhidden = false;
	if ((r_pers->isChecked() && !isPersonalDataEmpty()) || (r_uin->isChecked() && !e_uin->text().isEmpty()))
		setActionState(firstSearchAction, true);
	setActionState(stopSearchAction, false);

// 	if (searchResults.isEmpty()  || ((int)searchResults.count() == items))
// 	{
// 		kdebugmf(KDEBUG_INFO, "No results. Exit.\n");
// 		MessageBox::msg(tr("There were no results of your search"), false, "Information", this);
//		searchhidden = false;
// 	}
// 	else
// 	{
// 		if (r_pers->isChecked() && !isPersonalDataEmpty())
// 			setActionState(nextResultsAction, true);

// 		if (results->topLevelItemCount() > 0)
// 			setActionState(clearResultsAction, true);
// 	}

// 	if (!results->selectedItems().isEmpty())
// 	{
// 		setActionState(addFoundAction, true);
// 		setActionState(chatFoundAction, true);
// 	}

	searching = false;

	kdebugf2();
}

void SearchDialog::closeEvent(QCloseEvent * e)
{
	QWidget::closeEvent(e);
}

void SearchDialog::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
//	results->triggerUpdate();//workaround for bug in Qt, which do not refresh results properly
}

void SearchDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

void SearchDialog::uinTyped(void)
{
	r_uin->setChecked(true);

 	setActionState(firstSearchAction, !e_uin->text().isEmpty());
}

void SearchDialog::personalDataTyped(void)
{
	workaround = true;
	r_pers->setChecked(true);
	workaround = false;

 	setActionState(firstSearchAction, !isPersonalDataEmpty());
 	setActionState(nextResultsAction, false);
}

void SearchDialog::byrFromDataTyped(void)
{
	bool b = e_byrFrom->text().isEmpty();
	e_byrTo->setEnabled(!b);
	if (b)
		e_byrTo->setText(QString::null);
}

void SearchDialog::persClicked()
{
 	only_active->setEnabled(true);
 	only_active->setChecked(false);
	if (!workaround)
		e_nick->setFocus();

 	setActionState(firstSearchAction, !isPersonalDataEmpty());
}

void SearchDialog::uinClicked()
{
 	only_active->setEnabled(false);
	if (!workaround)
		e_uin->setFocus();

 	setActionState(firstSearchAction, !e_uin->text().isEmpty());
 	setActionState(nextResultsAction, false);
}

void SearchDialog::updateInfoClicked()
{
// 	kdebugf();
	// TODO: review this code
// 	QTreeWidgetItem *selected = selectedItem();
// 
// 	if (!selected)
// 		return;
// 
// 	QString uin = selected->text(1);
// 	QString firstname = selected->text(2);
//	QString lastname = selected->text(3);
// 	QString nickname = selected->text(4);

// 	Contact con = ContactManager::instance()->byId(SearchAccount, uin);

	// Build altnick. Try user nick first.
// 	QString altnick = nickname;
	// If nick is empty, try firstname+lastname.
// 	if (altnick.isEmpty())
// 	{
// 		altnick = firstname;
//		if (firstname.length() && lastname.length())
//			altnick += " ";
//		altnick += lastname;
// 	}
	// If nick is empty, use uin.
// 	if (altnick.isEmpty())
// 		altnick = uin;

// 	con.setFirstName(firstname);
// 	con.setNickName(nickname);

// 	(new ContactDataWindow(con, kadu))->show();
// 	kdebugf2();
}

bool SearchDialog::isPersonalDataEmpty() const
{
	return
		e_name->text().isEmpty() &&
		e_nick->text().isEmpty() &&
		e_byrFrom->text().isEmpty() &&
		e_surname->text().isEmpty() &&
		c_gender->currentIndex() == 0 &&
		e_city->text().isEmpty();
}

void SearchDialog::selectionChanged(Contact contact)
{
	//kdebugmf(KDEBUG_FUNCTION_START, "%p\n", );
//  	bool enableActions = results->selectedItems().count() > 0;
	bool enableActions = false;
 	setActionState(addFoundAction, enableActions);
 	setActionState(chatFoundAction, enableActions);
}

void SearchActionsSlots::firstSearchActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->firstSearch();
}

void SearchDialog::createDefaultToolbars(QDomElement toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "search_bottomDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

	addToolButton(toolbarConfig, "firstSearchAction", true);
	addToolButton(toolbarConfig, "nextResultsAction", true);
	addToolButton(toolbarConfig, "stopSearchAction", true);
	addToolButton(toolbarConfig, "clearSearchAction", true);
	addToolButton(toolbarConfig, "addSearchedAction", true);
	addToolButton(toolbarConfig, "chatSearchedAction", true);
}

void SearchDialog::setActionState(ActionDescription *actionDescription, bool toogle)
{
	KaduAction *action = actionDescription->action(this);
	if (action)
		action->setEnabled(toogle);
}

void SearchActionsSlots::firstSearchActionCreated(KaduAction *action)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(action->parent());
	if (!search)
		return;

	if (search->searching || (search->r_pers->isChecked() && search->isPersonalDataEmpty()) || (search->r_uin->isChecked() && search->e_uin->text().isEmpty()))
		action->setEnabled(false);
}

void SearchActionsSlots::nextResultsActionCreated(KaduAction *action)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(action->parent());
	if (!search)
		return;
	if (search->searching || search->r_uin->isChecked() || search->isPersonalDataEmpty())
		action->setEnabled(false);

}

void SearchActionsSlots::stopSearchActionCreated(KaduAction *action)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(action->parent());
	if (!search)
		return;
	if (!search->searching)
		action->setEnabled(false);
}

void SearchActionsSlots::clearResultsActionCreated(KaduAction *action)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(action->parent());
	if (!search)
		return;
// 	if (!search->results->topLevelItemCount())
// 		action->setEnabled(false);
}

void SearchActionsSlots::actionsFoundActionCreated(KaduAction *action)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(action->parent());
	if (!search)
		return;
// 	if (search->results->selectedItems().count() == 0)
// 		action->setEnabled(false);
}

void SearchActionsSlots::nextResultsActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->nextSearch();
}

void SearchActionsSlots::stopSearchActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->stopSearch();
}

void SearchActionsSlots::clearResultsActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->clearResults();
}

void SearchActionsSlots::addFoundActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->addFound();
}

void SearchActionsSlots::chatFoundActionActivated(QAction *sender, bool toggled)
{
	SearchDialog *search = dynamic_cast<SearchDialog *>(sender->parent());
	if (search)
		search->chatFound();
}
