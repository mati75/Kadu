/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kkadu.im)
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

#include "anonymous_check.h"

#include "anonymous_check-plugin.h"

AnonymousCheckPlugin::~AnonymousCheckPlugin()
{
}

bool AnonymousCheckPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	AnonymousCheckInstance = new AnonCheck::AnonymousCheck(this);

	return true;
}

void AnonymousCheckPlugin::done()
{
}

#include "moc_anonymous_check-plugin.cpp"

Q_EXPORT_PLUGIN2(anonymous_check, AnonymousCheckPlugin)
