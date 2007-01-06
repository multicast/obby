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

#ifndef _OBBY_CLIENT_DOCUMENT_INFO_HPP_
#define _OBBY_CLIENT_DOCUMENT_INFO_HPP_

#include <net6/client.hpp>
#include "format_string.hpp"
#include "insert_operation.hpp"
#include "delete_operation.hpp"
#include "jupiter_client.hpp"
#include "local_document_info.hpp"

namespace obby
{

template<typename selector_type>
class basic_client_buffer;

/** Information about a document that is provided without being subscribed to
 * a document.
 */
template<typename selector_type>
class basic_client_document_info
 : virtual public basic_local_document_info<selector_type>
{
public:
	/** Constructor which does not automatically create an underlaying
	 * document.
	 */
	basic_client_document_info(
		const basic_client_buffer<selector_type>& buffer,
		net6::basic_client<selector_type>& net,
		const user* owner, unsigned int id,
	        const std::string& title
	);

	/** Constructor which allows to give initial content and as such creates
	 * an underlaying document assuming the local client just created the
	 * document.
	 */
	basic_client_document_info(
		const basic_client_buffer<selector_type>& buffer,
		net6::basic_client<selector_type>& net,
		const user* owner, unsigned int id,
		const std::string& title, const std::string& content
	);

	/** Constructor which reads the document_info from a network packet
	 * that is received when the document list is initially synchronised.
	 */
	basic_client_document_info(
		const basic_client_buffer<selector_type>& buffer,
		net6::basic_client<selector_type>& net,
		const net6::packet& init_pack
	);

	/** Inserts the given text at the given position into the document.
	 */
	virtual void insert(position pos, const std::string& text);

	/** Erases the given range from the document.
	 */
	virtual void erase(position pos, position len);

	/** Sends a rename request for the document.
	 */
	virtual void rename(const std::string& new_title);

	/** Sends a subscribe request for the local user. If the subscribe
	 * request succeeded, the subscribe_event will be emitted.
	 */
	virtual void subscribe();

	/** Unsubscribes the local user from this document. signal_unsubscribe
	 * will be emitted if the request has been accepted.
	 */
	virtual void unsubscribe();

	/** Called by the buffer if a network event occured that belongs to the
	 * document.
	 */
	virtual void on_net_packet(const document_packet& pack);

	/** Called by the client buffer when user synchronisation begins.
	 * This clears all the correcntly subscribed users.
	 * TODO: Make another function here. Maybe something like
	 * on_net_sync(const net6::packet& pack)
	 */
	virtual void obby_sync_init();

	/** Adds a user to the list of subscribed users. This function is
	 * called by the buffer while synchronising the document list.
	 * TODO: Merge with TODO item above
	 */
	virtual void obby_sync_subscribe(const user& user);

	/** Called by the buffer if the local user created a document. The
	 * document content is assigned immediately in this case because
	 * the clients does not wait for server acknowledgement to show the
	 * new document without delay.
	 * TODO: Make something other with this...
	 */
	virtual void obby_local_init(const std::string& initial_content);

protected:
	/** Subscribes a user to this document.
	 */
	virtual void user_subscribe(const user& user);

	/** Unsubscribes a user from this document.
	 */
	virtual void user_unsubscribe(const user& user);

	/** Executes a packet.
	 */
	bool execute_packet(const document_packet& pack);

	/** Rename command.
	 */
	virtual void on_net_rename(const document_packet& pack);

	/** Record command: Change in the document.
	 */
	virtual void on_net_record(const document_packet& pack);

	/** Synchronisation initialisation command.
	 */
	virtual void on_net_sync_init(const document_packet& pack);

	/** Synchronisation of a line of the document.
	 */
	virtual void on_net_sync_line(const document_packet& pack);

	/** User subscription command.
	 */
	virtual void on_net_subscribe(const document_packet& pack);

	/** User unsubscription.
	 */
	virtual void on_net_unsubscribe(const document_packet& pack);

	/** Callback from jupiter implementation with record of local operation
	 * that has to be sent to the server.
	 */
	virtual void on_jupiter_local(const record& rec, const user* from);

	std::auto_ptr<jupiter_client> m_jupiter;

public:
	/** Returns the buffer to which this document_info belongs.
	 */
	const basic_client_buffer<selector_type>& get_buffer() const;

private:
	/** Returns the underlaying net6 obejct.
	 */
	net6::basic_client<selector_type>& get_net6();

	/** Returns the underlaying net6 obejct.
	 */
	const net6::basic_client<selector_type>& get_net6() const;
};

typedef basic_client_document_info<net6::selector> client_document_info;

template<typename selector_type>
basic_client_document_info<selector_type>::basic_client_document_info(
	const basic_client_buffer<selector_type>& buffer,
	net6::basic_client<selector_type>& net,
	const user* owner, unsigned int id,
	const std::string& title
) : basic_document_info<selector_type>(buffer, net, owner, id, title),
    basic_local_document_info<selector_type>(buffer, net, owner, id, title)
{
	// If we created this document, the constructor with initial content
	// should be called.
	if(owner == &buffer.get_self() )
	{
		throw std::logic_error(
			"obby::basic_client_document_info::"
			"basic_client_document_info"
		);
	}

	// Implictly subscribe owner
	if(owner != NULL)
		user_subscribe(*owner);
}

template<typename selector_type>
basic_client_document_info<selector_type>::basic_client_document_info(
	const basic_client_buffer<selector_type>& buffer,
	net6::basic_client<selector_type>& net,
	const user* owner, unsigned int id,
	const std::string& title, const std::string& content
) : basic_document_info<selector_type>(buffer, net, owner, id, title),
    basic_local_document_info<selector_type>(buffer, net, owner, id, title)
{
	// content is provided, so we should have created this document
	if(owner != &buffer.get_self() )
	{
		throw std::logic_error(
			"obby::basic_client_document_info::"
			"basic_client_document_info"
		);
	}

	// Assign document, initialise content
	basic_document_info<selector_type>::assign_document();
	basic_document_info<selector_type>::
		m_document->insert(0, content, NULL);

	// Subscribe owner
	user_subscribe(*owner);
}

template<typename selector_type>
basic_client_document_info<selector_type>::basic_client_document_info(
	const basic_client_buffer<selector_type>& buffer,
	net6::basic_client<selector_type>& net,
	const net6::packet& init_pack
) : basic_document_info<selector_type>(
	buffer, net,
	init_pack.get_param(0).net6::parameter::as<const user*>(
		buffer.get_user_table()
	), init_pack.get_param(1).net6::parameter::as<unsigned int>(),
	init_pack.get_param(2).net6::parameter::as<std::string>()
   ), basic_local_document_info<selector_type>(
	buffer, net,
	init_pack.get_param(0).net6::parameter::as<const user*>(
		buffer.get_user_table()
	), init_pack.get_param(1).net6::parameter::as<unsigned int>(),
	init_pack.get_param(2).net6::parameter::as<std::string>()
   )
{
	// Load initially subscribed users
	for(unsigned int i = 3; i < init_pack.get_param_count(); ++ i)
	{
		// Get user
		const user* cur_user = init_pack.get_param(i).net6::
			parameter::as<const user*>(buffer.get_user_table());

		// Must not be local user (who just joined the session and now
		// synchronises the document list)
		if(cur_user == &buffer.get_self() )
		{
			throw std::logic_error(
				"obby::basic_client_document_info::"
				"basic_client_document_info"
			);
		}

		// Subscribe it
		user_subscribe(*cur_user);
	}
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	insert(position pos, const std::string& text)
{
	if(m_jupiter.get() == NULL)
	{
		throw std::logic_error(
			"obby::basic_client_document_info::insert"
		);
	}

	insert_operation op(pos, text);
	m_jupiter->local_op(op, &get_buffer().get_self() );
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	erase(position pos, position len)
{
	if(m_jupiter.get() == NULL)
	{
		throw std::logic_error(
			"obby::basic_client_document_info::erase"
		);
	}

	delete_operation op(pos, len);
	m_jupiter->local_op(op, &get_buffer().get_self() );
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	rename(const std::string& new_title)
{
	document_packet pack(*this, "rename");
	pack << new_title;
	get_net6().send(pack);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	subscribe()
{
	// Already subscribed
	if(m_jupiter.get() != NULL)
	{
		throw std::logic_error(
			"obby::basic_client_document_info::subscribe"
		);
	}

	// Send request
	document_packet pack(*this, "subscribe");
	get_net6().send(pack);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	unsubscribe()
{
	// Not subscribed?
	if(m_jupiter.get() == NULL)
	{
		throw std::logic_error(
			"obby::basic_client_document_info::unsubscribe"
		);
	}

	// Send request
	document_packet pack(*this, "unsubscribe");
	get_net6().send(pack);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_packet(const document_packet& pack)
{
	if(!execute_packet(pack) )
	{
		throw net6::bad_value(
			"Unexpected command: " + pack.get_command()
		);
	}
}

template<typename selector_type>
void basic_client_document_info<selector_type>::obby_sync_init()
{
	// Document gets synced now, all subscribed users will be transmitted.
	basic_document_info<selector_type>::m_users.clear();
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	obby_sync_subscribe(const user& user)
{
	// Subscribe user to info
	basic_document_info<selector_type>::user_subscribe(user);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	obby_local_init(const std::string& content)
{
	// Assign local document before subscribing
	basic_document_info<selector_type>::assign_document();
	// Subscribe local user
	user_subscribe(get_buffer().get_self() );
	// Add initial content
	basic_document_info<selector_type>::
		m_document->insert(0, content, NULL);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::user_subscribe(const user& user)
{
	// Call base function
	basic_document_info<selector_type>::user_subscribe(user);
	if(&get_buffer().get_self() == &user)
	{
		// Note that the document must be there at this point because
		// the whole document synchronisation process should have been
		// performed before we subscribed to a document.
		if(basic_document_info<selector_type>::m_document.get() == NULL)
		{
			throw std::logic_error(
				"obby::basic_client_document_info::"
				"user_subscribe"
			);
		}

		// Create jupiter algorithm to merge changes
		m_jupiter.reset(new jupiter_client(
			*basic_document_info<selector_type>::m_document
		) );

		// Connect signal handlers
		m_jupiter->local_event().connect(
			sigc::mem_fun(
				*this,
				&basic_client_document_info::on_jupiter_local
			)
		);
	}
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	user_unsubscribe(const user& user)
{
	// Call base function
	basic_document_info<selector_type>::user_unsubscribe(user);

	if(&get_buffer().get_self() == &user)
	{
		// Release document if the local user unsubscribed
		basic_document_info<selector_type>::release_document();
		// Release jupiter algorithm
		m_jupiter.reset(NULL);
	}
}

template<typename selector_type>
bool basic_client_document_info<selector_type>::
	execute_packet(const document_packet& pack)
{
	// TODO: std::map<> with command to function
	if(pack.get_command() == "rename")
		{ on_net_rename(pack); return true; }

	if(pack.get_command() == "record")
		{ on_net_record(pack); return true; }

	if(pack.get_command() == "sync_init")
		{ on_net_sync_init(pack); return true; }

	if(pack.get_command() == "sync_line")
		{ on_net_sync_line(pack); return true; }

	if(pack.get_command() == "subscribe")
		{ on_net_subscribe(pack); return true; }

	if(pack.get_command() == "unsubscribe")
		{ on_net_unsubscribe(pack); return true; }
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_rename(const document_packet& pack)
{
	// First parameter is the user who changed the title
	const std::string& new_title =
		pack.get_param(1).net6::parameter::as<std::string>();

	// Rename document
	basic_document_info<selector_type>::document_rename(new_title);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_record(const document_packet& pack)
{
	// Not subscribed?
	if(m_jupiter.get() == NULL)
	{
		format_string str(
			"Got record without being subscribed to document "
			"%0%/%1%"
		);

		str << basic_document_info<selector_type>::get_owner_id()
		    << basic_document_info<selector_type>::get_id();

		throw net6::bad_value(str.str() );
	}

	// Get author of record
	const user* author = pack.get_param(0).net6::
		parameter::as<const user*>(get_buffer().get_user_table());

	// Extract record from packet (TODO: virtualness for document_packet,
	// would allow to remove "+ 2" here)
	unsigned int index = 1 + 2;
	record rec(
		pack,
		index,
		basic_document_info<selector_type>::m_buffer.get_user_table()
	);

	// Apply remote operation
	m_jupiter->remote_op(rec, author);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_sync_init(const document_packet& pack)
{
	// Subscribed?
	if(basic_document_info<selector_type>::m_document.get() != NULL)
	{
		format_string str(
			"Got sync_init for subscribed document %0%/%1%"
		);

		str << basic_document_info<selector_type>::get_owner_id()
		    << basic_document_info<selector_type>::get_id();

		throw net6::bad_value(str.str() );
	}

	// Assign empty document
	basic_document_info<selector_type>::assign_document();
	// Clear all lines, the document will be synced line-wise
	// TODO: Get at least rid of this function call
	basic_document_info<selector_type>::m_document->clear_lines();
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_sync_line(const document_packet& pack)
{
	// No document assigned or subscribed?
	if(basic_document_info<selector_type>::m_document.get() == NULL)
	{
		format_string str(
			"Got sync_line without sync_init for document %0%/%1%"
		);

		str << basic_document_info<selector_type>::get_owner_id()
		    << basic_document_info<selector_type>::get_id();

		throw net6::bad_value(str.str() );
	}

	// Add line to document
	unsigned int index = 2;
	basic_document_info<selector_type>::m_document->add_line(
		line(pack, index, get_buffer().get_user_table())
	);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_subscribe(const document_packet& pack)
{
	const user* new_user = pack.get_param(0).net6::
		parameter::as<const user*>(get_buffer().get_user_table() );

	user_subscribe(*new_user);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_net_unsubscribe(const document_packet& pack)
{
	const user* old_user = pack.get_param(0).net6::
		parameter::as<const user*>(get_buffer().get_user_table() );

	user_unsubscribe(*old_user);
}

template<typename selector_type>
void basic_client_document_info<selector_type>::
	on_jupiter_local(const record& rec, const user* from)
{
	// Build packet with record
	document_packet pack(*this, "record");
	rec.append_packet(pack);
	// Send to server
	get_net6().send(pack);
}

template<typename selector_type>
const basic_client_buffer<selector_type>&
basic_client_document_info<selector_type>::get_buffer() const
{
	return dynamic_cast<const basic_client_buffer<selector_type>&>(
		basic_document_info<selector_type>::m_buffer
	);
}

template<typename selector_type>
net6::basic_client<selector_type>&
basic_client_document_info<selector_type>::get_net6()
{
	return dynamic_cast<net6::basic_client<selector_type>&>(
		basic_document_info<selector_type>::m_net
	);
}

template<typename selector_type>
const net6::basic_client<selector_type>&
basic_client_document_info<selector_type>::get_net6() const
{
	return dynamic_cast<const net6::basic_client<selector_type>&>(
		basic_document_info<selector_type>::m_net
	);
}

} // namespace obby

#endif // _OBBY_CLIENT_DOCUMENT_INFO_HPP_
