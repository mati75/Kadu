/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * autor
 * Micha³ Podsiadlik
 * michal at gov.one.pl
 *
 * obsluga maildir:
 * Konrad Klimaszewski
 * klimasz at if.pw.edu.pl>
 */

#include <stdlib.h>

#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QListWidget>

#include "config_file.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"

#include "mail.h"
#include "accountdialog.h"

#include "debug.h"

extern "C" KADU_EXPORT int mail_init(bool firstLoad)
{
	mail = new Mail();

	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mail.ui"), mail);
	notification_manager->registerEvent("Mail", "New mail", CallbackNotRequired);
	notification_manager->registerEvent("MailError", "Connection error", CallbackNotRequired);
	return 0;
}

extern "C" KADU_EXPORT void mail_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mail.ui"), mail);
	notification_manager->unregisterEvent("MailError");
	notification_manager->unregisterEvent("Mail");
	delete mail;
	mail = 0;
}

Mail::Mail()
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(checkmail()));

	import_0_5_0_Configuration();
	loadOldConfguration();

	QDomElement mail_elem = xml_config_file->findElement(
	xml_config_file->rootElement(), "Mail");
	
	QDomNodeList accounts_list = mail_elem.elementsByTagName("Account");

	for (unsigned int i = 0, cnt = accounts_list.count(); i < cnt; ++i)
	{
		UserListElement e;
		QDomElement acc_elem = accounts_list.item(i).toElement();
		Pop3Proto* acc=new Pop3Proto(
			acc_elem.attribute("Name"),
			acc_elem.attribute("Server"),
			acc_elem.attribute("Port").toInt(),
			acc_elem.attribute("User"),
			pwHash(acc_elem.attribute("Password")));

		connect(acc, SIGNAL(done(int,int,int,QString)),	SLOT(printstat(int,int,int,QString)));
		acc->setLastmails(acc_elem.attribute("Last").toInt());
		acc->setEncryption((SecureType)acc_elem.attribute("Encryption").toInt());
		accounts.append(acc);
	}

	createDefaultConfiguration();
	timer->start(config_file.readNumEntry("Mail", "Interval", 60)*1000);
	checkmail();
	kdebugm(KDEBUG_INFO, "Mail started\n");
}

void Mail::refreshCertyficates()
{
	foreach(Pop3Proto *acc, accounts)
		acc->loadCertyficate();
}

Mail::~Mail()
{
	kdebugf();
	// i zapisz wszystko na koniec
	configurationUpdated();
	delete timer;
	kdebugm(KDEBUG_INFO, "Mail stopped\n");
}

void Mail::checkmail()
{
	kdebugf();
	if(config_file.readBoolEntry("Mail", "LocalMaildir"))
		maildir();

	foreach(Pop3Proto *acc, accounts)
		acc->getStats();
}

void Mail::printstat(int last, int total, int size, QString name)
{
	kdebugf();
	
	if(total>last)
	{
		MailNotification *notification = new MailNotification();
		notification->setText(formatmessage(last, total, size, name));
		
		if(config_file.readBoolEntry("Mail", "RunClient"))
			openEmailClient();
		else
			notification->setCallbackEnabled();

		notification_manager->notify(notification);
	}
}

QString Mail::formatmessage(int last, int total, int size, QString name)
{
	QString msg, msgsize;
	/*
	 * %n liczba nowych wiadomo¶ci
	 * %t liczba wszystkich wiadomo¶ci
	 * %s rozmiar
	 * %a nazwa konta
	 */
	msg=config_file.readEntry("Mail", "Format");
	//ciekawe kto ma tak± wielk± poczte ??
	if(size>1073741824)
		msgsize.sprintf("%.2f GB", (float)size/1073741824);
	else if(size>1048576)
		msgsize.sprintf("%.2f MB", (float)size/1048576);
	else if(size>1024)
		msgsize.sprintf("%.2f kB", (float)size/1024);
	else
		msgsize.sprintf("%i B", size);

	msg.replace("%n", QString::number(total-last));
	msg.replace("%t", QString::number(total));
	msg.replace("%s", msgsize);
	msg.replace("%a", name);
	return msg;
}

void Mail::maildir()
{
	QString path;
	unsigned int totalsize = 0;
	int last;

	path = config_file.readEntry("Mail", "MaildirPath");
	last = config_file.readNumEntry("Mail", "LastMailDir", 0);

	path += "/new";
	if(path[0] == '~')
		path.replace(0, 1, QDir::homeDirPath());
	path = QDir::cleanDirPath(path);

	QDir dir(path);

	if(!dir.exists())
		connectionError(tr("Maildir not found!"));
	else if(!dir.isReadable())		
		connectionError(tr("Maildir is not readable!"));
	else 
	{
		const QFileInfoList filelist = dir.entryInfoList();
		foreach(const QFileInfo& file, filelist){
			if ( file.fileName() == "." || file.fileName() == ".." )
				; // nothing
			else
				totalsize += file.size();
		}
		config_file.writeEntry("Mail", "LastMailDir", last);
		printstat(last, dir.count()-2, totalsize, "MailDir");
	}
}

