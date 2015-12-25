/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc/misc.h"

#include "kadu_completion.h"
#include "kadu_completion_plugin.h"

KaduCompletion::KaduCompletionPlugin::~KaduCompletionPlugin()
{
}

bool KaduCompletion::KaduCompletionPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	KaduCompletion::createInstance();

	return true;
}

void KaduCompletion::KaduCompletionPlugin::done()
{
	KaduCompletion::destroyInstance();
}

Q_EXPORT_PLUGIN2(kadu_completion, KaduCompletion::KaduCompletionPlugin)

#include "moc_kadu_completion_plugin.cpp"
