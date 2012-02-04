/****************************************************************************
*                                                                           *
*   SentHistory plugin for Kadu                                             *
*   Copyright (C) 2008-2011  Piotr Dąbrowski ultr@ultr.pl                   *
*                                                                           *
*   This program is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation, either version 3 of the License, or       *
*   (at your option) any later version.                                     *
*                                                                           *
*   This program is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*   GNU General Public License for more details.                            *
*                                                                           *
*   You should have received a copy of the GNU General Public License       *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                           *
****************************************************************************/


#ifndef SENTHISTORY_H
	#define SENTHISTORY_H


#include <QList>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/custom-input.h"
#include "plugins/generic-plugin.h"


#define  SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGE              "Ctrl+Up"
#define  SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGE                  "Ctrl+Down"
#define  SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGEFROMALLCHATS  "Ctrl+Alt+Up"
#define  SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGEFROMALLCHATS      "Ctrl+Alt+Down"


class SentHistory : public QObject, public ConfigurationAwareObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES( GenericPlugin )
	public:
		virtual int init( bool firstLoad );
		virtual void done();
		SentHistory();
		~SentHistory();
	protected:
		void configurationUpdated();
	private slots:
		void chatCreated( ChatWidget *chatwidget );
		void chatDestroying( ChatWidget *chatwidget );
		void messageSendRequested( ChatWidget *chatwidget );
		void editKeyPressed( QKeyEvent* e, CustomInput* custominput, bool &handled );
	private:
		void createDefaultConfiguration();
		void inputMessage( ChatWidget* edit );
		static QList< QPair<Chat,QString> > sentmessages;
		QMap<Chat,int> messagen;
		QMap<Chat,bool> thischatonly;
		QMap<Chat,QString> currentmessage;
};


#endif
