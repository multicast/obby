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

#ifndef _OBBY_HOST_DOCUMENT_HPP_
#define _OBBY_HOST_DOCUMENT_HPP_

#include <net6/host.hpp>
#include "local_document.hpp"
#include "server_document.hpp"

namespace obby
{

class host_document_info;

template<typename selector_type>
class basic_host_buffer;

/** host_document used by host_buffer. Usually you do not have to create or
 * delete documents yourself, the buffers manage them.
 */
	
class host_document : public local_document,
                      public server_document
{
public: 
	/** Creates a new host_document.
	 */
	host_document(const host_document_info& info, net6::host& host);
	virtual ~host_document();

	/** Returns the document info for this document.
	 */
	const host_document_info& get_info() const;

	/** Returns the buffer to which the document is assigned.
	 */
	const basic_host_buffer<net6::selector>& get_buffer() const;

	/** Inserts <em>text</em> at <em>pos</em> and synchronises it with
	 * the clients.
	 */
	virtual void insert(position pos, const std::string& text);

	/** Removes text from the given area and synchronises it with the
	 * clients.
	 */
	virtual void erase(position begin, position end);
protected:
};

}

#endif // _OBBY_HOST_DOCUMENT_HPP_
