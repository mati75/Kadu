/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SCREEN_SHOT_CONFIGURATION_H
#define SCREEN_SHOT_CONFIGURATION_H

#include <QtCore/QString>

#include "configuration/configuration-aware-object.h"

class ScreenShotConfiguration : private ConfigurationAwareObject
{
	Q_DISABLE_COPY(ScreenShotConfiguration)

	static ScreenShotConfiguration * Instance;

	QString FileFormat;
	bool UseShortJpgExtension;
	int Quality;
	QString ImagePath;
	QString FileNamePrefix;
	bool PasteImageClauseIntoChatWidget;
	bool WarnAboutDirectorySize;
	long DirectorySizeLimit;

	ScreenShotConfiguration();
	virtual ~ScreenShotConfiguration();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	static ScreenShotConfiguration * instance();

	static void createInstance();
	static void destroyInstance();

	const QString & fileFormat() const { return FileFormat; }
	bool useShortJpgExtension() { return UseShortJpgExtension; }
	int quality() { return Quality; }
	const QString & imagePath() const { return ImagePath; }
	const QString & fileNamePrefix() const { return FileNamePrefix; }
	bool pasteImageClauseIntoChatWidget() { return PasteImageClauseIntoChatWidget; }
	bool warnAboutDirectorySize() { return WarnAboutDirectorySize; }
	long directorySizeLimit() { return DirectorySizeLimit; }

	QString screenshotFileNameExtension();

};

#endif // SCREEN_SHOT_CONFIGURATION_H
