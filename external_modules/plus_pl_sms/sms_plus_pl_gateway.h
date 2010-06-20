#ifndef SMSMIASTOPLUSAGATEWAY_H
#define SMSMIASTOPLUSAGATEWAY_H

#include "../sms/sms.h"
#include <curl/curl.h>
#include "sendthread.h"
#include "QTimer"

class SmsPlusPlGateway : public SmsGateway
{
	Q_OBJECT
	friend class SendThread;
	protected:
		CURL *curl;
		virtual void httpFinished();
		virtual void httpRedirected(QString);
		void emitFinished(bool success);
		SendThread sendThread;
		QTimer timer;

	public:
		SmsPlusPlGateway(QObject* parent, const char *name = 0);
		~SmsPlusPlGateway();
		static SmsGateway* isValidPlusPl(const QString& number, QObject* parent);
		static bool isNumberCorrect(const QString& number);
	signals:
		void displayInfosSignal();
	protected slots:
		void checkIfFinished();
		void displayInfos();
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
};

#endif
