/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gui/widgets/choose-identity-widget.h"
#include "gui/windows/message-dialog.h"
#include "icons-manager.h"
#include "protocols/protocols-manager.h"
#include "server/jabber-server-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-add-account-widget.h"

JabberAddAccountWidget::JabberAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

JabberAddAccountWidget::~JabberAddAccountWidget()
{
}

void JabberAddAccountWidget::createGui()
{
  	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	QWidget *jidWidget = new QWidget(this);
	QGridLayout *jidLayout = new QGridLayout(jidWidget);
	jidLayout->setSpacing(0);
	jidLayout->setMargin(0);
	jidLayout->setColumnStretch(0, 2);
	jidLayout->setColumnStretch(2, 2);

	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Username);

	AtLabel = new QLabel("@", this);
	jidLayout->addWidget(AtLabel, 0, 1);

	Domain = new QComboBox();
	Domain->setEditable(true);
	jidLayout->addWidget(Domain, 0, 2);

	layout->addRow(tr("Username") + ':', jidWidget);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	layout->addRow(0, RememberPassword);

	RemindPassword = new QLabel(QString("<a href='remind'>%1</a>").arg(tr("Remind password")));
	RemindPassword->setTextInteractionFlags(Qt::TextBrowserInteraction);
	layout->addRow(tr("Forgot Your Password?"), RemindPassword);

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

	connect(AddAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	dataChanged();
}

void JabberAddAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!Username->text().isEmpty());
	AddAccountButton->setEnabled(!Username->text().isEmpty()
				     && !AccountPassword->text().isEmpty()
				     && (!Domain->isVisible() || !Domain->currentText().isEmpty())
				     && Identity->currentIdentity());
}

void JabberAddAccountWidget::apply()
{
	Account jabberAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//jabberAccount.setProtocolName("jabber");
	jabberAccount.setAccountIdentity(Identity->currentIdentity());
	jabberAccount.setProtocolName("jabber");

	/*
	 * Because now there is one AddAccountWidget for XMPP, GTalk and FB, and only in FB the Domain field is hidden,
	 * so it’s not pretty but fast way to see if we’re adding FB account, which requires special connection settings.
	 */
	if (Domain->isVisible())
		jabberAccount.setId(Username->text() + '@' + Domain->currentText());
	else // facebook
		jabberAccount.setId(Username->text() + "@chat.facebook.com");
	jabberAccount.setPassword(AccountPassword->text());
	jabberAccount.setHasPassword(!AccountPassword->text().isEmpty());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());

	JabberAccountDetails *details = dynamic_cast<JabberAccountDetails *>(jabberAccount.details());
	if (details)
	{
		details->setState(StorableObject::StateNew);
		details->setResource("Kadu");
		details->setPriority(5);
		if (!Domain->isVisible())
		{
			details->setEncryptionMode(JabberAccountDetails::Encryption_No);
			details->setPlainAuthMode(JabberAccountDetails::NoAllowPlain);
		}
	}

	resetGui();

	emit accountCreated(jabberAccount);
}

void JabberAddAccountWidget::cancel()
{
	resetGui();
}

void JabberAddAccountWidget::resetGui()
{
	AccountPassword->setText("");
	Username->setText("");
	Domain->setCurrentIndex(-1);
	RememberPassword->setChecked(true);
	Identity->setCurrentIdentity(Identity::null);
}

void JabberAddAccountWidget::setDomainSectionVisible(bool visible)
{
	Domain->setVisible(visible);
	AtLabel->setVisible(visible);
}
