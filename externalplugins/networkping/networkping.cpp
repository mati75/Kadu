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




#include "configuration/configuration-file.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "debug.h"

#include "networkping.h"




NetworkPing *networkping;




int NetworkPing::init( bool firstLoad )
{
	Q_UNUSED( firstLoad );
	kdebugf();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/plugins/configuration/networkping.ui") );
	kdebugf2();
	return 0;
}


void NetworkPing::done()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/plugins/configuration/networkping.ui") );
	kdebugf2();
}


NetworkPing::NetworkPing()
{
	// default configuration
	createDefaultConfiguration();
	// is online
	isonline = NetworkManager::instance()->isOnline();
	connect( NetworkManager::instance(), SIGNAL(offline()), this, SLOT(networkManagerOffline()) );
	connect( NetworkManager::instance(), SIGNAL(online()) , this, SLOT(networkManagerOnline())  );

	// PING
	pingtimer = new QTimer( this );
	connect( pingtimer, SIGNAL(timeout()), this, SLOT(ping()) );
	// tcp socket
	tcpsocket = new QTcpSocket( this );
	// ping timeout timer
	pingtimeouttimer = new QTimer( this );
	connect( pingtimeouttimer, SIGNAL(timeout()), this, SLOT(processPing()) );

	// CHECK IP
	checkiptimer = new QTimer( this );
	connect( checkiptimer, SIGNAL(timeout()), this, SLOT(checkIP()) );
	// network access manager
	networkaccessmanager = new QNetworkAccessManager( this );
	networkreply = NULL;
	// ping timeout timer
	checkiptimeouttimer = new QTimer( this );
	connect( checkiptimeouttimer, SIGNAL(timeout()), this, SLOT(processCheckIP()) );

	// connect to NetworkManager
	connect( this, SIGNAL(signalOffline()), NetworkManager::instance(), SIGNAL(offline()) );
	connect( this, SIGNAL(signalOnline()) , NetworkManager::instance(), SIGNAL(online())  );
	// update configuration and start timer(s)
	configurationUpdated();
}


NetworkPing::~NetworkPing()
{
}


void NetworkPing::createDefaultConfiguration()
{
	config_file.addVariable( "NetworkPing", "PingInterval"    , NETWORKPING_DEFAULT_PINGINTERVAL     );
	config_file.addVariable( "NetworkPing", "PingHost"        , NETWORKPING_DEFAULT_PINGHOST         );
	config_file.addVariable( "NetworkPing", "PingPort"        , NETWORKPING_DEFAULT_PINGPORT         );
	config_file.addVariable( "NetworkPing", "PingTimeout"     , NETWORKPING_DEFAULT_PINGTIMEOUT      );
	config_file.addVariable( "NetworkPing", "CheckIP"         , NETWORKPING_DEFAULT_CHECKIP          );
	config_file.addVariable( "NetworkPing", "CheckIPInterval" , NETWORKPING_DEFAULT_CHECKIPINTERVAL  );
	config_file.addVariable( "NetworkPing", "CheckIPTimeout"  , NETWORKPING_DEFAULT_CHECKIPTIMEOUT   );
	config_file.addVariable( "NetworkPing", "IPAddressChecker", NETWORKPING_DEFAULT_IPADDRESSCHECKER );
}


void NetworkPing::networkManagerOffline()
{
	isonline = false;
}


void NetworkPing::networkManagerOnline()
{
	isonline = true;
}


