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

#include <QDialog>
#include <QString>
#include <QSslError>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QDateTime>
#include <QDir>

#include "debug.h"
#include "misc.h"
#include "message_box.h"

#include "certdialog.h"
#include "mail.h"

class CertyficateErrorItem: public QListWidgetItem
{
	public:
		CertyficateErrorItem(QListWidget *parent, const QSslError& e){
			setText(e.errorString());
			cert=e.certificate();
		}
		bool haveCertyficate(){
			return !cert.isNull();
		}
		QSslCertificate& certyficate(){
			return cert;
		}
	private:
		QSslCertificate cert;
};

SslErrorDialog::SslErrorDialog(const QList<QSslError> &errors)
{
	setCaption(tr("Certyficate error"));
	resize(200, 100);
	QVBoxLayout *layout=new QVBoxLayout(this);
	QLabel *text=new QLabel(tr("There were following errors during SSL hadnshake:"), this);

	QWidget *hwidget=new QWidget(this);
	QHBoxLayout *hlayout=new QHBoxLayout(hwidget);
	showCertyficate=new QPushButton(tr("Show certyficate"), hwidget);
	showCertyficate->setEnabled(false);
	connect(showCertyficate, SIGNAL(clicked()),
		this, SLOT(showCertyficateClicked()));

	hlayout->addStretch();
	hlayout->addWidget(showCertyficate);

	list=new QListWidget(this);
	connect(list, SIGNAL(itemClicked(QListWidgetItem*)),
		this, SLOT(selectionChanged(QListWidgetItem*)));
	connect(list, SIGNAL(itemActivated(QListWidgetItem*)),
		this, SLOT(selectionChanged(QListWidgetItem*)));
	
	foreach(const QSslError &e, errors)
	{
		list->addItem(new CertyficateErrorItem(list, e));
	}

	QDialogButtonBox *buttons=new QDialogButtonBox(this);
	buttons->setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(text);
	layout->addWidget(list);
	layout->addWidget(hwidget);
	layout->addWidget(new QLabel(tr("Are you sure do you want to continue?"), this));
	layout->addWidget(buttons);
}

void SslErrorDialog::selectionChanged(QListWidgetItem *item)
{
	CertyficateErrorItem *erroritem=dynamic_cast<CertyficateErrorItem*>(item);
	showCertyficate->setEnabled(erroritem->haveCertyficate());
}

void SslErrorDialog::showCertyficateClicked()
{
	CertyficateErrorItem *item=dynamic_cast<CertyficateErrorItem*>(list->currentItem());
	CertyficateInfo d(item->certyficate());
	d.exec();
}

