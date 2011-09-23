/****************************************************************************
*                                                                           *
*   NExtInfo module for Kadu                                                *
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


#ifndef NEXTINFO_H
	#define NEXTINFO_H


#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QList>

#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"
#include "gui/actions/action-description.h"
#include "gui/windows/buddy-data-window-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "notify/notify-event.h"

#include "buddynextinfodata.h"

#include "defines.h"


enum RemindTime
{
	RemindTimeNow,
	RemindTimeTomorrow,
	RemindTimeTheDay,
	RemindTimeNextYear
};


class NExtInfo : public ConfigurationUiHandler, ConfigurationAwareObject, BuddyDataWindowAwareObject
{
	Q_OBJECT
	public:
		NExtInfo( bool firstLoad );
		~NExtInfo();
	public:
		static void updateActionBirthday( Action *action );
		static void updateActionNameday( Action *action );
		static void updateActionBirthdayMenu( Action *action );
		static void updateActionNamedayMenu( Action *action);
	public:
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
		void updateActionsBirthday();
		void updateActionsNameday();
		QPair< bool, QPair<int,int> > checkBirthdayNotify( BuddyNExtInfoData *bdata );
		QPair< bool, QPair<int,int> > checkNamedayNotify(  BuddyNExtInfoData *bdata );
		bool checkBirthdayRemind( BuddyNExtInfoData *bdata );
		bool checkNamedayRemind(  BuddyNExtInfoData *bdata );
	public slots:
		void actionBirthdayCreated( Action *action );
		void actionNamedayCreated(  Action *action );
		void actionBirthdayTriggered( QAction *sender, bool checked );
		void actionNamedayTriggered(  QAction *sender, bool checked );
		void actionBirthdayNowTriggered();
		void actionBirthdayTomorrowTriggered();
		void actionBirthdayTheDayTriggered();
		void actionBirthdayNextYearTriggered();
		void actionNamedayNowTriggered();
		void actionNamedayTomorrowTriggered();
		void actionNamedayTheDayTriggered();
		void actionNamedayNextYearTriggered();
		void showHelp();
		void importDataFromExtInfo();
	protected:
		virtual void configurationUpdated();
		virtual void buddyDataWindowCreated( BuddyDataWindow *buddydatawindow );
		virtual void buddyDataWindowDestroyed( BuddyDataWindow *buddydatawindow );
	private slots:
		void notifyBirthdayNameday();
	private:
		void createDefaultConfiguration();
		BuddyNExtInfoData *bData( Buddy buddy );
		void setBirthdayRemind( Buddy buddy, RemindTime time );
		void setNamedayRemind( Buddy buddy, RemindTime time );
		void importOldData( int olddataformatversion );
		QString ordinal( QString code, int n );
		ActionDescription *actionbirthday, *actionnameday;
		QTimer *birthdaynamedaytimer;
		ActionDescription *nextinfoaction;
		NotifyEvent *notifyevent;
		bool notify;                // configuration value
		bool notifyAboutBirthdays;  // configuration value
		bool notifyAboutNamedays;   // configuration value
		int  notifyAdvance;         // configuration value
		int  notifyInterval;        // configuration value
};


extern NExtInfo *nextinfo;


#endif