void NetworkPing::configurationUpdated()
{
	pingtimer->setInterval(           1000 * config_file.readNumEntry( "NetworkPing", "PingInterval"   , NETWORKPING_DEFAULT_PINGINTERVAL    ) );
	pingtimeouttimer->setInterval(    1000 * config_file.readNumEntry( "NetworkPing", "PingTimeout"    , NETWORKPING_DEFAULT_PINGTIMEOUT     ) );
	checkiptimer->setInterval(        1000 * config_file.readNumEntry( "NetworkPing", "CheckIPInterval", NETWORKPING_DEFAULT_CHECKIPINTERVAL ) );
	checkiptimeouttimer->setInterval( 1000 * config_file.readNumEntry( "NetworkPing", "CheckIPTimeout" , NETWORKPING_DEFAULT_CHECKIPTIMEOUT  ) );
	// timers
	pingtimer->start();
	if( config_file.readBoolEntry( "NetworkPing", "CheckIP" ) )
		checkiptimer->start();
	else
		checkiptimer->stop();
	// ping and check NOW
	ping();
	if( config_file.readBoolEntry( "NetworkPing", "CheckIP" ) )
		checkIP();
}


void NetworkPing::networkOffline()
{
	if( isonline )
		emit signalOffline();
}


void NetworkPing::networkOnline()
{
	if( ! isonline )
		emit signalOnline();
}


void NetworkPing::networkReset()
{
	emit signalOffline();
	emit signalOnline();
}




void NetworkPing::ping()
{
	if( tcpsocket->state() != QAbstractSocket::UnconnectedState )
		return;
	resetPing();
	QString host = config_file.readEntry(    "NetworkPing", "Host", NETWORKPING_DEFAULT_PINGHOST );
	int port     = config_file.readNumEntry( "NetworkPing", "Port", NETWORKPING_DEFAULT_PINGPORT );
	tcpsocket->connectToHost( host, port );
	pingtimeouttimer->start();
}


void NetworkPing::resetPing()
{
	pingtimeouttimer->stop();
	tcpsocket->disconnect();
	tcpsocket->abort();
	connect( tcpsocket, SIGNAL(connected())                        , this, SLOT(processPing()) );
	connect( tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processPing()) );
}


void NetworkPing::processPing()
{
	pingtimeouttimer->stop();
	bool online = false;
	if( dynamic_cast<QTimer*>( sender() ) != NULL )
		online = false;
	else if( tcpsocket->state() == QAbstractSocket::ConnectedState )
		online = true;
	else if( tcpsocket->error() == QAbstractSocket::RemoteHostClosedError )
		online = true;
	else if( tcpsocket->error() == QAbstractSocket::HostNotFoundError )
		online = true;
	resetPing();
	if( online == false )
		networkOffline();
	else
		networkOnline();
}




void NetworkPing::checkIP()
{
	resetCheckIP();
	if( ! config_file.readBoolEntry( "NetworkPing", "CheckIP" ) )
		return;
	if( ! isonline )
		return;
	if( ( networkreply != NULL ) && networkreply->isRunning() )
		return;
	QString url = config_file.readEntry( "NetworkPing", "IPAddressChecker", NETWORKPING_DEFAULT_IPADDRESSCHECKER );
	networkreply = networkaccessmanager->get( QNetworkRequest( QUrl( url ) ) );
	checkiptimeouttimer->start();
}


void NetworkPing::resetCheckIP()
{
	checkiptimeouttimer->stop();
	networkaccessmanager->disconnect();
	if( networkreply != NULL )
	{
		networkreply->abort();
		networkreply->deleteLater();
		networkreply = NULL;
	}
	connect( networkaccessmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processCheckIP()) );
}


void NetworkPing::processCheckIP()
{
	checkiptimeouttimer->stop();
	QString ip;
	if( dynamic_cast<QTimer*>( sender() ) == NULL )
	{
		if( networkreply != NULL )
		{
			if( networkreply->isFinished() )
			{
				if( networkreply->error() == QNetworkReply::NoError )
				{
					QString content = networkreply->readAll();
					QRegExp ipregexp( "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}" );
					if( content.contains(ipregexp) )
						ip = ipregexp.cap();
				}
			}
		}
	}
	resetCheckIP();
	if( ! ip.isEmpty() )
	{
		if( ( ! lastip.isEmpty() ) && ( ip != lastip ) )
			networkReset();
		lastip = ip;
	}
}




Q_EXPORT_PLUGIN2( networkping, NetworkPing )
