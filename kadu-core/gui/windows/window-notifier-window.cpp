/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "notify/notification.h"
#include "debug.h"
#include "icons-manager.h"

#include "window-notifier-window.h"

WindowNotifierWindow::WindowNotifierWindow(Notification *notification, QWidget *parent) :
		QDialog(parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
		CurrentNotification(notification)
{
	kdebugf();

	setWindowRole("kadu-window-notifier");

	CurrentNotification->acquire();

	setWindowTitle(CurrentNotification->title());
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

WindowNotifierWindow::~WindowNotifierWindow()
{
	CurrentNotification->release();
}

void WindowNotifierWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setSpacing(10);

	QWidget* labels = new QWidget();
	QHBoxLayout* labelsLayout = new QHBoxLayout(labels);
	labelsLayout->setSpacing(10);

	if (!CurrentNotification->icon().isNull())
	{
		QLabel *iconLabel = new QLabel;
		iconLabel->setPixmap(CurrentNotification->icon().pixmap(64, 64));
		labelsLayout->addWidget(iconLabel);
	}

	QLabel *textLabel = new QLabel;
	textLabel->setText(CurrentNotification->text());

	labelsLayout->addWidget(textLabel);

	layout->addWidget(labels, 0, Qt::AlignCenter);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttons);
	buttonsLayout->setSpacing(20);

	layout->addWidget(buttons, 0, Qt::AlignCenter);

	const QList<Notification::Callback> callbacks = CurrentNotification->getCallbacks();

	if (callbacks.size())
		foreach(const Notification::Callback &i, callbacks)
			addButton(buttons, i.Caption, i.Slot);
	else
		addButton(buttons, tr("OK"), SLOT(callbackAccept()));

	connect(CurrentNotification, SIGNAL(closed(Notification *)), this, SLOT(close()));

	buttons->setMaximumSize(buttons->sizeHint());
}

void WindowNotifierWindow::addButton(QWidget *parent, const QString &caption, const char *slot)
{
	QPushButton *button = new QPushButton();
	parent->layout()->addWidget(button);
	button->setText(caption);
	connect(button, SIGNAL(clicked()), CurrentNotification, slot);
	connect(button, SIGNAL(clicked()), CurrentNotification, SLOT(clearDefaultCallback()));
}
