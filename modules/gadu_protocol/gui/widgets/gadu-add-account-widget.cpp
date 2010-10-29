/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "accounts/account-manager.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocols-manager.h"
#include "qt/long-validator.h"
#include "html_document.h"
#include "icons-manager.h"

#include "gui/windows/gadu-remind-password-window.h"
#include "server/gadu-server-register-account.h"
#include "gadu-account-details.h"
#include "gadu-protocol-factory.h"
#include "token-widget.h"

#include "gadu-add-account-widget.h"

GaduAddAccountWidget::GaduAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

GaduAddAccountWidget::~GaduAddAccountWidget()
{
}

void GaduAddAccountWidget::apply()
{
}

void GaduAddAccountWidget::cancel()
{
}

void GaduAddAccountWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountId = new QLineEdit(this);
	AccountId->setValidator(new LongValidator(1, 3999999999U, this));
	connect(AccountId, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Gadu-Gadu number") + ':', AccountId);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	layout->addRow(0, RememberPassword);

	RemindPassword = new QLabel(QString("<a href='remind'>%1</a>").arg(tr("Forgot Your Password?")));
	RemindPassword->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	layout->addRow(0, RemindPassword);
	connect(RemindPassword, SIGNAL(linkActivated(QString)), this, SLOT(remindPasssword()));

	Identity = new IdentitiesComboBox(this);
	connect(Identity, SIGNAL(identityChanged(Identity)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', Identity);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	AddAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Add Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(AddAccountButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	connect(AddAccountButton, SIGNAL(clicked(bool)), this, SLOT(addAccountButtonClicked()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelButtonClicked()));

	dataChanged();
}

void GaduAddAccountWidget::resetGui()
{
	AccountId->setText("");
	AccountPassword->setText("");
	RememberPassword->setChecked(true);
	Identity->setCurrentIdentity(Identity::null);
}

void GaduAddAccountWidget::addAccountButtonClicked()
{
	Account gaduAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//gaduAccount.setProtocolName("gadu");
	gaduAccount.setAccountIdentity(Identity->currentIdentity());
	gaduAccount.setProtocolName("gadu");
	gaduAccount.setId(AccountId->text());
	gaduAccount.setPassword(AccountPassword->text());
	gaduAccount.setHasPassword(!AccountPassword->text().isEmpty());
	gaduAccount.setRememberPassword(RememberPassword->isChecked());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(gaduAccount.details());
	if (details)
	{
		details->setState(StorableObject::StateNew);
		details->setInitialRosterImport(true);
	}

	resetGui();

	emit accountCreated(gaduAccount);
}

void GaduAddAccountWidget::cancelButtonClicked()
{
	resetGui();
}

void GaduAddAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!AccountId->text().isEmpty());

	AddAccountButton->setEnabled(
		!AccountId->text().isEmpty() &&
		!AccountPassword->text().isEmpty() &&
		Identity->currentIdentity()
	);
}

void GaduAddAccountWidget::remindPasssword()
{
	bool ok;
	int uin = AccountId->text().toInt(&ok);
	if (ok)
		(new GaduRemindPasswordWindow(uin))->show();
}
