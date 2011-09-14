/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QMutexLocker>

#include "debug.h"

#include "sound-player.h"

#include "sound-play-thread.h"

SoundPlayThread::SoundPlayThread() :
		End(false), Play(false), Player(0)
{
}

SoundPlayThread::~SoundPlayThread()
{
}

void SoundPlayThread::start()
{
	kdebugf();

	QMutex mutex;

	while (!End)
	{
		QMutexLocker locker(&mutex);
		NewSoundToPlay.wait(locker.mutex());

		if (End)
			break;

		if (Play)
		{
			if (Player)
			{
				PlayingMutex.lock();
				Player->playSound(Path);
				PlayingMutex.unlock();
			}

			Play = false;
		}
	}

	emit finished();

	deleteLater();

	kdebugf2();
}

void SoundPlayThread::end()
{
	End = true;
	NewSoundToPlay.wakeAll();
}

void SoundPlayThread::play(SoundPlayer *player, const QString &path)
{
	if (!PlayingMutex.tryLock())
		return; // one sound is played, we ignore next one

	if (Player != player)
	{
		if (Player)
			disconnect(Player, SIGNAL(destroyed()), this, SLOT(playerDestroyed()));

		Player = player;

		if (Player)
			connect(Player, SIGNAL(destroyed()), this, SLOT(playerDestroyed()));
	}

	Path = path;

	Play = true;

	PlayingMutex.unlock();
	NewSoundToPlay.wakeAll();
}

void SoundPlayThread::playerDestroyed()
{
	Player = 0;
}
