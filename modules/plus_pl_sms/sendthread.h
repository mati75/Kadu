#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <curl/curl.h>

#include <QThread>

class SendThread : public QThread
{
	Q_OBJECT
	public:
		SendThread();
		~SendThread();
	protected:
		CURL *curl;
		QString nr;
		QString msg;
		QString signature;
		QString body;
		QString bodyOfTcpSocket;
		QString JSESSCookie;
		QString othersInfos;
		QByteArray caBundlePath;
		int sentSMSesToPlus;
		int sentSMSesToOthers;
		char errorMsg[CURL_ERROR_SIZE];
		bool finished;
		bool success;
		bool displayInfos;
		enum ErrorType
		{
			NO_ERROR,
			CONNECTION_ERROR,
			INVALID_LOGIN,
			INVALID_SMS_SEND,
			NO_FREE_MESSAGES_TO_OTHER_NETS,
			SPAM_PROTECTION,
			UNKNOWN_ERROR
		};
		ErrorType errorType;

		void initCurl();
		void cleanup();
		bool performGet(QString path);
		bool performPost(QString path, QString postData);
		ErrorType getErrorType() { return errorType; }
		bool getSentSMSesInfo();
		QString getJSessCookie(QString &body);
		bool login();
		bool postSMS();
		bool logout();
	public:
		virtual void run();
		QString getMsg() { return msg; }
		QString getNr() { return nr; }
		bool isFinished() { return finished; }
		bool isSuccess() { return success; }
		QString getErrorMsg();
		QString getInfosMsg();
		bool getDisplayInfos() { return displayInfos; }
	public slots:
		void setMsg(const QString &message) { msg = message; }
		void setNr(const QString &number) { nr = number; }
		void setSignature(const QString &newSignature) { signature = newSignature; }
		void setDisplayInfos(bool di = false) { displayInfos = di; }
	protected slots:
		void setFinished(bool f = true) { finished = f; }
		void setSuccess(bool s = true) { success = s; }
		void setErrorType(ErrorType err);
		bool validLogin();
		bool validSMSSend();
	friend size_t getBody(void *buffer, size_t size, size_t nmemb, SendThread *sendThread);
};

size_t getBody(void *buffer, size_t size, size_t nmemb, SendThread *sendThread);

#endif
