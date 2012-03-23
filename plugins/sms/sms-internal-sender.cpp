/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>

#include "configuration/configuration-file.h"
#include "gui/windows/message-dialog.h"
#include "debug.h"

#include "scripts/sms-script-manager.h"
#include "sms-gateway-manager.h"
#include "sms-gateway-query.h"
#include "sms-token-read-job.h"

#include "sms-internal-sender.h"

SmsInternalSender::SmsInternalSender(const QString &number, const SmsGateway &gateway, QObject *parent) :
		SmsSender(number, parent), Gateway(gateway)
{
}

SmsInternalSender::~SmsInternalSender()
{
}

void SmsInternalSender::sendMessage(const QString &message)
{
	Message = message;

	if (Gateway.signatureRequired() && !validateSignature())
	{
		emit finished(false, "dialog-error", tr("Signature can't be empty."));
		kdebugf2();
		return;
	}

	if (Gateway.id().isEmpty())
		queryForGateway();
	else
		sendSms();
}

void SmsInternalSender::queryForGateway()
{
	emit progress("dialog-information", tr("Detecting gateway..."));

	SmsGatewayQuery *query = new SmsGatewayQuery(this);
	connect(query, SIGNAL(finished(const QString &)), this, SLOT(gatewayQueryDone(const QString &)));
	query->process(number());
}

void SmsInternalSender::readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod)
{
	SmsTokenReadJob *job = new SmsTokenReadJob(callbackObject, callbackMethod);
	job->exec(tokenImageUrl);
}

void SmsInternalSender::gatewayQueryDone(const QString &gatewayId)
{
	if (gatewayId.isEmpty())
	{
		emit finished(false, "dialog-error", tr("Automatic gateway selection is not available. Please select SMS gateway manually."));
		kdebugf2();
		return;
	}

	Gateway = SmsGatewayManager::instance()->byId(gatewayId);

	emit progress("dialog-information", tr("Detected gateway: %1.").arg(Gateway.name()));

	sendSms();
}

QScriptValue SmsInternalSender::readFromConfiguration(const QString &group, const QString &name, const QString &defaultValue)
{
	return config_file.readEntry(group, name, defaultValue);
}

void SmsInternalSender::sendSms()
{
	emit gatewayAssigned(number(), Gateway.id());

	emit progress("dialog-information", tr("Sending SMS..."));

	QScriptEngine *engine = SmsScriptsManager::instance()->engine();

	QScriptValue jsGatewayManagerObject = engine->evaluate("gatewayManager");
	QScriptValue jsSendSms = jsGatewayManagerObject.property("sendSms");
	QScriptValueList arguments;
	arguments.append(Gateway.id());
	arguments.append(number());
	arguments.append(signature());
	arguments.append(Message);
	arguments.append(engine->newQObject(this));

	jsSendSms.call(jsGatewayManagerObject, arguments);
}

void SmsInternalSender::result()
{
	emit finished(true, "dialog-information", tr("SMS sent"));
}

void SmsInternalSender::failure(const QString &errorMessage)
{
	emit finished(false, "dialog-error", errorMessage);
}
