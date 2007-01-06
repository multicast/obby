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

#include <net6/server.hpp>
#include "user.hpp"

obby::user::user(const net6::user& user6, int red, int green, int blue)
 : m_user6(&user6), m_id(user6.get_id() ), m_name(user6.get_name() ),
   m_red(red), m_green(green), m_blue(blue), m_flags(CONNECTED)
{
}

obby::user::user(unsigned int id, const std::string& name, int red, int green,
                 int blue)
 : m_user6(NULL), m_id(id), m_name(name), m_red(red), m_green(green),
   m_blue(blue), m_flags(NONE)
{
}

void obby::user::release_net6()
{
	m_user6 = NULL;
	remove_flags(CONNECTED);
}

void obby::user::assign_net6(const net6::user& user6, int red, int green,
                             int blue)
{
	// User must not be already connected
	if(get_flags() & CONNECTED)
		throw std::logic_error("obby::user::assign_net6");

	// Name must be the same
	if(m_name != user6.get_name() )
		throw std::logic_error("obby::user::assign_net6");

	m_user6 = &user6;
	m_red = red;
	m_green = green;
	m_blue = blue;

	add_flags(CONNECTED);
}

const net6::user& obby::user::get_net6() const
{
	if(m_user6 == NULL)
		// TODO: Own error class?
		throw std::logic_error("obby::user::get_net6");

	return *m_user6;
}

const std::string& obby::user::get_name() const
{
	return m_name;
}

const net6::address& obby::user::get_address() const
{
	if(m_user6 == NULL)
		// TODO: Own error class?
		throw std::logic_error("obby::user::get_address");

	return m_user6->get_connection().get_remote_address();
}

unsigned int obby::user::get_id() const
{
	return m_id;
}

int obby::user::get_red() const
{
	return m_red;
}

int obby::user::get_green() const
{
	return m_green;
}

int obby::user::get_blue() const
{
	return m_blue;
}

const std::string& obby::user::get_token() const
{
	return m_token;
}

const std::string& obby::user::get_password() const
{
	return m_password;
}

obby::user::flags obby::user::get_flags() const
{
	return m_flags;
}

void obby::user::set_colour(int red, int green, int blue)
{
	m_red = red;
	m_green = green;
	m_blue = blue;
}

void obby::user::set_token(const std::string& token)
{
	m_token = token;
}

void obby::user::set_password(const std::string& password)
{
	m_password = password;
}

void obby::user::add_flags(flags new_flags)
{
	m_flags |= new_flags;
}

void obby::user::remove_flags(flags old_flags)
{
	m_flags &= ~old_flags;
}

