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

#include "local_buffer.hpp"

obby::local_buffer::local_buffer()
 : buffer()
{
}

obby::local_buffer::~local_buffer()
{
}

const std::string& obby::local_buffer::get_name() const
{
	return get_self().get_name();
}

obby::local_document_info*
obby::local_buffer::find_document(unsigned int owner_id, unsigned int id) const
{
	return dynamic_cast<obby::local_document_info*>(
		buffer::find_document(owner_id, id)
	);
}

obby::local_buffer::signal_user_colour_failed_type
obby::local_buffer::user_colour_failed_event() const
{
	return m_signal_user_colour_failed;
}

