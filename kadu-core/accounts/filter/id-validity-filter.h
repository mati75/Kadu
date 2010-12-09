/*
 * %kadu copyright begin%
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

#ifndef ID_REGULAR_EXPRESSION_FILTER
#define ID_REGULAR_EXPRESSION_FILTER

#include "accounts/filter/abstract-account-filter.h"

class IdValidityFilter : public AbstractAccountFilter
{
	Q_OBJECT

	QString Id;

protected:
	virtual bool acceptAccount(Account account);

public:
	explicit IdValidityFilter(QObject *parent = 0);
	virtual ~IdValidityFilter();

	void setId(const QString &id);

};

#endif // ID_REGULAR_EXPRESSION_FILTER
