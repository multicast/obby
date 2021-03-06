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

#ifndef _OBBY_ERROR_HPP_
#define _OBBY_ERROR_HPP_

#include <net6/error.hpp>

namespace obby
{

namespace login
{
	typedef net6::login::error error;

	extern const error ERROR_COLOUR_IN_USE;
	extern const error ERROR_WRONG_GLOBAL_PASSWORD;
	extern const error ERROR_WRONG_USER_PASSWORD;
	extern const error ERROR_PROTOCOL_VERSION_MISMATCH;
	extern const error ERROR_NOT_ENCRYPTED;

	extern const error ERROR_MAX;

	std::string errstring(error err);
}

}

#endif // _OBBY_ERROR_HPP_
