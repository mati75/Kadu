/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qvbox.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qapplication.h>

#include "misc.h"
#include "unregister.h"
#include "debug.h"
#include "config_file.h"

Unregister::Unregister(QDialog *parent, const char *name)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Unregister user"));
	
	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);
	
	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("UnregisterWindowIcon"));
	l_info->setText(tr("This dialog box allows you to unregister your account. Be aware of using this "
				"option. It will permanently delete your UIN and you will not be able to use "
				"it later!"));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	//our QVGroupBox
	QVGroupBox *vgb_uinpass = new QVGroupBox(center);
	vgb_uinpass->setTitle(tr("UIN and password"));
	center->setStretchFactor(vgb_uinpass, 1);
	//end our QGroupBox
	
	// create needed fields
	
	new QLabel(tr("UIN:"), vgb_uinpass);
	uin = new QLineEdit(vgb_uinpass);
	
	new QLabel(tr("Password:"), vgb_uinpass);
	pwd = new QLineEdit(vgb_uinpass);
	pwd->setEchoMode(QLineEdit::Password);
	// end create needed fields
	
	// buttons
	QHBox *bottom = new QHBox(center);
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_unregister = new QPushButton(icons_manager.loadIcon("UnregisterAccountButton"), tr("Unregister"), bottom, "unregister");
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons
	
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_unregister, SIGNAL(clicked()), this, SLOT(doUnregister()));
	connect(gadu, SIGNAL(unregistered(bool)), this, SLOT(unregistered(bool)));
	
 	loadGeometry(this, "General", "UnregisterDialogGeometry", 0, 0, 355, 230);
	kdebugf2();
}

Unregister::~Unregister()
{
	kdebugf();
	saveGeometry(this, "General", "UnregisterDialogGeometry");
	kdebugf2();
}

void Unregister::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void Unregister::doUnregister() {
	kdebugf();

	if (!uin->text().toUInt() || !pwd->text().length()) 
	{
		QMessageBox::warning(this, "Kadu", tr("Please fill out all fields"), tr("OK"), 0, 0, 1);
		return;
	}

	TokenDialog *tokendialog = new TokenDialog();
	if (tokendialog->exec() != QDialog::Accepted) 
	{
		delete tokendialog;
		return;
	}
	
	QString Tokenid, Tokenval;
	tokendialog->getToken(Tokenid, Tokenval);
	delete tokendialog;
	
	QString Password = pwd->text();

	if (gadu->doUnregister(uin->text().toUInt(), Password, Tokenid, Tokenval))
		setEnabled(false);
	kdebugf2();
}

void Unregister::unregistered(bool ok)
{
	kdebugf();
	if (ok)
	{
		QMessageBox::information(this, "Kadu", tr("Unregistation was successful. Now you don't have any GG number :("));
		close();
	}
	else
	{
		QMessageBox::warning(0, tr("Unregister user"),
				tr("An error has occured while unregistration. Please try again later."), tr("OK"), 0, 0, 1);
		setEnabled(true);
	}
	kdebugf2();
}

void Unregister::deleteConfig() {
	kdebugf();

	QFile::remove(ggPath("kadu.conf"));
	config_file.writeEntry("General","UIN",0);

	qApp->mainWidget()->setCaption(tr("No user"));

	kdebugf2();
}

