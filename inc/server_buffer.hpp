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

#ifndef _OBBY_SERVER_BUFFER_HPP_
#define _OBBY_SERVER_BUFFER_HPP_

#include "error.hpp"
#include "sha1.hpp"
#include "rsa.hpp"
#include "buffer.hpp"
#include "server_document_info.hpp"

namespace obby
{

/** Buffer that serves as (dedicated) server. It listens for incoming
 * connections from client_buffers and synchronises their changes.
 */
template<typename selector_type>
class basic_server_buffer : virtual public basic_buffer<selector_type>
{
public: 
	typedef typename basic_buffer<selector_type>::base_document_info
		base_document_info;
	typedef basic_server_document_info<selector_type>
		document_info;

	typedef sigc::signal<void, const net6::user&> signal_connect_type;
	typedef sigc::signal<void, const net6::user&> signal_disconnect_type;

	/** Creates a new server buffer listening on port <em>port</em>.
	 * This constructor will automatically generate a RSA key pair.
	 */
	basic_server_buffer(unsigned int port);

	/** Creates a new server buffer listening on port <em>port</em>.
	 */
	basic_server_buffer(unsigned int port, const RSA::Key& public_key,
	                    const RSA::Key& private_key);

	/** Changes the global password for this session.
	 */
	void set_global_password(const std::string& password);
	
	/** Creates a new document with predefined content.
	 * signal_document_insert will be emitted and may be used to access
	 * the resulting obby::document_info.
	 */
	virtual void document_create(const std::string& title,
	                             const std::string& content = "");

	/** Removes an existing document.
	 */
	virtual void document_remove(base_document_info& doc);

	/** Looks for a document with the given ID which belongs to the user
	 * with the given owner ID. Note that we do not take a real user object
	 * here because the ID is enough and one might not have a user object
	 * to the corresponding ID. So a time-consuming lookup is obsolete.
	 */
	document_info* document_find(unsigned int owner_id,
	                             unsigned int id) const;

	/** Sends a global message to all users.
	 */
	virtual void send_message(const std::string& message);

	/** Signal which will be emitted if a new client has connected.
	 */
	signal_connect_type connect_event() const;

	/** Signal which will be emitted if a connected client has quit.
	 */
	signal_disconnect_type disconnect_event() const;

protected:
	/** Private constuctor used by derived objects. It does not create
	 * a net6::server object to allow derived object creating derived
	 * classes from net6::server.
	 */
	basic_server_buffer();

	/** Private constrctor which may be used by derived buffers to create
	 * their own derivates of net6::server.
	 */
	basic_server_buffer(const RSA::Key& public_key,
	                    const RSA::Key& private_key);

	/** Registers net6 signal handlers. May be used by derived classes
	 * which override the server_buffer constructor.
	 */
	void register_signal_handlers();

        /** Creates a new document info object according to the type of buffer.
	 */
	virtual document_info* new_document_info(const user* owner,
	                                         unsigned int id,
	                                         const std::string& title,
	                                         const std::string& content);

	/** Internal function to create a document with the given owner.
	 */
	void document_create_impl(const obby::user* owner, unsigned int id,
	                          const std::string& title,
	                          const std::string& content);

	/** Internal function send a message to all users that comes from
	 * the user <em>writer</em>.
	 */
	void send_message_impl(const std::string& message,
	                       const obby::user* writer);

	/** Changes the colour of the given user to the new provided
	 * colors and relays the fact to the other users.
	 */
	void user_colour_impl(obby::user& user, int red, int green, int blue);

	/** net6 signal handlers.
	 */
	void on_connect(const net6::user& user6);
	void on_disconnect(const net6::user& user6);
	void on_join(const net6::user& user6);
	void on_part(const net6::user& user6);
	bool on_auth(const net6::user& user6, const net6::packet& pack,
	             net6::login::error& error);
	unsigned int on_login(const net6::user& user6,
	                      const net6::packet& pack);
	void on_extend(const net6::user& user6, net6::packet& pack);
	void on_data(const net6::user& from, const net6::packet& pack);

