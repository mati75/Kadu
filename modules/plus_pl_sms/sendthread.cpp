/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QTcpSocket>
#include <QTextCodec>

#include "sendthread.h"
#include "config_file.h"
#include "misc.h"
#include "debug.h"

SendThread::SendThread()
{
	initCurl();
	displayInfos = false;
}

SendThread::~SendThread()
{
	kdebugf();
	if(running())
	{
		kdebugm(KDEBUG_INFO, "Thread is running! Terminating!\n");
		terminate();
		wait();
	}
	cleanup();
}

void SendThread::initCurl()
{
	kdebugf();
	caBundlePath = dataPath("kadu/modules/data/plus_pl_sms/curl-ca-bundle.crt").toAscii();
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
	curl_easy_setopt(curl, CURLOPT_CAINFO, caBundlePath.constData());
	curl_easy_setopt(curl, CURLOPT_AUTOREFERER, TRUE);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorMsg);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
	//curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8080");
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, (void *)getBody);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	setErrorType(NO_ERROR);
	kdebugf2();
}

void SendThread::cleanup()
{
	kdebugf();
	curl_easy_cleanup(curl);
	kdebugf2();
}
	
bool SendThread::performGet(QString path)
{
	kdebugf();
	QByteArray asciiPath = path.toAscii();
	curl_easy_setopt(curl, CURLOPT_HTTPGET, TRUE);
	curl_easy_setopt(curl, CURLOPT_URL, asciiPath.constData());
	kdebugm(KDEBUG_INFO, "GET...\n");
	body = "";
	kdebugm(KDEBUG_INFO, "In performGet: errorType = %d\n", errorType);
	if(!curl_easy_perform(curl))
	{
		kdebugm(KDEBUG_INFO, "GET success.\n");
		return true;
	}
	else
	{
		setSuccess(false);
		setErrorType(CONNECTION_ERROR);
		kdebugm(KDEBUG_INFO, "GET FAILED!\n");
		return false;
	}
}

bool SendThread::performPost(QString path, QString postData)
{
	kdebugf();
	QByteArray asciiPostData = postData.toAscii();
	QByteArray asciiPath = path.toAscii();
	curl_easy_setopt(curl, CURLOPT_POST, TRUE);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, asciiPostData.length());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, asciiPostData.constData());
	kdebugm(KDEBUG_INFO, "Host...\n");
	curl_easy_setopt(curl, CURLOPT_URL, asciiPath.constData());
	body = "";
	kdebugm(KDEBUG_INFO, "About to post: %s\n", asciiPostData.constData());
	if(!curl_easy_perform(curl))
	{
		kdebugm(KDEBUG_INFO, "POST success.\n");
		return true;
	}
	else
	{
		setSuccess(false);
		setErrorType(CONNECTION_ERROR);
		kdebugm(KDEBUG_INFO, "POST FAILED!\n");
		return false;
	}
}

bool SendThread::getSentSMSesInfo()
{
	if(!performGet("http://www1.plus.pl/rozrywka_i_informacje/sms/send_sms.jsp"))
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Getting info about sent messages FAILED!\n");
		setErrorType(UNKNOWN_ERROR);
		return false;
	}
	else
	{
		kdebugm(KDEBUG_INFO, "Got info about sent messages.\n");
		QString temp;
		QString sentSMSesToOthersStr;
		QString numberSubstr;
		QRegExp info(">\\d+ pkt<");
		QTextStream s(&body, QIODevice::IO_ReadOnly);
		bool isL = false;
		while(!s.atEnd())
		{
			temp = s.readLine();
			if (isL){
				sentSMSesToOthersStr = temp;
				break;
			}
			if(temp.contains("do innych sieci"))
				isL = true;
		}

		int searchIndex = info.search(sentSMSesToOthersStr);
		kdebugm(KDEBUG_INFO, "searchIndex: %d\n", searchIndex);
		temp = info.cap(0);
		othersInfos = temp.mid(1, temp.length() - 2);
		
		return true;
	}
}


QString SendThread::getJSessCookie(QString &body)
{
	kdebugf();
	QTextStream s(&body, QIODevice::IO_ReadOnly);
	QString ret, temp;
	while(!s.atEnd())
	{
		temp = s.readLine();
		if(temp.startsWith("Set-Cookie:") &&
				temp.contains("JSESSION"))
		{
			kdebugm(KDEBUG_INFO, "Got JSESSION cookie!!\n");
			// Cut off "Set-" at the beggining
			ret = temp.right(temp.length() - 4);
			break;
		}
	}

	return ret;
}

