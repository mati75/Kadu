/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <math.h>

#include <QtCore/QEvent>
#include <QtCore/QScopedArrayPointer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QScrollBar>

#include "emoticons/emoticon-selector-button.h"
#include "emoticons/emoticons-manager.h"

#include "emoticon-selector.h"

EmoticonSelector::EmoticonSelector(const QWidget *activatingWidget, QWidget *parent) :
		QScrollArea(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Popup);
	setFrameStyle(QFrame::NoFrame);

	int selector_count = EmoticonsManager::instance()->selectorCount();
	if (selector_count == 0)
	{
		close();
		return;
	}

	QWidget *mainwidget = new QWidget(this);

	addEmoticonButtons(selector_count, mainwidget);
	setWidget(mainwidget);
	calculatePositionAndSize(activatingWidget, mainwidget);
}

void EmoticonSelector::addEmoticonButtons(int num_emoticons, QWidget *mainwidget)
{
	int selector_width = 460;
	int total_height = 0, cur_width = 0, btn_width = 0;
	QScopedArrayPointer<EmoticonSelectorButton *> btns(new EmoticonSelectorButton *[num_emoticons]);
	QVBoxLayout *layout = new QVBoxLayout(mainwidget);
	QHBoxLayout *row = 0;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	for (int i = 0; i < num_emoticons; ++i)
	{
		btns[i] = new EmoticonSelectorButton(
			EmoticonsManager::instance()->selectorString(i),
			EmoticonsManager::instance()->selectorAnimPath(i),
			EmoticonsManager::instance()->selectorStaticPath(i),
			mainwidget);
		btn_width = btns[i]->sizeHint().width();

		if (cur_width + btn_width >= selector_width)
			cur_width = 0;

		if (cur_width == 0)
			total_height += btns[i]->sizeHint().height() + 1;
		cur_width += btn_width;

		connect(btns[i], SIGNAL(clicked(const QString &)), this, SLOT(iconClicked(const QString &)));
	}

	if (total_height < selector_width - 80)
		selector_width = sqrt((float)selector_width * total_height) * 1.1f;
	else if (total_height > selector_width + 40)
		selector_width += 40;

	cur_width = 0;
	for (int i = 0; i < num_emoticons; ++i)
	{
		btn_width = btns[i]->sizeHint().width();

		if (cur_width + btn_width >= selector_width)
			cur_width = 0;

		if (cur_width == 0)
		{
			row = new QHBoxLayout;
			layout->addLayout(row);
		}

		row->addWidget(btns[i]);
		cur_width += btn_width;
	}
	if (row)
		row->setAlignment(Qt::AlignLeft); // align the last row to left
}

void EmoticonSelector::calculatePositionAndSize(const QWidget *activatingWidget, const QWidget *mainwidget)
{
	QPoint w_pos = activatingWidget->mapToGlobal(QPoint(0,0));
	QSize s_size = QApplication::desktop()->size();
	QSize e_size = mainwidget->sizeHint();

	bool is_on_left;
	bool hscroll_needed = false;
	int x, max_width;
	int width = e_size.width();
	int hscrollbar_height = horizontalScrollBar()->sizeHint().height();
	int vscrollbar_width = verticalScrollBar()->sizeHint().width();

	// if the distance to the left edge of the screen equals or is bigger than the distance to the right edge
	if (w_pos.x() >= s_size.width() - (w_pos.x() + activatingWidget->width()))
	{
		is_on_left = true;
		x = w_pos.x() - e_size.width();
		max_width = w_pos.x();
		if (x < 0)
		{
			width = w_pos.x();
			x = 0;
			hscroll_needed = true;
		}
	}
	else
	{
		is_on_left = false;
		x = w_pos.x() + activatingWidget->width();
		max_width = s_size.width() - x;
		if (x + e_size.width() > s_size.width())
		{
			width = s_size.width() - (w_pos.x() + activatingWidget->width());
			hscroll_needed = true;
		}
	}

	int height = e_size.height() + (hscroll_needed ? hscrollbar_height : 0);
	// center vertically
	int y = w_pos.y() + activatingWidget->height()/2 - height/2;
	// if we exceed the bottom edge of the screen, let's align the widget to it
	if (y + height > s_size.height())
		y = s_size.height() - height;
	// if we exceed the top edge of the screen, let's align the widget to it
	if (y < 0)
		y = 0;
	if (height > s_size.height())
	{
		height = s_size.height();

		// due to vertical scrollbar added, width of the widget has been increased, so we have to reposition it
		int add_width = vscrollbar_width;
		if (width + add_width > max_width)
			add_width = max_width - width;
		width += add_width;
		if (is_on_left)
			x -= add_width;
	}

	setFixedSize(QSize(width, height));
	move(x, y);
}

void EmoticonSelector::iconClicked(const QString &emoticon_string)
{
	emit emoticonSelect(emoticon_string);
	close();
}

bool EmoticonSelector::event(QEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress && !rect().contains(static_cast<QMouseEvent*>(e)->globalPos() - mapToGlobal(QPoint(0, 0))))
	{
		close();
		return true;
	}
	return QScrollArea::event(e);
}

void EmoticonSelector::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		close();
		return;
	}
	QAbstractScrollArea::keyPressEvent(e);
}
