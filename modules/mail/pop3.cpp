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

#include <QSslCertificate>

#include "message_box.h"
#include "debug.h"

#include "certdialog.h"
#include "pop3.h"
#include "mail.h"

Pop3Proto::Pop3Proto(QString name, QString host, int port, QString user, QString password): QObject(), Name(name), Host(host), User(user), Password(password), Port(port)
{
	kdebugf();
	mailserver = new QSslSocket();
	connect(mailserver, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(connecterror(QAbstractSocket::SocketError)));
	connect(mailserver, SIGNAL(connected()), SLOT(connected()));
	connect(mailserver, SIGNAL(readyRead()), SLOT(parsemessage()));
	connect(mailserver, SIGNAL(encrypted()), SLOT(encrypted()));
	connect(mailserver, SIGNAL(sslErrors(const QList<QSslError>&)),
		SLOT(verifyCertificate(const QList<QSslError>&)));
	loadCertyficate();
}

void Pop3Proto::loadCertyficate()
{
	QString certFile=ggPath("certs/").append(Host).append(".crt");
	if(QFile::exists(certFile)){
		kdebugm(KDEBUG_INFO, "appending certyficate\n");
		mailserver->addCaCertificates(QSslCertificate::fromPath(certFile));
	}
}


Pop3Proto::~Pop3Proto()
{
	delete mailserver;	
}

void Pop3Proto::connecterror(QAbstractSocket::SocketError error)
{
	mail->connectionError(tr("Cannot connect to mail server : %0 on account %1").arg(mailserver->errorString()).arg(Name));
}

void Pop3Proto::encrypted()
{
	kdebugf();
	mailserver->write(QString("USER %1\r\n").arg(User).toLatin1());
	state = LoginUser;
	mailserver->flush();
}

void Pop3Proto::connected()
{
	kdebugf();
}

void Pop3Proto::verifyCertificate(const QList<QSslError> & errors)
{
	kdebugf();
	SslErrorDialog d(errors);

	if(d.exec()==QDialog::Accepted)
		mailserver->ignoreSslErrors();
}

void Pop3Proto::parsemessage()
{
	if(!mailserver->canReadLine())
		return;

	QString response=mailserver->readLine();

	QString send;
	QStringList arg=QStringList::split(" ", response);

	if (response.find("+OK")<0)
	{
		switch(state)
		{
		    case Handshake:
			case Connecting:
				mail->connectionError(tr("Cannot connect to mail server on account %1").arg(Name));
				break;
			case LoginUser:
				mail->connectionError(tr("Bad login to POP server on %0").arg(Name));
				break;
			case LoginPassword:
				mail->connectionError(tr("Bad password to POP server on %0").arg(Name));
				break;
			case Stat:
				mail->connectionError(tr("Cannot check mail"));
				break;
			default:
				break;
		}
	}
	else 
	{
		switch (state)
		{
		    case Handshake:
				kdebugm(KDEBUG_INFO, "handshaking\n");
				state=Connecting;
				mailserver->startClientEncryption();
				break;
			case Connecting:
				if(Encryption==StartTLS && !mailserver->isEncrypted()){
					mailserver->write("STLS\r\n");
					mailserver->flush();
					state=Handshake;
					break;
				}
				mailserver->write(QString("USER %1\r\n").arg(User).toLatin1());
				state = LoginUser;
				mailserver->flush();
				break;
			case LoginUser:
				mailserver->write(QString("PASS %1\r\n").arg(Password).toLatin1());
				state = LoginPassword;
				mailserver->flush();
				break;
			case LoginPassword:
				mailserver->write("STAT\r\n");
				state = Stat;
				mailserver->flush();
				break;
			case Stat:
				emit done(Lastmails, arg[1].toInt(), arg[2].toInt(), Name);
				Lastmails = arg[1].toInt();
				mailserver->write("QUIT\r\n");
				state = Quit;
				break;
			case Quit:
				kdebugm(KDEBUG_INFO, "disconnected from server\n");
			default:
				mailserver->close();
				break;

		}		
	}
}

void Pop3Proto::getStats()
{
	kdebugm(KDEBUG_INFO, "Connecting to: " + Host + ":%i\n", Port);

	if(Encryption==StartTLS){
		mailserver->setProtocol(QSsl::TlsV1);
		kdebugm(KDEBUG_INFO, "using TLS\n");
	}
	else{
		mailserver->setProtocol(QSsl::SslV3);
		kdebugm(KDEBUG_INFO, "using SSL3\n");
	}

	state=Connecting;

	if(Encryption==Immediately)
		mailserver->connectToHostEncrypted(Host, Port);
	else
		mailserver->connectToHost(Host, Port);
}
