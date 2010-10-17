/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "parser/parser.h"
#include "status/description-manager.h"
#include "status/description-model.h"
#include "activate.h"

#include "activate.h"
#include "debug.h"
#include "icons-manager.h"

#include "choose-description.h"

#define CHOOSEDESCRIPTION_MINIMUMSIZE   250,80
#define CHOOSEDESCRIPTION_PREFERREDSIZE 350,80

QMap<StatusContainer *, ChooseDescription *> ChooseDescription::Dialogs;

ChooseDescription *ChooseDescription::showDialog(StatusContainer *statusContainer, const QPoint &position)
{
	ChooseDescription *dialog;
	if (Dialogs.contains(statusContainer))
	{
		dialog = Dialogs[statusContainer];
	}
	else
	{
		dialog = new ChooseDescription(statusContainer, Core::instance()->kaduWindow());
		Dialogs[statusContainer] = dialog;
		if (position.isNull())
			dialog->resize(dialog->sizeHint());
	}
	if (! position.isNull())
	{
		dialog->setPosition(position);
	}
	else
	{
		dialog->setPosition( QPoint(
			( qApp->desktop()->screenGeometry().width()  - dialog->size().width()  ) / 2,
			( qApp->desktop()->screenGeometry().height() - dialog->size().height() ) / 2
			) );
	}
	dialog->show();
	_activateWindow(dialog);

	return dialog;
}

ChooseDescription::ChooseDescription(StatusContainer *statusContainer, QWidget *parent)
	: QDialog(parent), MyStatusContainer(statusContainer)
{
	kdebugf();

	setWindowRole("kadu-choose-description");

	setWindowTitle(tr("Select description"));
	setAttribute(Qt::WA_DeleteOnClose);

	connect(statusContainer, SIGNAL(statusChanged()), this, SLOT(statusChanged()));

	QString currentDescription = MyStatusContainer->status().description();

	Description = new QComboBox(this);
	Description->setMaxVisibleItems(10);
	Description->setModel(DescriptionManager::instance()->model());
	Description->setEditable(true);
	Description->setLineEdit(new QLineEdit(this));
	Description->setInsertPolicy(QComboBox::NoInsert);
	Description->setDuplicatesEnabled(false);

	Description->setEditText(currentDescription);

	MaxDescriptionLength = statusContainer->maxDescriptionLength();

	connect(Description, SIGNAL(activated(int)), this, SLOT(activated(int)));

	OkButton = new QPushButton(tr("&OK"), this);
	OkButton->setIcon(statusContainer->statusIcon());
	OkButton->setDefault(true);
	connect(OkButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	cancelButton->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	QGridLayout *grid = new QGridLayout(this);

	grid->addWidget(Description, 0, 0, 1, -1);
	if (MaxDescriptionLength > 0)
	{
		AvailableChars = new QLabel(this);
		Description->lineEdit()->setMaxLength(MaxDescriptionLength);
		currentDescriptionChanged(Description->currentText());
		connect(Description, SIGNAL(textChanged(const QString &)), this, SLOT(currentDescriptionChanged(const QString &)));
		grid->addWidget(AvailableChars, 1, 0);
	}

	QWidget *spacer = new QWidget(this);
	spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
	grid->addWidget(spacer, 1, 1);

	grid->addWidget(OkButton, 1, 2, Qt::AlignRight);
	grid->addWidget(cancelButton, 1, 3, Qt::AlignRight);

	connect(this, SIGNAL(accepted()), this, SLOT(setDescription()));

	setMinimumSize( QDialog::sizeHint().expandedTo( QSize(CHOOSEDESCRIPTION_MINIMUMSIZE) ) );

	kdebugf2();
}

ChooseDescription::~ChooseDescription()
{
	Dialogs.remove(MyStatusContainer);
}

QSize ChooseDescription::sizeHint() const
{
	return QDialog::sizeHint().expandedTo( QSize(CHOOSEDESCRIPTION_PREFERREDSIZE) );
}

void ChooseDescription::setPosition(const QPoint &position)
{
	QSize sh = sizeHint();
	int width = sh.width();
	int height = sh.height();

	QDesktopWidget *d = QApplication::desktop();

	QPoint p = position;
	if (p.x() + width + 20 >= d->width())
		p.setX(d->width() - width - 20);
	if (p.y() + height + 20 >= d->height())
		p.setY(d->height() - height - 20);
	move(p);
}

void ChooseDescription::setDescription()
{
	QString description = Description->currentText();
	DescriptionManager::instance()->addDescription(description);

// 	if (config_file.readBoolEntry("General", "ParseStatus", false))
// 		description = Parser::parse(description, account, Core::instance()->myself(), true);

	Status status = MyStatusContainer->status();
	status.setDescription(description);
	MyStatusContainer->setStatus(status);
}

void ChooseDescription::activated(int index)
{
	// TODO: fix this workaround
	QString text = Description->model()->data(Description->model()->index(index, 0), Qt::DisplayRole).toString();
	Description->setEditText(text);
}

void ChooseDescription::currentDescriptionChanged(const QString &text)
{
	int length = text.length();
	AvailableChars->setText(' ' + QString::number(MaxDescriptionLength - length));
}

void ChooseDescription::statusChanged()
{
	OkButton->setIcon(MyStatusContainer->statusIcon());
}
