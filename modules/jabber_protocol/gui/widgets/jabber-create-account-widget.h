/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_CREATE_ACCOUNT_WIDGET_H
#define JABBER_CREATE_ACCOUNT_WIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "gui/widgets/identities-combo-box.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;
class JabberServerRegisterAccount;
class TokenWidget;

class JabberCreateAccountWidget : public QWidget
{
	Q_OBJECT
	
	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	IdentitiesComboBox *IdentityCombo;
	QPushButton *RegisterAccountButton;

	QPushButton *ExpandConnectionOptionsButton;
	QWidget *OptionsWidget;
	QCheckBox *CustomHostPort;
	QHBoxLayout *HostPortLayout;
	QLabel *CustomHostLabel;
	QLineEdit *CustomHost;
	QLabel *CustomPortLabel;
	QLineEdit *CustomPort;
	QLabel *EncryptionModeLabel;
	QComboBox *EncryptionMode;
	QCheckBox *LegacySSLProbe;

	int ssl_;
	bool opt_host_, legacy_ssl_probe_;
	QString host_;
	int port_;
	bool ShowConnectionOptions;
	
	void createGui();
	bool checkSSL();
	void resetGui();

private slots:
	void dataChanged();
	void registerNewAccountFinished(JabberServerRegisterAccount *jsra);
	void connectionOptionsChanged();
	void hostToggled(bool on);
	void sslActivated(int i);
	void cancel();
	virtual void apply();
public:
	explicit JabberCreateAccountWidget(QWidget *parent = 0);
	virtual ~JabberCreateAccountWidget();

signals:
	void accountCreated(Account account);
	void cancelled();

};

#endif // JABBER_CREATE_ACCOUNT_WIDGET_H
