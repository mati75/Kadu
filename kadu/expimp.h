#ifndef EXPIMP_H
#define EXPIMP_H

#include <qdialog.h>
#include <qlistview.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#include "libgadu.h"
#include "userlist.h"

/**
	Dialog umożliwiający import listy kontaktów z serwera GG
**/
class UserlistImport : public QDialog {
	Q_OBJECT
	public:
		UserlistImport(QWidget *parent=0, const char *name=0);
		void init(void);
		
	private:
		struct gg_http *gg_http;
		struct gg_event *e;
		int ret;
		QArray<struct userlist> importedusers;
		QPushButton *fetchbtn;
		QSocketNotifier *snr;
		QSocketNotifier *snw;
		UserList importedUserlist;

		void deleteSocketNotifiers();

	protected:
		QListView *results;

	private slots:
		void socketEvent(void);
		void startTransfer(void);
		void updateUserlist(void);
		void dataReceived(void);
		void dataSent(void);
		void fromfile();
	protected:
		void closeEvent(QCloseEvent *e);
};

/**
	Dialog umożliwiający eksport listy kontaktów na serwer GG
**/
class UserlistExport : public QDialog {
	Q_OBJECT
	public:
		UserlistExport(QWidget *parent=0, const char *name=0);
		void init(void);
	private:
		struct gg_http *gg_http;
		struct gg_event *e;
		int ret;
		QPushButton * sendbtn;
		QPushButton * deletebtn;
		QPushButton * tofilebtn;
		QSocketNotifier *snr;
		QSocketNotifier *snw;
		QString saveContacts();
		void deleteSocketNotifiers();

	private slots:
		void socketEvent(void);
		void startTransfer(void);
		void dataReceived(void);
		void dataSent(void);
		void clean(void);
		void ExportToFile(void);
	protected:
		void closeEvent(QCloseEvent * e);
};

#endif