	/** Executes a network packet.
	 */
	bool execute_packet(const net6::packet& pack, const user& from);

	/** Document commands.
	 */
	virtual void on_net_document_create(const net6::packet& pack,
	                                    const user& from);
	virtual void on_net_document_remove(const net6::packet& pack,
	                                    const user& from);

	/** Messaging commands.
	 */
	virtual void on_net_message(const net6::packet& pack, const user& from);

	/** User commands.
	 */
	virtual void on_net_user_password(const net6::packet& pack,
	                                  const user& from);
	virtual void on_net_user_colour(const net6::packet& pack,
	                                const user& from);

	/** Forwarding commands.
	 */
	virtual void on_net_document(const net6::packet& pack,
	                             const user& from);

	/** This map temporarily caches all tokens issued to the various
	 * clients, they get removed as soon as they are copied into the
	 * corresponding user object.
	 */
	std::map<const net6::user*, std::string> m_tokens;

	/** RSA keys to enable secure authentication.
	 */
	RSA::Key m_public;
	RSA::Key m_private;

	/** Global session password. Only people who know this password are
	 * allowed to join the session.
	 */
	std::string m_global_password;

	signal_connect_type m_signal_connect;
	signal_disconnect_type m_signal_disconnect;
private:
	/** Private function which executes the constructor code, called by
	 * both public constructors.
	 */
	void init_impl(unsigned int port);

	/** This fucntion provides access to the underlaying net6::basic_server
	 * object.
	 */
	net6::basic_server<selector_type>& net6_server();

