#ifndef MAIL_H
#define MAIL_H

/*
 * autor 
 * Micha³ Podsiadlik
 * michal at gov.one.pl
 *
 * obsluga maildir:
 * Konrad Klimaszewski
 * klimasz at if.pw.edu.pl>
 */

#include <QTimer>
#include <QString>

#include "main_configuration_window.h"
#include "../notify/notify.h"

#include "pop3.h"

class QLineEdit;
class QListWidget;

class MailNotification: public Notification
{
	Q_OBJECT

	public:
		MailNotification();
		void setCallbackEnabled();

	public slots:
		void openEmailClient();
};

class Mail : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	public:	
		Mail();
		virtual ~Mail();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		void refreshCertyficates();
			
	private:
		QTimer *timer;
		QString formatmessage(int last, int total, int size, QString name);
		QLinkedList<Pop3Proto*> accounts;
		void updateList();

		QListWidget *accountsListBox;
		QLineEdit *maildirLineEdit;

		/**
			\fn void import_0_5_0_Configuration()
			Import danych konfiguracyjnych ze starszej wersji Kadu.
		**/
		void import_0_5_0_Configuration();
		void loadOldConfguration();
		void createDefaultConfiguration();

	protected:
		virtual void configurationUpdated();

	public slots:
		void checkmail();
		void onSelectMaildir();
		void maildir();
		void printstat(int last, int total, int size, QString name);
		void onAddButton();
		void onEditButton();
		void onRemoveButton();
		void connectionError(QString msg);
		void openEmailClient();
};

extern Mail* mail;

#endif
