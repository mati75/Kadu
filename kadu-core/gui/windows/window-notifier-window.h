/*
 * %kadu copyright begin%
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

#ifndef WINDOW_NOTIFIER_WINDOW_H
#define WINDOW_NOTIFIER_WINDOW_H

#include <QtGui/QDialog>

class Notification;

class WindowNotifierWindow : public QDialog
{
	Q_OBJECT

	Notification *CurrentNotification;

	void createGui();
	void addButton(QWidget *parent, const QString &caption, const char *slot);

public:
	explicit WindowNotifierWindow(Notification *notification, QWidget *parent = 0);
	virtual ~WindowNotifierWindow();

};

#endif // WINDOW_NOTIFIER_WINDOW_H
