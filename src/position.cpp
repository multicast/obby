/* libobby - Network text editing library
 * Copyright (C) 2005 0x539 dev group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "position.hpp"

obby::position::position()
{
}

obby::position::position(unsigned int line, unsigned int col)
 : m_line(line), m_col(col)
{
}

obby::position::position(const position& other)
 : m_line(other.m_line), m_col(other.m_col)
{
}

obby::position::~position()
{
}

obby::position& obby::position::operator=(const position& other)
{
	m_line = other.m_line;
	m_col = other.m_col;

	return *this;
}

unsigned int obby::position::get_line() const
{
	return m_line;
}

unsigned int obby::position::get_col() const
{
	return m_col;
}

void obby::position::move_to(unsigned int line, unsigned int col)
{
	m_line = line;
	m_col = col;
}

void obby::position::move_by(int lines, int cols);
{
	// Avoid underflow
	if(static_cast<int>(m_line) - lines < 0)
		m_line = 0;
	else
		m_line += lines;

	if(static_cast<int>(m_col) - cols < 0)
		m_col = 0;
	else
		m_col += cols;
}