void Mail::onSelectMaildir()
{
	kdebugf();
	QString dir = QFileDialog::getExistingDirectory();
	if(dir!=QString::null)
		maildirLineEdit->setText(dir);
}

void Mail::onAddButton()
{
	kdebugf();
	Pop3Proto* acc=new Pop3Proto(tr("New"), "", 0, "", "");
	AccountDialog* dlg=new AccountDialog(acc, MainConfigurationWindow::instance());
	if(dlg->exec()==QDialog::Accepted)
	{
		connect(acc, SIGNAL(done(int,int,int,QString)),	SLOT(printstat(int,int,int,QString)));
		acc->setLastmails(0);
		accounts.append(acc);
		updateList();
	}
	else
		delete acc;
}

void Mail::onEditButton()
{
	kdebugf();

	foreach(Pop3Proto *acc, accounts)
		if(acc->getName()==accountsListBox->currentItem()->text())
		{
			AccountDialog* dlg=new AccountDialog(acc, MainConfigurationWindow::instance());
			if(dlg->exec()==QDialog::Accepted)
				updateList();
		}
}

void Mail::onRemoveButton()
{
	kdebugf();

	foreach(Pop3Proto *acc, accounts)
		if(acc->getName()==accountsListBox->currentItem()->text())
		{
			kdebugm(KDEBUG_INFO, "removing %s\n", acc->getName().latin1());
			acc->disconnect(this);
			accounts.removeOne (acc);
			updateList();
		}

}
void Mail::connectionError(QString msg)
{
	const UserListElements ules;
	Notification *notification = new Notification("MailError", "CriticalSmall", ules);
	notification->setText(msg);
	notification_manager->notify(notification);
}

void Mail::updateList()
{
	accountsListBox->clear();

	foreach(Pop3Proto *acc, accounts)
		accountsListBox->addItem(acc->getName());
}

void Mail::import_0_5_0_Configuration()
{
	ConfigFile* oldConfig = new ConfigFile(ggPath("mail.conf"));

	for(int i=0;;i++)
	{
		QString config_section;
		config_section.sprintf("Account_%i", i);
		if(!oldConfig->readEntry(config_section, "Name", "").isEmpty())
		{
			config_file.addVariable("Mail", config_section + "_Name", oldConfig->readEntry(config_section, "Name"));
			oldConfig->removeVariable(config_section, "Name");

			config_file.addVariable("Mail", config_section + "_Server", oldConfig->readEntry(config_section, "Server"));
			oldConfig->removeVariable(config_section, "Server");

			config_file.addVariable("Mail", config_section + "_ServerPort", oldConfig->readEntry(config_section, "ServerPort", "110"));
			oldConfig->removeVariable(config_section, "ServerPort");

			config_file.addVariable("Mail", config_section + "_User", oldConfig->readEntry(config_section, "User"));
			oldConfig->removeVariable(config_section, "User");

			config_file.addVariable("Mail", config_section + "_Password", oldConfig->readEntry(config_section, "Password", ""));
			oldConfig->removeVariable(config_section, "Password");

			config_file.addVariable("Mail", config_section + "_Encryption", oldConfig->readEntry(config_section, "Encryption"));
			oldConfig->removeVariable(config_section, "Encryption");

			config_file.addVariable("Mail", config_section + "_Last", oldConfig->readEntry(config_section, "Last"));
			oldConfig->removeVariable(config_section, "Last");
		}
		else
			break;
	}
	config_file.addVariable("Mail", "MaildirPath", oldConfig->readEntry("Mail", "MaildirPath", ""));
	config_file.addVariable("Mail", "LastMailDir", oldConfig->readEntry("Mail", "LastMailDir", ""));
	config_file.addVariable("Mail", "Format", oldConfig->readEntry("Mail", "Format", ""));
	config_file.addVariable("Mail", "RunClient", oldConfig->readEntry("Mail", "RunClient", ""));

	//domy¶lne ustawienia dymków z poprzedniej konfiguracji
	config_file.addVariable("Hints", "Event_Mail_fgcolor", oldConfig->readEntry("Mail", "HintColor", ""));
	config_file.addVariable("Hints", "Event_Mail_bgcolor", oldConfig->readEntry("Mail", "HintBackground", ""));
	config_file.addVariable("Hints", "Event_Mail_timeout", oldConfig->readEntry("Mail", "HintTimeout", ""));
	oldConfig->sync();
	delete oldConfig;
}

