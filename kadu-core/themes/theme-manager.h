/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QtCore/QObject>

#include "themes/theme.h"

class ThemeManager : public QObject
{
	Q_OBJECT

	QList<Theme> Themes;
	int CurrentThemeIndex;

	int getDefaultThemeIndex();

protected:
	virtual bool isValidThemePath(const QString &themePath) = 0;
	virtual QString getThemeName(const QString &themePath) = 0;

public:
	ThemeManager();

	void loadThemes(QStringList pathList);

	const QList<Theme> & themes() const { return Themes; }
	const Theme & currentTheme() const;

signals:
	void themeListUpdated();

};

#endif // THEME_MANAGER_H
