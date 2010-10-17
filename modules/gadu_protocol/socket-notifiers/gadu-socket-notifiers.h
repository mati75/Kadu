/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#if false // #ifndef _MSC_VER
#pragma GCC visibility push(hidden)
#endif

class GaduSocketNotifiers : public QObject
{
	Q_OBJECT

	int Socket;
	bool Started;
	QSocketNotifier *ReadNotifier;
	QSocketNotifier *WriteNotifier;
	QTimer *TimeoutTimer;

	bool Lock;

	void createSocketNotifiers();
	void deleteSocketNotifiers();

private slots:
	void socketTimeout();
	void dataReceived();
	void dataSent();

protected:
	void watchFor(int socket);
	void disable();
	void enable();

	void lock();
	void unlock();

	virtual bool checkRead() = 0;
	virtual bool checkWrite() = 0;
	virtual void socketEvent() = 0;
	virtual int timeout() = 0;
	virtual bool handleSoftTimeout() = 0;
	virtual void connectionTimeout() = 0;

public:
	GaduSocketNotifiers(QObject *parent = 0);
	virtual ~GaduSocketNotifiers();

};

#if false // #ifndef _MSC_VER
#pragma GCC visibility pop
#endif

#endif // GADU_SOCKET_NOTIFIERS_H
