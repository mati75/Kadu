#ifndef POP3_H
#define POP3_H

/*
 * autor 
 * Micha³ Podsiadlik
 * michal at gov.one.pl
 */

#include <QObject>
#include <QAbstractSocket>
#include <QString>
#include <QSslSocket>

enum SecureType {
	WithoutEncryption,
	StartTLS,
	Immediately
};

class Pop3Proto: public QObject
{
	Q_OBJECT

	private:
		QSslSocket *mailserver;
		enum State {
			None,
			Handshake,
			Connecting,
			LoginUser,
			LoginPassword,
			Stat,
			Quit
		} state;
		QString Name, Host, User, Password;
		int Port, Lastmails;
		SecureType Encryption;
	public:	
		Pop3Proto(QString name, QString host, int port, QString user, QString password);
		~Pop3Proto();
		void loadCertyficate();
		void setName(QString name) { Name=name; };
		void setHost(QString host) { Host=host; };
		void setPort(int port) { Port=port; };
		void setUser(QString user) { User=user; };
		void setPassword(QString password) { Password=password; };
		void setLastmails(int lastmails) { Lastmails=lastmails; };
		QString getName() { return Name; };
		QString getHost() { return Host; };
		int getPort() { return Port; };
		QString getUser() { return User; };
		QString getPassword() { return Password; };
		int getLastmails() { return Lastmails; };
		void getStats();
		SecureType getEncryption() { return Encryption; };
		void setEncryption(SecureType type) {Encryption=type; }; 
	public slots:
		void connecterror(QAbstractSocket::SocketError error);
		void connected();
		void parsemessage();
		void encrypted();
		void verifyCertificate(const QList<QSslError>& errors);
	signals:
		void done(int last, int total, int size, QString name);
		
};

#endif

