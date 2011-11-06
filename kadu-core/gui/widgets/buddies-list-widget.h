/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef BUDDIES_LIST_WIDGET_H
#define BUDDIES_LIST_WIDGET_H

#include <QtGui/QWidget>

#include "exports.h"

class QTreeView;
class QVBoxLayout;

class FilterWidget;

class KADUAPI BuddiesListWidget : public QWidget
{
	Q_OBJECT

public:
	enum FilterPosition
	{
		FilterAtTop,
		FilterAtBottom
	};

private:
	FilterPosition CurrentFilterPosition;

	QVBoxLayout *Layout;
	FilterWidget *NameFilterWidget;
	QTreeView *View;

	void removeView();
	void insertView();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	static bool shouldEventGoToFilter(QKeyEvent *event);

	explicit BuddiesListWidget(FilterPosition filterPosition, QWidget *parent = 0);
	virtual ~BuddiesListWidget();

	FilterWidget * filterWidget() { return NameFilterWidget; }

	void setPosition(FilterPosition filterPosition);
	void setTreeView(QTreeView *treeView);

signals:
	void filterChanged(const QString &filterText);

};

#endif //BUDDIES_LIST_WIDGET_H
