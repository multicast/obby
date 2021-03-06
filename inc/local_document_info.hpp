/* libobby - Network text editing library
 * Copyright (C) 2005, 2006 0x539 dev group
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

#ifndef _OBBY_LOCAL_DOCUMENT_INFO_HPP_
#define _OBBY_LOCAL_DOCUMENT_INFO_HPP_

#include <net6/local.hpp>
#include "serialise/object.hpp"
#include "document_info.hpp"

namespace obby
{

template<typename Document, typename Selector>
class basic_local_buffer;

/** Information about a document that is provided without being subscribed to
 * a document.
 */

template<typename Document, typename Selector>
class basic_local_document_info:
	virtual public basic_document_info<Document, Selector>
{
public:
	typedef basic_document_info<Document, Selector> base_type;
	typedef typename base_type::document_type document_type;

	typedef basic_local_buffer<Document, Selector> buffer_type;
	typedef typename buffer_type::net_type net_type;

	enum subscription_state {
		UNSUBSCRIBED,
		UNSUBSCRIBING,
		SUBSCRIBED,
		SUBSCRIBING
	};

	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const user* owner,
	                          unsigned int id,
	                          const std::string& title,
	                          unsigned int suffix,
	                          const std::string& encoding);

	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const user* owner,
	                          unsigned int id,
	                          const std::string& title,
	                          const std::string& encoding);

	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const serialise::object& obj);

	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const net6::packet& init_pack);

	/** Sends a subscribe request for the local user. If the subscribe
	 * request succeeded, the subscribe_event will be emitted.
	 */
	virtual void subscribe() = 0;

	/** Unsubscribes the local user from this document. signal_unsubscribe
	 * will be emitted if the request has been accepted.
	 */
	virtual void unsubscribe() = 0;

	/** Returns whether the local user is subscribed to this document.
	 */
	virtual bool is_subscribed() const;

	/** Returns whether the given user <em>user</em> is subscribed to
	 * this document.
	 */
	virtual bool is_subscribed(const user& user) const;

	/** @brief Returns the state of the local user's subscription to
	 * this document.
	 */
	virtual subscription_state get_subscription_state() const = 0;

	/** @brief Called when the session has been closed.
	 */
	virtual void obby_session_close();

protected:
	/** Implementation of the session close callback that does not call
	 * the base function.
	 */
	void session_close_impl();

public:
	/** Returns the buffer this document belongs to.
	 */
	const buffer_type& get_buffer() const;

protected:
	/** Returns the underlaying net6 object.
	 */
	net_type& get_net6();

	/** Returns the underlaying net6 object.
	 */
	const net_type& get_net6() const;
};

template<typename Document, typename Selector>
basic_local_document_info<Document, Selector>::
	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const user* owner,
	                          unsigned int id,
	                          const std::string& title,
	                          unsigned int suffix,
	                          const std::string& encoding):
	base_type(buffer, net, owner, id, title, suffix, encoding)
{
}

template<typename Document, typename Selector>
basic_local_document_info<Document, Selector>::
	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const user* owner,
	                          unsigned int id,
	                          const std::string& title,
	                          const std::string& encoding):
	base_type(buffer, net, owner, id, title, encoding)
{
}

template<typename Document, typename Selector>
basic_local_document_info<Document, Selector>::
	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const serialise::object& obj):
	base_type(buffer, net, obj)
{
}

template<typename Document, typename Selector>
basic_local_document_info<Document, Selector>::
	basic_local_document_info(const buffer_type& buffer,
	                          net_type& net,
	                          const net6::packet& init_pack):
	base_type(buffer, net, init_pack)
{
}

template<typename Document, typename Selector>
bool basic_local_document_info<Document, Selector>::is_subscribed() const
{
	return base_type::is_subscribed(get_buffer().get_self() );
}

template<typename Document, typename Selector>
bool basic_local_document_info<Document, Selector>::
	is_subscribed(const user& user) const
{
	return base_type::is_subscribed(user);
}

template<typename Document, typename Selector>
void basic_local_document_info<Document, Selector>::obby_session_close()
{
	session_close_impl();
	basic_document_info<Document, Selector>::session_close_impl();
}

template<typename Document, typename Selector>
void basic_local_document_info<Document, Selector>::session_close_impl()
{
}

template<typename Document, typename Selector>
const typename basic_local_document_info<Document, Selector>::buffer_type&
basic_local_document_info<Document, Selector>::get_buffer() const
{
	return dynamic_cast<const buffer_type&>(base_type::get_buffer() );
}

template<typename Document, typename Selector>
typename basic_local_document_info<Document, Selector>::net_type&
basic_local_document_info<Document, Selector>::get_net6()
{
	return dynamic_cast<net_type&>(base_type::get_net6() );
}

template<typename Document, typename Selector>
const typename basic_local_document_info<Document, Selector>::net_type&
basic_local_document_info<Document, Selector>::get_net6() const
{
	return dynamic_cast<const net_type&>(base_type::get_net6() );
}

} // namespace obby

#endif // _OBBY_LOCAL_DOCUMENT_INFO_HPP_
