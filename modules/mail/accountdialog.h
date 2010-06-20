#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

/*
 * autor 
 * Micha³ Podsiadlik
 * michal at gov.one.pl
 */

#include <qdialog.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <QResizeEvent>

#include "debug.h"

#include "pop3.h"

class QFormLayout;

class AccountDialog: public QDialog
{
	Q_OBJECT

	public:
		AccountDialog(Pop3Proto* acc, QWidget *parent);

	private:
		QFormLayout *layout;
		QLineEdit *name, *host, *user, *password;
		QSpinBox *port;
		QComboBox *conntype;
		Pop3Proto *Acc;

	private slots:
		void save();
};

#endif