bool SendThread::login()
{
	kdebugm(KDEBUG_INFO, "post data...\n");
	char *tmp;
	QTextCodec *codec = QTextCodec::codecForName("ISO8859-2");
	QByteArray cClassTmp = codec->fromUnicode(config_file.readEntry("SMS", "PlusPlGateway_User"));
	tmp = curl_escape(cClassTmp.constData(), cClassTmp.length());
	QString userUrlEncoded = tmp;
	curl_free(tmp);
	cClassTmp = codec->fromUnicode(config_file.readEntry("SMS", "PlusPlGateway_Pass"));
	tmp = curl_escape(cClassTmp.constData(), cClassTmp.length());
        QString passUrlEncoded = tmp;
	curl_free(tmp);
	QString postData = "op=login&login=" + userUrlEncoded + "&password=" + passUrlEncoded +
		"&Zaloguj=Zaloguj";
	if(!performPost("https://www1.plus.pl/sso/logowanie/auth", postData))
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Login FAILED!\n");
		return false;
	}
	else
	{
		kdebugm(KDEBUG_INFO, "Login posted.\n");
	}
	if(!performGet("http://www1.plus.pl/bsm/auth"))
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Login FAILED!\n");
		return false;
	}

	JSESSCookie = getJSessCookie(body);

	return true;
}

bool SendThread::postSMS()
{
	kdebugf();
	QString confirmationType = config_file.readEntry("SMS", "ConfirmationType", "none");
	QString notifyCode = "0";
	if ( confirmationType == "www" )
		notifyCode = "10";
	else if ( confirmationType == "sms" )
		notifyCode = "30";
	QString separator;
	separator += (const char)0xef;
	separator += (const char)0xbf;
	separator += (const char)0xbf;
	QString postData =
		"3" + separator + "0" + separator + "9" + separator +
		"http://www1.plus.pl/bsm/" + separator +
		"BB205D38BD6DC27BAB2E91C384A21325" + separator +
		"pl.plus.map.bsm.gwt.client.service.SendSmsService" + separator + "send" + separator +
		"pl.plus.map.bsm.gwt.client.dto.SmsMessageTO" + separator +
		"pl.plus.map.bsm.gwt.client.dto.SmsMessageTO/2299909364" + separator +
		msg + separator +
		nr + separator +
		signature + separator + // TODO: Handle sender!!!!
		// 1...2...3...4...1...5...6...0...0...0...7...0...0...1...1...0...0...0...0...0...0...0...0...0...0...0...8...0...0...0...9...0...0...0...
		"1" + separator +
		"2" + separator +
		"3" + separator +
		"4" + separator +
		"1" + separator +
		"5" + separator +
		"6" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"7" + separator +
		"0" + separator +
		"0" + separator +
		"1" + separator +
		"1" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"8" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator +
		"9" + separator +
		"0" + separator +
		"0" + separator +
		"0" + separator + (char)0x0d + (char)0x0a +
		(char)0x0d + (char)0x0a;

	QTcpSocket socket;

	kdebugm(KDEBUG_INFO, "Posting sms...\n");
	socket.connectToHost("www1.plus.pl", 80);
	if(!socket.waitForConnected())
	{
		kdebugm(KDEBUG_INFO, "Failed to establish "
			       "connection to www1.plus.pl");
		return false;
	}

	if(socket.state() != QAbstractSocket::ConnectedState)
	{
		kdebugm(KDEBUG_INFO, "Failed to establish "
				"connection to www1.plus.pl!! "
				"state is %d", socket.state());
		return false;
	}

	if(!socket.isValid())
	{
		kdebugm(KDEBUG_INFO, "Failed to establish "
				"connection to www1.plus.pl!! "
				"Socket is NOW valid!!");
		return false;
	}

	kdebugm(KDEBUG_INFO, "Ready to write to the socket\n");
	QString fullData("POST /bsm/service/SendSmsService HTTP/1.1\r\n");
	fullData += "Accept: */*\r\n";
	fullData += "Accept-Language: pl\r\n";
	fullData += "Referer: http://www1.plus.pl/bsm/service/SendSmsService\r\n";
	fullData += "Content-Type: text/plain; charset=utf-8\r\n";
	fullData += "Accept-Encoding: gzip, deflate\r\n";
	fullData += "User-Agent: Mozilla/5.0 (X11; U; Linux i686; pl; rv:1.9.0.7) Gecko/2009021906 Firefox/3.0.11\r\n";
	fullData += "Host: www1.plus.pl\r\n";
	fullData += "Content-Length: ";
	fullData += QString().number(postData.length());
	fullData += "\r\nPragma: no-cache\r\n";
	fullData += JSESSCookie;
	fullData += "\r\nConnection: keep-alive\r\n";
	fullData += "\r\n";
	fullData += postData;
	fullData += "\r\n";
	QByteArray fullDataAscii(fullData.toAscii());
	socket.write(fullDataAscii);
	socket.flush();
	int numRead;
	char buf[500];
	bodyOfTcpSocket = "";
	QString tmpStr;
	while(true)
	{
		numRead = socket.read(buf, 500);
		for(int i = 0; i < numRead; i++)
			bodyOfTcpSocket += buf[i];
		if(0 == numRead && !socket.waitForReadyRead(5000))
			break;
	}
	socket.disconnectFromHost();

	return true;
}

