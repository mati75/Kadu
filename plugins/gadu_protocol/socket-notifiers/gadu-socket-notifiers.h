/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_SOCKET_NOTIFIERS_H
#define GADU_SOCKET_NOTIFIERS_H

#include <QtCore/QObject>

class QSocketNotifier;
class QTimer;

class GaduSocketNotifiers : public QObject
{
	Q_OBJECT

	int Socket;
	bool Started;
	QSocketNotifier *ReadNotifier;
	QSocketNotifier *WriteNotifier;
	QTimer *TimeoutTimer;

	void createSocketNotifiers();
	void deleteSocketNotifiers();

private slots:
	void socketTimeout();
	void dataReceived();
	void dataSent();

protected:
	void watchFor(int socket);

	virtual bool checkRead() = 0;
	virtual bool checkWrite() = 0;
	virtual void socketEvent() = 0;
	virtual int timeout() = 0;
	virtual bool handleSoftTimeout() = 0;
	virtual void connectionTimeout() = 0;

public:
	GaduSocketNotifiers(QObject *parent = 0);
	virtual ~GaduSocketNotifiers();

	void disable();
	void enable();

};

#endif // GADU_SOCKET_NOTIFIERS_H
