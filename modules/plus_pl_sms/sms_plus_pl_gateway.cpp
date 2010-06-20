/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sms_plus_pl_gateway.h"
#include <QMessageBox>

#include "config_file.h"
#include "debug.h"
#include "modules.h"

SmsPlusPlGateway::SmsPlusPlGateway(QObject* parent, const char *name)
	: SmsGateway(parent)
{
	modules_manager->moduleIncUsageCount("plus_pl_sms");
	connect( &timer, SIGNAL(timeout()), this, SLOT(checkIfFinished()) );
	connect( this, SIGNAL(displayInfosSignal()), this, SLOT(displayInfos()) );
}

SmsPlusPlGateway::~SmsPlusPlGateway()
{
	modules_manager->moduleDecUsageCount("plus_pl_sms");
}

void SmsPlusPlGateway::send(const QString& number, const QString& message, const QString& /*contact*/, const QString&signature)
{
	kdebugf();
	QDialog* p=(QDialog*)(parent()->parent());
	if(config_file.readEntry("SMS", "PlusPlGateway_User") != "" && config_file.readEntry("SMS", "PlusPlGateway_Pass") != "")
	{
		sendThread.setMsg(message);
		sendThread.setNr(number);
		sendThread.setDisplayInfos(config_file.readBoolEntry("SMS", "plus_pl_sms_display_infos", false));
		sendThread.setSignature(signature);
		kdebugm(KDEBUG_INFO, "Starting timer.\n");
		timer.start(500, FALSE);
		sendThread.start();
	}
	else
	{
		QMessageBox::critical(p, "SMS", tr("You must specify your login and password to Plus.pl in SMS tab in configure dialog!"));
		State = SMS_LOADING_RESULTS;
		emit finished(false);
	}
	kdebugf2();
}

void SmsPlusPlGateway::emitFinished(bool success)
{
	kdebugf();
	emit finished(success);
	kdebugf2();
}

SmsGateway*SmsPlusPlGateway::isValidPlusPl(const QString& number, QObject* parent)
{
	if(isNumberCorrect(number))
		return new SmsPlusPlGateway(parent, "sms_plus_pl_gateway");
	else
		return NULL;
}

bool SmsPlusPlGateway::isNumberCorrect(const QString& number)
{
	kdebugf();
	QString handleType = config_file.readEntry("SMS", "HandleType", "Always");
	// TODO: Investigate the compilation error
	//kdebugm(KDEBUG_INFO, "#Prezu# %s, %d: handleType=%s, (number[0]=%c, (QChar(number[2])-'0').latin1()=%d\n",
	//		__PRETTY_FUNCTION__, __LINE__, handleType.data(), number[0].latin1(), (number[2]-'0')%2);
	if(handleType == "always") 
		return true;
	else if(handleType == "never")
	{
		// TODO: Take a look at the number verification
		return	((number[0]=='6' && (QChar(number[2]).digitValue() - '0') % 2 != 0) ||
			 (number[0]=='7' &&  number[1] == '8' && number[2] == '7') ||
			 (number[0]=='8' &&  number[1] == '8' && number[2] == '8') ||
			 (number[0]=='8' &&  number[1] == '8' && number[2] == '9') ||
			 (number[0]=='8' &&  number[1] == '8' && number[2] == '0') ||
			 (number[0]=='8' &&  number[1] == '8' && number[2] == '6'));
	}
	else
	{
		// tu kod nigdy nie dochodzi
		return true;
	}
}

void SmsPlusPlGateway::httpRedirected(QString link)
{
}

void SmsPlusPlGateway::httpFinished()
{
}

void SmsPlusPlGateway::displayInfos()
{
	QDialog* p=(QDialog*)(parent()->parent());
	QMessageBox::information(p, "SMS", sendThread.getInfosMsg());
}

void SmsPlusPlGateway::checkIfFinished()
{
	QDialog* p=(QDialog*)(parent()->parent());
	kdebugf();
	if(sendThread.isFinished())
	{
		timer.stop();
		bool success = sendThread.isSuccess();
		kdebugm(KDEBUG_INFO, "SUCCESS: %d!\n", success);
		kdebugm(KDEBUG_INFO, "Stopping timer.\n");
		State = SMS_LOADING_RESULTS;
		emit finished(success);
		if(!success)
			QMessageBox::critical(p, "SMS", sendThread.getErrorMsg());
		else 
		{
			if(sendThread.getDisplayInfos())
				emit displayInfosSignal();
		}
	}
	else
		kdebugm(KDEBUG_INFO, "Not finished yet...\n");
}

