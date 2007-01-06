/* libobby - Network text editing library
 * Copyright (C) 2005 0x539 dev group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "common.hpp"
#include "config.hpp"
#include "gettext.hpp"

extern "C"
{

const char* obby_version()
{
	return PACKAGE_VERSION;
}

const char* obby_codename()
{
	return "firenze";
}

#ifdef WITH_HOWL
/* This is an entry point for which external scripts could check. */
void obby_has_howl()
{
	return;
}
#endif
}

void obby::init_gettext()
{
#ifdef ENABLE_NLS
	// Gettext initialisation
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
#endif
}

