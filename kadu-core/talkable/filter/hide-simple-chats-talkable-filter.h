/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HIDE_SIMPLE_CHATS_TALKABLE_FILTER_H
#define HIDE_SIMPLE_CHATS_TALKABLE_FILTER_H

#include "talkable/filter/talkable-filter.h"

class HideSimpleChatsTalkableFilter : public TalkableFilter
{
	Q_OBJECT

public:
	explicit HideSimpleChatsTalkableFilter(QObject *parent = 0);
	virtual ~HideSimpleChatsTalkableFilter();

	virtual FilterResult filterChat(const Chat &chat);

};

#endif // HIDE_SIMPLE_CHATS_TALKABLE_FILTER_H
