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
 * michal at kadu.net
 */

#ifndef _CERTDIALOG_H
#define _CERTDIALOG_H

#include <QDialog>

class QListWidget;
class QSslError;
class QPushButton;
class QListWidgetItem;
class QSslCertificate;

class SslErrorDialog: public QDialog
{
	Q_OBJECT
	public:
		SslErrorDialog(const QList<QSslError>& errors);
	private:
		QListWidget *list;
		QPushButton *showCertyficate;
	private slots:
		void selectionChanged(QListWidgetItem *item);
		void showCertyficateClicked();
};

class CertyficateInfo: public QDialog
{
	Q_OBJECT
	public:
		CertyficateInfo(QSslCertificate& cert);
	private:
		QString byteArrayToString(QByteArray array);
		QPushButton *saveCertyficate;
		QSslCertificate &certificate;
	private slots:
		void saveSertyficate();
};

#endif