	/** This fucntion provides access to the underlaying net6::basic_server
	 * object.
	 */
	const net6::basic_server<selector_type>& net6_server() const;
};

typedef basic_server_buffer<net6::selector> server_buffer;

template<typename selector_type>
basic_server_buffer<selector_type>::basic_server_buffer()
 : basic_buffer<selector_type>()
{
	// TODO: Key length as ctor parameter
	std::pair<RSA::Key, RSA::Key> keys = RSA::generate(
		basic_buffer<selector_type>::m_rclass, 256
	);

	m_public = keys.first; m_private = keys.second;
}

template<typename selector_type>
basic_server_buffer<selector_type>::
	basic_server_buffer(const RSA::Key& public_key,
	                    const RSA::Key& private_key)
 : basic_buffer<selector_type>(), m_public(public_key), m_private(private_key)
{
}

template<typename selector_type>
basic_server_buffer<selector_type>::basic_server_buffer(unsigned int port)
 : basic_buffer<selector_type>()
{
	init_impl(port);

	// TODO: Key length as ctor parameter
	std::pair<RSA::Key, RSA::Key> keys = RSA::generate(
		basic_buffer<selector_type>::m_rclass, 256
	);

	m_public = keys.first; m_private = keys.second;
}

template<typename selector_type>
basic_server_buffer<selector_type>::
	basic_server_buffer(unsigned int port, const RSA::Key& public_key,
	                    const RSA::Key& private_key)
 : basic_buffer<selector_type>(), m_public(public_key), m_private(private_key)
{
	init_impl(port);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::init_impl(unsigned int port)
{
	basic_buffer<selector_type>::m_net =
		new net6::basic_server<selector_type>(port);

	register_signal_handlers();
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	set_global_password(const std::string& password)
{
	m_global_password = password;
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	document_create(const std::string& title, const std::string& content)
{
	// TODO: m_doc_counter should not be declared in basic_buffer
	// but client_buffer / server_buffer separately
	unsigned int id = ++ basic_buffer<selector_type>::m_doc_counter;

	// Create the document with the special owner NULL which means that
	// this document was created by the server.
	document_create_impl(NULL, id, title, content);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	document_remove(base_document_info& info)
{
	// Emit unsubscribe signal for all users that were
	// subscribed to this document
	// TODO: Do this in document_delete
	for(typename document_info::user_iterator user_iter = info.user_begin();
	    user_iter != info.user_end();
	    ++ user_iter)
		info.unsubscribe_event().emit(*user_iter);

	// Tell other clients about removal
	net6::packet remove_pack("obby_document_remove");
	remove_pack << info;
	net6_server().send(remove_pack);

	// Delete document
	basic_buffer<selector_type>::document_delete(info);
}

template<typename selector_type>
typename basic_server_buffer<selector_type>::document_info*
basic_server_buffer<selector_type>::
	document_find(unsigned int owner_id, unsigned int id) const
{
	return dynamic_cast<document_info*>(
		basic_buffer<selector_type>::document_find(owner_id, id)
	);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	send_message(const std::string& message)
{
	// Server sent a message
	// TODO: Emit signal in send_message_impl
	basic_buffer<selector_type>::m_signal_server_message.emit(message);

	// send_message_impl relays the message to the clients
	send_message_impl(message, 0);
}

template<typename selector_type>
typename basic_server_buffer<selector_type>::signal_connect_type
basic_server_buffer<selector_type>::connect_event() const
{
	return m_signal_connect;
}

template<typename selector_type>
typename basic_server_buffer<selector_type>::signal_disconnect_type
basic_server_buffer<selector_type>::disconnect_event() const
{
	return m_signal_disconnect;
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	document_create_impl(const user* owner, unsigned int id,
	                     const std::string& title,
                             const std::string& content)
{
	// Create new document
	document_info* info = new_document_info(owner, id, title, content);
	// Add to buffer
	basic_buffer<selector_type>::document_add(*info);
	// Publish the new document to the users. Note that we do not have
	// to send the document's initial content because no user is currently
	// subscribed, and the content is synced with the subscription.
	net6::packet pack("obby_document_create");
	pack << owner << id << title;

	// TODO: send_to_all_except function or something
	for(user_table::iterator iter = basic_buffer<selector_type>::
		m_user_table.begin(user::flags::CONNECTED);
	    iter != basic_buffer<selector_type>::
		m_user_table.end(user::flags::CONNECTED);
	    ++ iter)
	{
		// The owner already knows about the document.
		if(&(*iter) != owner)
			net6_server().send(pack, iter->get_net6() );
	}
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	send_message_impl(const std::string& message, const user* writer)
{
	net6::packet message_pack("obby_message");
	message_pack << writer << message;
	net6_server().send(message_pack);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	user_colour_impl(obby::user& user, int red, int green, int blue)
{
	// TODO: user_colour_impl should check for color conflicts
	user.set_colour(red, green, blue);
	// TODO: user::set_colour should emit this signal
	basic_buffer<selector_type>::m_signal_user_colour.emit(user);

	net6::packet colour_pack("obby_user_colour");
	colour_pack << &user << red << green << blue;
	net6_server().send(colour_pack);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_connect(const net6::user& user6)
{
	// Create random token for this connection
	mpz_class toknum = basic_buffer<selector_type>::m_rclass.get_z_bits(48);
	std::string token = toknum.get_str(36);

	// Add the token for this user into the token map until a obby::user
	// object exists. The token is required for the login process
	// (password authentication and such) when there is no obby::user.
	m_tokens[&user6] = token;

	// Send token and the server's public key with the welcome packet
	net6::packet welcome_pack("obby_welcome");
	welcome_pack << basic_buffer<selector_type>::PROTOCOL_VERSION
	             << token
	             << m_public.get_n().get_str(36)
	             << m_public.get_k().get_str(36);
	net6_server().send(welcome_pack, user6);

	// User connected
	m_signal_connect.emit(user6);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::on_disconnect(const net6::user& user6)
{
	m_signal_disconnect.emit(user6);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::on_join(const net6::user& user6)
{
	// Find user in list
	user* new_user = basic_buffer<selector_type>::
		m_user_table.find(user6);

	// Should not happen
	if(new_user == NULL)
	{
		format_string str("User %0% is not connected");
		str << user6.get_id();
		throw net6::basic_parameter::bad_value(str.str() );
	}

	// Calculate number of required sync packets (one for each
	// non-connected user, one for each document in the list).
	unsigned int sync_n = basic_buffer<selector_type>::
		m_user_table.count(user::flags::CONNECTED, true);
	sync_n += basic_buffer<selector_type>::document_count();

	// Send initial sync packet with this number, the client may then show
	// a progressbar or something.
	net6::packet init_pack("obby_sync_init");
	init_pack << sync_n;
	net6_server().send(init_pack, user6);

	// Synchronise non-connected users.
	for(user_table::iterator iter = basic_buffer<selector_type>::
		m_user_table.begin(user::flags::CONNECTED, true);
	    iter != basic_buffer<selector_type>::
		m_user_table.end(user::flags::CONNECTED, true);
	    ++ iter)
	{
		net6::packet user_pack("obby_sync_usertable_user");
		user_pack << iter->get_id() << iter->get_name()
		          << iter->get_red() << iter->get_green()
		          << iter->get_blue();
		net6_server().send(user_pack, user6);
	}

	// Synchronise the document list
	for(typename basic_buffer<selector_type>::document_iterator iter =
		basic_buffer<selector_type>::document_begin();
	   iter != basic_buffer<selector_type>::document_end();
	   ++ iter)
	{
		// Setup document packet
		net6::packet document_pack("obby_sync_doclist_document");
		document_pack << iter->get_owner() << iter->get_id()
		              << iter->get_title();

		// Add users that are subscribed
		for(typename document_info::user_iterator user_iter =
			iter->user_begin();
		    user_iter != iter->user_end();
		    ++ user_iter)
			document_pack << &(*user_iter);

		net6_server().send(document_pack, user6);
	}

	// Done with synchronising
	net6::packet final_pack("obby_sync_final");
	net6_server().send(final_pack, user6);

	// Forward join message to docuemnts
	// TODO: Let the documents connect to signal_user_join
	for(typename basic_buffer<selector_type>::document_iterator iter =
		basic_buffer<selector_type>::document_begin();
	   iter != basic_buffer<selector_type>::document_end();
	   ++ iter)
		iter->obby_user_join(*new_user);

	// User joined successfully; emit user_join signal
	// TODO: Move to user_table::add_user
	basic_buffer<selector_type>::m_signal_user_join.emit(*new_user);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::on_part(const net6::user& user6)
{
	// Find obby::user object for given net6::user
	user* cur_user = basic_buffer<selector_type>::
		m_user_table.find_user(user6);

	// Should not happen
	if(cur_user == NULL)
	{
		format_string str("User %0% is not connected");
		str << user6.get_id();
		throw net6::basic_parameter::bad_value(str.str() );
	}

	// Forward part message to documents
	// TODO: Let the documents connect to signal_user_part
	for(typename basic_buffer<selector_type>::document_iterator iter =
		basic_buffer<selector_type>::document_begin();
	   iter != basic_buffer<selector_type>::document_end();
	   ++ iter)
		iter->obby_user_part(*cur_user);

	// Emit part signal, remove user from list
	// TODO: Move part signal emission to remove_user
	basic_buffer<selector_type>::m_signal_user_part.emit(*cur_user);
	basic_buffer<selector_type>::m_user_table.remove_user(cur_user);
}

template<typename selector_type>
bool basic_server_buffer<selector_type>::
	on_auth(const net6::user& user6, const net6::packet& pack,
	        net6::login::error& error)
{
	const std::string& name =
		pack.get_param(0).net6::basic_parameter::as<std::string>();
	unsigned int red =
		pack.get_param(1).net6::basic_parameter::as<int>();
	unsigned int green =
		pack.get_param(2).net6::basic_parameter::as<int>();
	unsigned int blue =
		pack.get_param(3).net6::basic_parameter::as<int>();

	// Get global and user password
	const std::string& global_password =
		pack.get_param(4).net6::basic_parameter::as<std::string>();
	const std::string& user_password =
		pack.get_param(5).net6::basic_parameter::as<std::string>();

	// Check colour
	if(!basic_buffer<selector_type>::check_colour(red, green, blue) )
	{
		error = login::ERROR_COLOR_IN_USE;
		return false;
	}

	// Get token for this client
	std::map<const net6::user*, std::string>::iterator token_iter =
		m_tokens.find(&user6);

	// Should not happen
	if(token_iter == m_tokens.end() )
	{
		format_string str("No token available for user %0%");
		str << user6.get_id();
		throw net6::basic_parameter::bad_value(str.str() );
	}

	// Get token from iterator
	const std::string& token = token_iter->second;

	// Check global password
	if(!m_global_password.empty() )
	{
		if(global_password != SHA1::hash(token + m_global_password) )
		{
			error = login::ERROR_WRONG_GLOBAL_PASSWORD;
			return false;
		}
	}

	// Search user in user table
	const user_table& table = basic_buffer<selector_type>::m_user_table;
	obby::user* user = table.find(name, user::flags::CONNECTED, true);

	// Compare user password
	if(user && !user->get_password().empty() )
	{
		if(user_password != SHA1::hash(token + user->get_password()) )
		{
			error = login::ERROR_WRONG_USER_PASSWORD;
			return false;
		}
	}

	// Auth OK
	return true;
}

template<typename selector_type>
unsigned int basic_server_buffer<selector_type>::
	on_login(const net6::user& user6, const net6::packet& pack)
{
	// Get color
	unsigned int red =
		pack.get_param(1).net6::basic_parameter::as<int>();
	unsigned int green =
		pack.get_param(2).net6::basic_parameter::as<int>();
	unsigned int blue =
		pack.get_param(3).net6::basic_parameter::as<int>();

	// Insert into user list
	user* new_user = basic_buffer<selector_type>::m_user_table.add_user(
		user6, red, green, blue
	);

	// Remove token from temporary token list because token can now be
	// stored in the user object.
	std::map<const net6::user*, std::string>::iterator token_iter =
		m_tokens.find(&user6);

	// Should not happen
	if(token_iter == m_tokens.end() )
	{
		format_string str("No token available for user %0%");
		str << user6.get_id();
		throw net6::basic_parameter::bad_value(str.str() );
	}

	// Store token in user, remove from list
	// TODO: Perform this by a call to user table, use only
	// const user.
	new_user->set_token(token_iter->second);
	m_tokens.erase(token_iter);

	// Tell net6 to use already existing ID, if any
	return new_user->get_id();
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_extend(const net6::user& user6, net6::packet& pack)
{
	// Find corresponding user in list
	user* cur_user = basic_buffer<selector_type>::
		m_user_table.find(user6);

	// Should not happen
	if(cur_user == NULL)
	{
		format_string str("User %0% is not connected");
		str << user6.get_id();
		throw net6::basic_parameter::bad_value(str.str() );
	}

	// Extend user-join packet with colour
	pack << cur_user->get_red() << cur_user->get_green()
	     << cur_user->get_blue();
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_data(const net6::user& user6, const net6::packet& pack)
{
	// Get obby::user from net6::user
	user* from_user = basic_buffer<selector_type>::
		m_user_table.find(user6);

	// Should not happen
	if(from_user == NULL)
	{
		format_string str("User %0% is not connected");
		str << user6.get_id();
		throw net6::basic_parameter::bad_value(str.str() );
	}

	// Execute packet
	if(!execute_packet(pack, *from_user) )
	{
		throw net6::basic_parameter::bad_value(
			"Unexpected command: " + pack.get_command()
		);
	}
}

template<typename selector_type>
bool basic_server_buffer<selector_type>::
	execute_packet(const net6::packet& pack, const user& from)
{
	// TODO: std::map<> mapping command to function
	if(pack.get_command() == "obby_document_create")
		{ on_net_document_create(pack, from); return true; }

	if(pack.get_command() == "obby_document_remove")
		{ on_net_document_remove(pack, from); return true; }

	if(pack.get_command() == "obby_message")
		{ on_net_message(pack, from); return true; }

	if(pack.get_command() == "obby_user_password")
		{ on_net_user_password(pack, from); return true; }

	if(pack.get_command() == "obby_user_colour")
		{ on_net_user_colour(pack, from); return true; }

	if(pack.get_command() == "obby_document")
		{ on_net_document(pack, from); return true; }
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_net_document_create(const net6::packet& pack, const user& from)
{
	unsigned int id =
		pack.get_param(0).net6::basic_parameter::as<int>();
	const std::string& title =
		pack.get_param(1).net6::basic_parameter::as<std::string>();
	const std::string& content =
		pack.get_param(2).net6::basic_parameter::as<std::string>();

	document_create_impl(&from, id, title, content);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_net_document_remove(const net6::packet& pack, const user& from)
{
	// Get document to remove
	document_info* doc =
		pack.get_param(0).net6::basic_parameter::as<document_info*>();

	// TODO: AUTH

	// Remove it
	document_remove(*doc);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_net_message(const net6::packet& pack, const user& from)
{
	const std::string& message =
		pack.get_param(0).net6::basic_parameter::as<std::string>();

	// TODO: Move signal emission to send_message_impl
	basic_buffer<selector_type>::m_signal_message.emit(from, message);
	send_message_impl(message, &from);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_net_user_password(const net6::packet& pack, const user& from)
{
	// Set password for this user
	// TODO: Do this by a call to user_table
	const_cast<user&>(from).set_password(
		RSA::decrypt(
			m_private,
			pack.get_param(0).net6::basic_parameter::as<
				std::string
			>()
		)
	);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_net_user_colour(const net6::packet& pack, const user& from)
{
	unsigned int red =
		pack.get_param(0).net6::basic_parameter::as<int>();
	unsigned int green =
		pack.get_param(1).net6::basic_parameter::as<int>();
	unsigned int blue =
		pack.get_param(2).net6::basic_parameter::as<int>();

	// Check new user colour for conflicts
	if(!basic_buffer<selector_type>::check_colour(red, green, blue, &from) )
	{
		net6::packet reply_pack("obby_user_colour_failed");
		net6_server().send(reply_pack, from.get_net6() );
	}
	else
	{
		// TODO: user_colour_impl should take const user&, user_table
		// performs the necessary operation
		user_colour_impl(const_cast<user&>(from), red, green, blue);
	}
}

template<typename selector_type>
void basic_server_buffer<selector_type>::
	on_net_document(const net6::packet& pack, const user& from)
{
	document_info& info = dynamic_cast<document_info&>(
		*pack.get_param(0).net6::basic_parameter::as<
			obby::document_info*
		>()
	);

	// TODO: Rename this function. Think about providing a signal that may
	// be emitted.
	info.on_net_packet(document_packet(pack), from);
}

template<typename selector_type>
void basic_server_buffer<selector_type>::register_signal_handlers()
{
	net6_server().connect_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_connect) );
	net6_server().disconnect_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_disconnect) );
	net6_server().join_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_join) );
	net6_server().part_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_part) );
	net6_server().login_auth_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_auth) );
	net6_server().login_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_login) );
	net6_server().login_extend_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_extend) );
	net6_server().data_event().connect(
		sigc::mem_fun(*this, &basic_server_buffer::on_data) );
}

template<typename selector_type>
typename basic_server_buffer<selector_type>::document_info*
basic_server_buffer<selector_type>::
	new_document_info(const user* owner, unsigned int id,
                          const std::string& title, const std::string& content)
{
	// Create server_document_info, according to server_buffer
	return new document_info(
		*this, net6_server(), owner, id, title, content
	);
}

template<typename selector_type>
net6::basic_server<selector_type>& basic_server_buffer<selector_type>::
	net6_server()
{
	return dynamic_cast<net6::basic_server<selector_type>&>(
		*basic_buffer<selector_type>::m_net
	);
}

template<typename selector_type>
const net6::basic_server<selector_type>& basic_server_buffer<selector_type>::
	net6_server() const
{
	return dynamic_cast<net6::basic_server<selector_type>&>(
		*basic_buffer<selector_type>::m_net
	);
}

} // namespace obby

#endif // _OBBY_SERVER_BUFFER_HPP_
