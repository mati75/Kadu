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
 */

#include <QLabel>
#include <QResizeEvent>
#include <QFormLayout>
#include <QDialogButtonBox>

#include "kadu.h"
#include "message_box.h"
#include "misc.h"
#include "icons_manager.h"

#include "accountdialog.h"

AccountDialog::AccountDialog(Pop3Proto* acc, QWidget *parent): QDialog(parent, Qt::WDestructiveClose)
{
	kdebugf();
	setCaption(tr("Account edit"));
	layout = new QFormLayout(this);
	name = new QLineEdit(acc->getName(), this);
	layout->addRow(tr("Account name"), name);
	host = new QLineEdit(acc->getHost(), this);
	layout->addRow(tr("Server address"), host);
	port = new QSpinBox(0, 65535, 1, this);
	layout->addRow(tr("Port"), port);
	if(!acc->getPort())
		port->setValue(110);
	else
		port->setValue(acc->getPort());
	conntype = new QComboBox(this);
	layout->addRow(tr("Connection type"), conntype);
	conntype->insertItem(tr("No encryption"));
	conntype->insertItem(tr("Encrypted (StartTLS)"));
	conntype->insertItem(tr("Encrypted (immediately)"));
	conntype->setCurrentItem(acc->getEncryption());
	user = new QLineEdit(acc->getUser(), this);
	layout->addRow(tr("User"), user);
	password = new QLineEdit(acc->getPassword(), this);
	layout->addRow(tr("Password"), password);
	password->setEchoMode(QLineEdit::Password);

	QDialogButtonBox *buttons=new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
		Qt::Horizontal, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(save()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	
	layout->addRow(buttons);
	Acc = acc;
	show();
}

void AccountDialog::save()
{
	kdebugf();
	if(name->text()==""){
		MessageBox::msg(tr("Name of account must be set"));
		return;
	}
	Acc->setName(name->text());
	Acc->setHost(host->text());
	Acc->setPort(port->value());
	Acc->setUser(user->text());
	Acc->setPassword(password->text());
	Acc->setEncryption((SecureType)conntype->currentItem());
	accept();
}