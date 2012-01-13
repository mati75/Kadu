/****************************************************************************
*                                                                           *
*   MessagesSplitter plugin for Kadu                                        *
*   Copyright (C) 2011  Piotr Dąbrowski ultr@ultr.pl                        *
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


#ifndef MESSAGESSPLITTER_H
	#define MESSAGESSPLITTER_H


#include <QObject>

#include "gui/widgets/chat-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "configuration/configuration-aware-object.h"
#include "plugins/generic-plugin.h"


class MessagesSplitter : public ConfigurationUiHandler, public ConfigurationAwareObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES( GenericPlugin )
	public:
		virtual int init( bool firstLoad );
		virtual void done();
		MessagesSplitter();
		~MessagesSplitter();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
	protected:
		void configurationUpdated();
	private slots:
		void chatCreated( ChatWidget *chatwidget );
		void chatDestroying( ChatWidget *chatwidget );
		void messageSendRequested( ChatWidget *chatwidget );
	private:
		void createDefaultConfiguration();
		void wait( int milliseconds );
		bool partssendinglock;
};


#endif