void Mail::loadOldConfguration()
{
	for(int i=0;;i++)
	{
		QString config_section;
		config_section.sprintf("Account_%i", i);
		if(config_file.readEntry("Mail", config_section + "_Name").length())
		{
			Pop3Proto* acc=new Pop3Proto(
					config_file.readEntry("Mail", config_section + "_Name"),
					config_file.readEntry("Mail", config_section + "_Server"),
					config_file.readNumEntry("Mail", config_section + "_ServerPort", 110),
					config_file.readEntry("Mail", config_section + "_User"),
					pwHash(config_file.readEntry("Mail", config_section + "_Password", "")));

			config_file.removeVariable("Mail", config_section + "_Name");
			config_file.removeVariable("Mail", config_section + "_Server");
			config_file.removeVariable("Mail", config_section + "_ServerPort");
			config_file.removeVariable("Mail", config_section + "_User");
			config_file.removeVariable("Mail", config_section + "_Password");
			
			connect(acc, SIGNAL(done(int,int,int,QString)),	SLOT(printstat(int,int,int,QString)));
			acc->setLastmails(config_file.readNumEntry("Mail", config_section + "_Last"));
			accounts.append(acc);
		}
		else
			break;
	}
}

void Mail::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigGroupBox *accountsGroupBox = mainConfigurationWindow->configGroupBox("Mail", "General", "Accounts");

	QWidget *accountsWidget = new QWidget(accountsGroupBox->widget());

	QHBoxLayout *accountsHBox = new QHBoxLayout(accountsWidget);
	accountsHBox->setSpacing(5);

	accountsListBox = new QListWidget(accountsWidget);

	QWidget *buttons = new QWidget(accountsWidget);
	QVBoxLayout *buttonsLayout = new QVBoxLayout(buttons);
	buttonsLayout->setSpacing(5);

	QPushButton *add = new QPushButton(icons_manager->loadIcon("AddSelectPathDialogButton"),
		tr("Add"), buttons);
	QPushButton *remove = new QPushButton(icons_manager->loadIcon("RemoveSelectPathDialogButton"),
		tr("Remove"),buttons);
	QPushButton *edit = new QPushButton(icons_manager->loadIcon("ChangeSelectPathDialogButton"),
		tr("Edit"),buttons);

	buttonsLayout->addWidget(add);
	buttonsLayout->addWidget(remove);
	buttonsLayout->addWidget(edit);
	buttonsLayout->addStretch(50);

	accountsHBox->addWidget(accountsListBox);
	accountsHBox->addWidget(buttons);

	accountsGroupBox->addWidget(accountsWidget);

	connect(add, SIGNAL(clicked()), this, SLOT(onAddButton()));
	connect(remove, SIGNAL(clicked()), this, SLOT(onRemoveButton()));
	connect(edit, SIGNAL(clicked()), this, SLOT(onEditButton()));
	updateList();

	connect(mainConfigurationWindow->widgetById("mail/local_maildir"),	SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("mail/maildir_path"), SLOT(setEnabled(bool)));
}

void Mail::configurationUpdated()
{
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement mail_elem = xml_config_file->accessElement(root_elem, "Mail");
	xml_config_file->removeChildren(mail_elem);

	foreach(Pop3Proto *acc, accounts)
	{
		QDomElement account_elem = xml_config_file->createElement(mail_elem, "Account");
		account_elem.setAttribute("Name", acc->getName());
		account_elem.setAttribute("Server", acc->getHost());
		account_elem.setAttribute("Port", acc->getPort());
		account_elem.setAttribute("User", acc->getUser());
		account_elem.setAttribute("Password", pwHash(acc->getPassword()));
		account_elem.setAttribute("Last", acc->getLastmails());
		account_elem.setAttribute("Encryption", acc->getEncryption());
	}
}

void Mail::createDefaultConfiguration()
{
	config_file.addVariable("Mail", "LastMailDir", "0");
	config_file.addVariable("Mail", "MaildirPath", "~/Maildir");
	config_file.addVariable("Mail", "LocalMaildir", "false");
	config_file.addVariable("Mail", "Format", tr("You have %n new mail(s) on %a and total you have %t mail(s) witch total size %s"));
	config_file.addVariable("Mail", "RunClient", "false");
	config_file.addVariable("Mail", "Interval", "60");
	//domy¶lne ustawienia dymków z poprzedniej konfiguracji
	config_file.addVariable("Notify", "Mail_Hints", "true");
	config_file.addVariable("Notify", "MailError_Hints", "true");
}

void Mail::openEmailClient()
{
	kdebugf();
	// TODO: Tymczasowo musi zostac w taki spsuty sposob
	openMailClient(QString(""));
}

MailNotification::MailNotification(): Notification("Mail", "Message", UserListElements())
{
}

void MailNotification::setCallbackEnabled()
{
	addCallback(tr("Read"), SLOT(openEmailClient()));
}

void MailNotification::openEmailClient()
{
	kdebugf();
	mail->openEmailClient();
}

Mail* mail;
