/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef EMOTICON_WALKER_H
#define EMOTICON_WALKER_H

#include <QtCore/QList>

class QChar;

class Emoticon;
class EmoticonPrefixTree;

class EmoticonWalker
{
	EmoticonPrefixTree *Tree;

	struct EmoticonCandidate
	{
		EmoticonPrefixTree *EmoticonNode;
		int EmoticonLength;
	};

	QList<EmoticonCandidate> Candidates;
	bool PreviousWasLetter;

	bool possibleEmoticonStart(QChar c) const;
	bool possibleEmoticonEnd(QChar c, bool nextIsLetter) const;

	void addEmptyCandidate();
	EmoticonPrefixTree * findCandidateExpansion(int i, QChar c);
	void expandCandidate(int i, EmoticonPrefixTree *expansion);
	void removeCandidate(int i);
	void tryExpandAllCandidates(QChar c);
	Emoticon findLongestCandidate() const;

public:
	explicit EmoticonWalker(EmoticonPrefixTree *tree);
	~EmoticonWalker();

	Emoticon matchEmoticon(QChar c, bool nextIsLetter);

};

#endif // EMOTICON_WALKER_Hs