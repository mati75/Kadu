/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROGRESS_WINDOW_H
#define PROGRESS_WINDOW_H

#include <QtGui/QDialog>

#include "gui/widgets/progress-icon.h"
#include "exports.h"

class ProgressLabel;

class KADUAPI ProgressWindow : public QDialog
{
	Q_OBJECT

	bool CanClose;

	ProgressLabel *Progress;
	QWidget *Container;
	QPushButton *CloseButton;

	void createGui();
	void enableClosing(bool enable);

protected:
	virtual void closeEvent(QCloseEvent *);

	QWidget * container() { return Container; }

	void setState(ProgressIcon::ProgressState state, const QString &text);
	void setText(const QString &text);

public:
	explicit ProgressWindow(QWidget *parent = 0);
	virtual ~ProgressWindow();

};

#endif // PROGRESS_WINDOW_H