bool SendThread::logout()
{
	kdebugm(KDEBUG_INFO, "Logging out...\n");
	if(!performGet("http://www1.plus.pl/sso/logowanie/form?TAM_OP=do.logout"))
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Logging out FAILED!\n");
		return false;
	}
	else
	{
		kdebugm(KDEBUG_INFO, "Logged out.\n");
		return true;
	}
}

size_t getBody(void *buffer, size_t size, size_t nmemb, SendThread *sendThread)
{
	kdebugf();
	char *buf;
	buf = (char *)buffer;
	int bodyLen = nmemb * size;
	sendThread->body.reserve(bodyLen);
	for(int i = 0; i < bodyLen; i++)
		sendThread->body += buf[i];
	
	// ********** DEBUG **********
	/*
	QFile log("/home/patryk/plus_pl_sms.log");
	if(log.open(IO_WriteOnly | IO_Append))
	{
		kdebugm(KDEBUG_INFO, "Opening log file success.\n");
		QTextStream s(&log);
		s << sendThread->body << flush;
		log.close();
	}
	else
	{
		kdebugm(KDEBUG_INFO, "Opening log file FAILED!\n");
		kdebugm(KDEBUG_INFO, "Because of: %s\n", log.errorString().toAscii().constData());
	}
	*/
	// ********** DEBUG **********
	
	return bodyLen;
}

void SendThread::run()
{
	kdebugf();
	sentSMSesToPlus = sentSMSesToOthers = 0;
	setFinished(false);
	setErrorType(NO_ERROR);
	setSuccess();
	
	kdebugm(KDEBUG_INFO, "Logging in.\nSUCCESS = %d\n", success);

	// Logowanie do bramki
	if(!login())
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Login FAILED!\n");
		return;
	}
	kdebugm(KDEBUG_INFO, "Checking login.\nSUCCESS = %d\n", success);
	if(!validLogin())
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Login FAILED! - wrong login, or password.\n");
		return;
	}
	kdebugm(KDEBUG_INFO, "Posting SMS.\nSUCCESS = %d\n", success);
	
	// Wysylanie smsa
	if(!postSMS())
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Sending SMS FAILED!\n");
		logout();
		return;
	}
	
	kdebugm(KDEBUG_INFO, "Checking if SMS was sent.\nSUCCESS = %d\n", success);
	if(!validSMSSend())
	{
		setFinished();
		kdebugm(KDEBUG_INFO, "Sending SMS FAILED!\n");
		logout();
		return;
	}
	kdebugm(KDEBUG_INFO, "Getting info of sent SMSes.\nSUCCESS = %d\n", success);
	
	// Info o wyslanych sms-ach
	if(displayInfos)
		if(!getSentSMSesInfo())
		{
			setFinished();
			kdebugm(KDEBUG_INFO, "No sent SMSes info.\n");
			logout();
		}
	else
		kdebugm(KDEBUG_INFO, "Not getting infos.\n");
	
	// Wylogowuje
	kdebugm(KDEBUG_INFO, "Logging out.\nSUCCESS = %d\n", success);
	logout();
	kdebugm(KDEBUG_INFO, "SUCCESS = %d\n", success);

	kdebugm(KDEBUG_INFO, "setting finished to TRUE.\n");
	setFinished();
	kdebugf2();
	kdebugm(KDEBUG_INFO, "Emit finished...\n");
	kdebugf2();
}