CertyficateInfo::CertyficateInfo(QSslCertificate& cert): certificate(cert)
{
	setCaption(tr("Certyficate info"));

	QString info;
	QString redText("<font color='red'>%1</font>");
	QVBoxLayout *layout=new QVBoxLayout(this);
	QGroupBox *subjectInfo=new QGroupBox(tr("Subject"), this);
	QFormLayout *subjectInfoLayout=new QFormLayout(subjectInfo);
	info=cert.subjectInfo(QSslCertificate::CommonName);
	subjectInfoLayout->addRow(tr("Common name:"), new QLabel(info, subjectInfo));
	info=cert.subjectInfo(QSslCertificate::Organization);
	if(!info.isEmpty())
		subjectInfoLayout->addRow(tr("Organization:"), new QLabel(info, subjectInfo));
	info=cert.subjectInfo(QSslCertificate::OrganizationalUnitName);
	if(!info.isEmpty())
		subjectInfoLayout->addRow(tr("Organizational Unit:"), new QLabel(info, subjectInfo));
	info=byteArrayToString(cert.serialNumber());
	if(!info.isEmpty())
		subjectInfoLayout->addRow(tr("Serial:"), new QLabel(info, subjectInfo));

	QGroupBox *issuerInfo=new QGroupBox(tr("Issuer"), this);
	QFormLayout *issuerInfoLayout=new QFormLayout(issuerInfo);
	info=cert.issuerInfo(QSslCertificate::CommonName);
	issuerInfoLayout->addRow(tr("Common name:"), new QLabel(info, issuerInfo));
	info=cert.issuerInfo(QSslCertificate::Organization);
	if(!info.isEmpty())
		issuerInfoLayout->addRow(tr("Organization:"), new QLabel(info, issuerInfo));
	info=cert.issuerInfo(QSslCertificate::OrganizationalUnitName);
	if(!info.isEmpty())
		issuerInfoLayout->addRow(tr("Organizational Unit:"), new QLabel(info, issuerInfo));
	QGroupBox *validityInfo=new QGroupBox(tr("Validity"), this);
	QFormLayout *validityInfoLayout=new QFormLayout(validityInfo);
	QLabel *validFrom;
	if(cert.effectiveDate()>QDateTime::currentDateTime())
		validFrom=new QLabel(redText.arg(cert.effectiveDate().toString(Qt::SystemLocaleShortDate)), validityInfo);
	else
		validFrom=new QLabel(cert.effectiveDate().toString(Qt::SystemLocaleShortDate));
	validityInfoLayout->addRow(tr("From:"), validFrom);
	QLabel *validTo;
	if(cert.expiryDate()<QDateTime::currentDateTime())
		validTo=new QLabel(redText.arg(cert.expiryDate().toString(Qt::SystemLocaleShortDate)), validityInfo);
	else
		validTo=new QLabel(cert.expiryDate().toString(Qt::SystemLocaleShortDate));
	validityInfoLayout->addRow(tr("To:"), validTo);
	QGroupBox *fingerprintsInfo=new QGroupBox(tr("Fingerprints"), this);
	QFormLayout *fingerprintsInfoLayout=new QFormLayout(fingerprintsInfo);
	fingerprintsInfoLayout->addRow(tr("SHA1:"),
		new QLabel(byteArrayToString(cert.digest(QCryptographicHash::Sha1)), fingerprintsInfo));
	fingerprintsInfoLayout->addRow(tr("MD5:"),
		new QLabel(byteArrayToString(cert.digest(QCryptographicHash::Md5)), fingerprintsInfo));
	QDialogButtonBox *buttons=new QDialogButtonBox(this);
	QPushButton *ok=new QPushButton(tr("&Ok"));
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	saveCertyficate=new QPushButton(tr("&Save"));
	connect(saveCertyficate, SIGNAL(clicked()), this, SLOT(saveSertyficate()));
	buttons->addButton(ok, QDialogButtonBox::AcceptRole);
	buttons->addButton(saveCertyficate, QDialogButtonBox::ApplyRole);

	layout->addWidget(subjectInfo);
	layout->addWidget(issuerInfo);
	layout->addWidget(validityInfo);
	layout->addWidget(fingerprintsInfo);
	layout->addWidget(buttons);
}

QString CertyficateInfo::byteArrayToString(QByteArray array)
{
	QString ret;
	for(int i=0;i<array.size();i++){
		ret.append(QString::number(array.at(i)&0xff, 16).toUpper()+" ");
	}
	return ret;
}

void CertyficateInfo::saveSertyficate()
{
	kdebugm(KDEBUG_INFO, "save certyficate\n");
	if(!MessageBox::ask(tr("Always trust this certyficate?")))
		return;
	QDir().mkdir(ggPath("certs/"));
	QFile f(ggPath("certs/").append(certificate.subjectInfo(QSslCertificate::CommonName)).append(".crt"));
	if(f.open(QIODevice::WriteOnly | QIODevice::Text)){
		f.write(certificate.toPem());
		f.close();
		saveCertyficate->setEnabled(false);
		mail->refreshCertyficates();
	}
	else
		MessageBox::msg(tr("Unable to save sertyficate"));
}