/****************************************************************************
*                                                                           *
*   SentHistory module for Kadu                                             *
*   Copyright (C) 2008-2010  Piotr Dąbrowski ultr@ultr.pl                   *
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
#include <QObject>

#include <chat/chat.h>
#include <gui/widgets/chat-widget.h>
#include <gui/widgets/custom-input.h>
#include "gui/windows/main-configuration-window.h"
#include "configuration/configuration-aware-object.h"


#define  SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGE              "Ctrl+Up"
#define  SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGE                  "Ctrl+Down"
#define  SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGEFROMALLCHATS  "Ctrl+Alt+Up"
#define  SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGEFROMALLCHATS      "Ctrl+Alt+Down"


class SentHistory : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		SentHistory();
		~SentHistory();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
	protected:
		void configurationUpdated();
	private slots:
		void chatCreated( ChatWidget *chatwidget );
		void chatDestroying( ChatWidget *chatwidget );
		void messageSendRequested( ChatWidget *chatwidget );
		void editKeyPressed( QKeyEvent* e, CustomInput* custominput, bool &handled );
		void inputMessage( ChatWidget* edit );
	private:
		void createDefaultConfiguration();
		static QList< QPair<Chat,QString> > sentmessages;
		ChatWidget *lastChatWidget;
		int message_n;
		bool thischatonly;
};


extern SentHistory *senthistory;


#endif
