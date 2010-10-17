/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef OPEN_CHAT_WITH_H
#define OPEN_CHAT_WITH_H

#include <QtGui/QWidget>

#include "open-chat-with-runner.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

class BuddiesListView;
class BuddyListModel;
class LineEditWithClearButton;

class KADUAPI OpenChatWith : public QWidget
{
	Q_OBJECT

	static OpenChatWith *Instance;

	explicit OpenChatWith();
	
	BuddiesListView *BuddiesWidget;
	LineEditWithClearButton *ContactID;
	QVBoxLayout *MainLayout;
	OpenChatWithRunner *OpenChatRunner;

	BuddyListModel *ListModel;
	
private slots:
	void inputAccepted();
	void inputChanged(const QString &text);
	void openChat();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
  	static OpenChatWith * instance();
  
	virtual ~OpenChatWith();
	
	void show();
};

#endif // OPEN_CHAT_WITH_H
