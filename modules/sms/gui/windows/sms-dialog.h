/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_DIALOG_H
#define SMS_DIALOG_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class SelectBuddyComboBox;

class SmsDialog : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	QLineEdit *RecipientEdit;
	SelectBuddyComboBox *RecipientComboBox;
	QComboBox *ProviderComboBox;
	QTextEdit *ContentEdit;
	QLabel *LengthLabel;
	QLineEdit *ContactEdit;
	QLineEdit *SignatureEdit;
	QPushButton *SendButton;
	QCheckBox *SaveInHistoryCheckBox;

	void createGui();

private slots:
    void recipientBuddyChanged(Buddy buddy);
	void recipientNumberChanged(const QString &number);

	void editReturnPressed();
	void updateCounter();

	void gatewayAssigned(const QString &number, const QString &gatewayId);
	void sendSms();

	void clear();

protected:
	virtual void configurationUpdated();
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit SmsDialog(QWidget *parent = 0);
	virtual ~SmsDialog();

	void setRecipient(const QString &phone);

};

/** @} */

#endif // SMS_DIALOG_H
