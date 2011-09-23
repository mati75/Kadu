/****************************************************************************
*                                                                           *
*   GlobalHotkeys module for Kadu                                           *
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


#ifndef GLOBALHOTKEYS_H
	#define GLOBALHOTKEYS_H


#include <QPointer>
#include <QString>
#include <QTimer>

#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/chat-widget.h"
#include "gui/windows/choose-description.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "buddiesmenu.h"
#include "conf.h"
#include "hotkey.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "defines.h"


void EmptyMsgHandler( QtMsgType type, const char *msg );


class GlobalHotkeys : public ConfigurationUiHandler, ConfigurationAwareObject
{
	friend class Functions;
	Q_OBJECT
	public:
		GlobalHotkeys();
		~GlobalHotkeys();
		static GlobalHotkeys *instance();
		static int messageshandled;
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
	signals:
		void mainConfigurationWindowCreatedSignal( MainConfigurationWindow* );
	protected:
		void configurationUpdated();
	private slots:
		void buddiesShortcutsAddNewButtonPressed();
		void buddiesMenusAddNewButtonPressed();
		void checkPendingHotkeys();
	private:
		static GlobalHotkeys *INSTANCE;
		int grabHotKey( HotKey hotkey );
		void processConfBuddiesShortcut( ConfBuddiesShortcut *confbuddiesshortcut );
		void processConfBuddiesMenu( ConfBuddiesMenu *confbuddiesmenu );
		bool processHotKey( HotKey hotkey );
		QPointer<ConfigActionButton> BUDDIESSHORTCUTSADDNEWBUTTON;
		QPointer<ConfigActionButton> BUDDIESMENUSADDNEWBUTTON;
		QTimer *HOTKEYSTIMER;
		Display *DISPLAY;
		QPointer<QWidget> SHOWNGLOBALWIDGET;
		HotKey SHOWNGLOBALWIDGETHOTKEY;
};


extern GlobalHotkeys *globalhotkeys;


#endif
