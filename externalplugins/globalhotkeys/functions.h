/****************************************************************************
*                                                                           *
*   GlobalHotkeys plugin for Kadu                                           *
*   Copyright (C) 2008-2012  Piotr Dąbrowski ultr@ultr.pl                   *
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


#ifndef FUNCTIONS_H
	#define FUNCTIONS_H


#include <QObject>

#include <conf.h>


class Functions : public QObject
{
	Q_OBJECT;
	public:
		Functions( QObject *parent = 0 );
		~Functions();
		static Functions *instance();
	public slots:
		void functionShowKadusMainWindow(         ConfHotKey *confhotkey );
		void functionHideKadusMainWindow(         ConfHotKey *confhotkey );
		void functionShowHideKadusMainWindow(     ConfHotKey *confhotkey );
		void functionTurnSilentModeOn(            ConfHotKey *confhotkey );
		void functionTurnSilentModeOff(           ConfHotKey *confhotkey );
		void functionToggleSilentMode(            ConfHotKey *confhotkey );
		void functionQuitKadu(                    ConfHotKey *confhotkey );
		void functionOpenIncomingChatWindow(      ConfHotKey *confhotkey );
		void functionOpenAllIncomingChatWindows(  ConfHotKey *confhotkey );
		void functionMinimizeOpenedChatWindows(   ConfHotKey *confhotkey );
		void functionRestoreMinimizedChatWindows( ConfHotKey *confhotkey );
		void functionMinimizeRestoreChatWindows(  ConfHotKey *confhotkey );
		void functionCloseAllChatWindows(         ConfHotKey *confhotkey );
		void functionOpenChatWith(                ConfHotKey *confhotkey );
		void functionChangeStatus(                ConfHotKey *confhotkey );
		void functionChangeDescription(           ConfHotKey *confhotkey );
		void functionAddANewBuddy(                ConfHotKey *confhotkey );
		void functionSearchForBuddy(              ConfHotKey *confhotkey );
		void functionFileTransfersWindow(         ConfHotKey *confhotkey );
		void functionMultilogonWindow(            ConfHotKey *confhotkey );
		void functionConfigurationWindow(         ConfHotKey *confhotkey );
		void functionAccountManagerWindow(        ConfHotKey *confhotkey );
		void functionPluginsWindow(               ConfHotKey *confhotkey );
	private:
		static Functions *INSTANCE;
};


#endif
