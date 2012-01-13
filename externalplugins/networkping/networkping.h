/****************************************************************************
*                                                                           *
*   NetworkPing plugin for Kadu                                             *
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


#ifndef NETWORKPING_H
	#define NETWORKPING_H


#define  NETWORKPING_DEFAULT_PINGINTERVAL      3  /* seconds */
#define  NETWORKPING_DEFAULT_PINGHOST          "8.8.8.8"
#define  NETWORKPING_DEFAULT_PINGPORT          53
#define  NETWORKPING_DEFAULT_PINGTIMEOUT       3  /* seconds */
#define  NETWORKPING_DEFAULT_CHECKIP           true
#define  NETWORKPING_DEFAULT_CHECKIPINTERVAL   30  /* seconds */
#define  NETWORKPING_DEFAULT_CHECKIPTIMEOUT    10  /* seconds */
#define  NETWORKPING_DEFAULT_IPADDRESSCHECKER  "http://checkip.dyndns.com/"


#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QTimer>

#include "configuration/configuration-aware-object.h"
#include "network/network-manager.h"
#include "plugins/generic-plugin.h"


class NetworkPing : public QObject, public ConfigurationAwareObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES( GenericPlugin )
	public:
		virtual int init( bool firstLoad );
		virtual void done();
		NetworkPing();
		~NetworkPing();
	signals:
		void signalOffline();
		void signalOnline();
	protected:
		void configurationUpdated();
	private slots:
		void networkManagerOnline();
		void networkManagerOffline();
		void ping();
		void processPing();
		void checkIP();
		void processCheckIP();
	private:
		void createDefaultConfiguration();
		void resetPing();
		void resetCheckIP();
		void networkOffline();
		void networkOnline();
		void networkReset();
		bool isonline;
		QTimer *pingtimer;
		QTimer *pingtimeouttimer;
		QTcpSocket *tcpsocket;
		QTimer *checkiptimer;
		QTimer *checkiptimeouttimer;
		QNetworkAccessManager *networkaccessmanager;
		QNetworkReply *networkreply;
		QString lastip;
};


#endif