QString SendThread::getErrorMsg()
{
	kdebugm(KDEBUG_INFO, "isSuccess: %d\nerrorType: %d\n", success, errorType);
	if(isSuccess())
		return QString("");
	QString err(errorMsg);
	QString retMsg;
	//kdebugm(KDEBUG_INFO, "%s\n", errorMsg);
	if(errorType == CONNECTION_ERROR)
	{
		kdebugm(KDEBUG_INFO, "%s\n", err.toAscii().constData());
		if(err.contains("couldn't connect to host"))
			retMsg = tr("Problem with connection to www.plus_pl.pl!");
		else if(err.contains("SSL certificate problem, verify that the CA cert is OK."))
			retMsg = tr("Certificate verification error!!! Someone is propabely messing up with "
					"you!!! Aborting.") + "\n" + tr("libcurl said:") + "\n" + QString(errorMsg);
		else
			retMsg = tr("Some connection error has occured!") + "\n" + tr("libcurl said:") + "\n" + 
				QString(errorMsg);
	}
	else if(getErrorType() == INVALID_LOGIN)
		retMsg = tr("Login failed! Propabely wrong login name or password. SMS was not sent.");
	else if(getErrorType() == NO_FREE_MESSAGES_TO_OTHER_NETS)
		retMsg = tr("You have no free messages to networks other than PlusGSM left.");
	else if(getErrorType() == SPAM_PROTECTION)
		retMsg = tr("Spam protection: SMS was not sent.");
	else if(getErrorType() == UNKNOWN_ERROR)
		retMsg = tr("Unknown error has occured while trying to send an SMS.");
	return retMsg;
}

QString SendThread::getInfosMsg()
{
	kdebugf();
	QString retStr = tr("SMSes to other networks left on an account: ") + othersInfos;
	return retStr;
}

void SendThread::setErrorType(ErrorType err)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "errorType = %d\n", errorType);
	if(err == NO_ERROR)
		errorType = err;
	else if(errorType == NO_ERROR)
	{
		kdebugm(KDEBUG_INFO, "Changing errorType to: %d\n", err);
		errorType = err;
	}
	kdebugf2();
}

bool SendThread::validLogin()
{
	kdebugf();
	QString invalidLoginSubstr = "Nieprawid³owy login lub has³o.";
	QString temp;
	// TODO: Check if first param is converted correctly
	QTextStream s(&body, QIODevice::IO_ReadOnly);
	bool contains = false;
	while(!s.atEnd())
	{
		temp = s.readLine();
		if(temp.contains(invalidLoginSubstr))
			contains = true;
	}
				
	if(contains)
	{
		kdebugm(KDEBUG_INFO, "Login FAILED!\n");
		setErrorType(INVALID_LOGIN);
		setSuccess(false);
		return false;
	}
	kdebugm(KDEBUG_INFO, "Logged in.\n");
	return true;
}

bool SendThread::validSMSSend()
{
	kdebugf();
	QString validSubstr = "java.util.Date";
	QString temp;
	QTextStream s(&bodyOfTcpSocket, QIODevice::IO_ReadOnly);
	bool containsValid = false;
	while(!s.atEnd())
	{
		temp = s.readLine();
		QByteArray tempArr(temp.toAscii());
		kdebugm(KDEBUG_INFO, "temp=%s\n", tempArr.constData());
		if(temp.contains(validSubstr))
		{
			kdebugm(KDEBUG_INFO, "Contains validSubstr:\n%s\n", temp.toAscii().constData());
			containsValid = true;
		}
	}
	
	if(containsValid)
	{
		kdebugm(KDEBUG_INFO, "SMS sent.\n");
		return true;
	}
	else
	{
		setErrorType(UNKNOWN_ERROR);
		kdebugm(KDEBUG_INFO, "Unknown error occured during posting SMS!\n");
		setSuccess(false);
		return false;
	}

	// Tutaj kod nie dochodzi, ale kompilator ostrzega wiec
	return false;
}

