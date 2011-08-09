/*
 * Copyright © 2008 Patryk Cisek, PK Team
 * Copyright © 2010, Patryk Cisek
 *
 * This file is part of anonymous_check.
 *
 *    anonymous_check is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    anonymous_check is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with anonymous_check.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "anonymous_check.h"

#include "debug.h"

AnonCheck::AnonymousCheck *anonymousCheck = 0;

extern "C" int anonymous_check_init() {
	kdebugf();
        anonymousCheck = new AnonCheck::AnonymousCheck();
	kdebugf2();
	return 0;
}

extern "C" void anonymous_check_close() {
	kdebugf();
        if(0 != anonymousCheck) {
            delete anonymousCheck;
            anonymousCheck = 0;
        }
	kdebugf2();
}

