/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef PREVIOUS_DESCRIPTIONS_WINDOW_H
#define PREVIOUS_DESCRIPTIONS_WINDOW_H

#include <QtGui/QDialog>

class QListView;
class QModelIndex;

class PreviousDescriptionsWindow : public QDialog
{
	Q_OBJECT

	QListView *DescriptionsList;

private slots:
	void chooseButtonClicked();
	void listItemDoubleClicked(const QModelIndex &index);

public:
	explicit PreviousDescriptionsWindow(QWidget *parent = 0);
	virtual ~PreviousDescriptionsWindow();

signals:
	void descriptionSelected(const QString &description);

};

#endif // PREVIOUS_DESCRIPTIONS_WINDOW_H