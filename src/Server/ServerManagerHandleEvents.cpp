/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerHandleEvents.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mede-mas <mede-mas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 14:52:05 by  rokupin          #+#    #+#             */
/*   Updated: 2024/05/02 15:53:46 by mede-mas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.h"
#include "connection/request/RequestExceptions.h"
#include "server/ServerExceptions.h"

/**
 *  Server retrieves the reference to the current connection.
 *  It essentially serves to save the state of the connection, due to the
 * possibility of 1 request being sent in multiple events it allows us not to
 * lose data if on the stage of finishing of the data request being processed
 * is still incomplete and therefore response weren't yet sent.
 */
void ServerManager::HandleEventsOnExistingConnection(int client_socket) {
	Connection		&connection = connections_[client_socket];

	while (is_running_) {
		if (!connection.url_headers_done_) {
			if (!ProcessHeaders(connection))
				return;
		}
		if (connection.url_headers_done_ && !connection.body_done_) {
			if (!ProcessBody(connection))
				return;
		}
		if (connection.body_done_) {
			// Check if the request is targeting a CGI script
			std::string extension = GetExtensionFromURL(connection.getUrl);
			auto it = config_cgi_handlers.find(extension);
			if (it != config_.cgi_handlers.end()) {
				// The request is for a CGI script
				ExecuteCGIScript(connection, it->second);
			} else {
				// Normal non-CGI request processing
				if (!Respond(connection)) {
				CloseConnectionWithLogMessage(client_socket, "Client request triggered error");
				}
			}
			
		}
	}
}

/**
 *  @return false means that we a stopping processing of the current request
 * without sending of the response.
 *  It either happens because we already processed all available data, and we
 * should come back later, when more data will be available - or because
 * client closed connection and there is nowhere to send the response.
 */
bool ServerManager::ProcessHeaders(Connection &connection) {
	try {
		connection.request_.Init(connection.connection_socket_);
		connection.address_ = connection.request_.GetHeaderValue("Host");
		Log("Got client request:");
		std::cout << connection.request_ << std::endl;
		connection.url_headers_done_ = true;
	} catch (const Stopped &e) {
		return true;
	} catch (const ZeroRead &e) {
		// socket is closed on the client's side. Remove connection
		CloseConnectionWithLogMessage(connection.connection_socket_,
									  "Done with all available request data,"
									  "response already sent");
		return false;
	} catch (const EwouldblockEagain &e) {
		// socket is still active, but no data is available.
		Log("Red all available data, but request is incomplete. "
			"We'll come back later. Maybe.");
		return false;
	} catch (const ReadFromSocketFailedException &) {
		// Probably, client wouldn't even be able to read our response
		// so just shut down this connection
		CloseConnectionWithLogMessage(connection.connection_socket_, "IO failed");
		return false;
	} catch (const ClientRequest::RequestException &) {
		// notify client about it
		Log("Request misconfigured");
		connection.location_.SetReturnCode(BAD_REQUEST);
		connection.url_headers_done_ = true;
		connection.body_done_ = true;
	}
	return true;
}

bool ServerManager::ProcessBody(Connection &connection) {
	try {
		const Server    &server = FindServer(connection);
		connection.location_ = server.ProcessRequest(connection);
		Log("Request processed");
		connection.body_done_ = true;
	} catch (const ZeroRead &) {
		CloseConnectionWithLogMessage(connection.connection_socket_,
									  "Client closed connection while we were "
									  "reading curl file metadata");
		return false;
	} catch (const ZeroReadUpload &) {
		CloseConnectionWithLogMessage(connection.connection_socket_,
									  "Client closed connection while we were "
									  "uploading file");
		return false;
	} catch (const EwouldblockEagain &) {
		// socket is still active, but no data is available.
		Log("Red all available data, but request is incomplete. "
			"We'll come back later. Maybe.");
		return false;
	} catch (const EwouldblockEagainUpload &) {
		Log("Red all available data, but request is incomplete. "
			"We'll come back later. Maybe.");
		return false;
	} catch (const SendContinueFailedException &) {
		Log("Can't send 100 Continue. Seems like client won't receive "
			"our response as well");
		return false;
	} catch (const ReadFromSocketFailedException &) {
		CloseConnectionWithLogMessage(connection.connection_socket_, "IO failed");
		return false;
	} catch (const IOFailedException &) {
		CloseConnectionWithLogMessage(connection.connection_socket_, "IO failed");
		return false;
	} catch (const ClientRequest::RequestException &) {
		Log("Request misconfigured");
		connection.location_.SetReturnCode(BAD_REQUEST);
		connection.body_done_ = true;
	}
	return true;
}

bool ServerManager::Respond(Connection &connection) {
	ServerResponse response(connection);

	Log("Prepared response:");
	std::cout << response << std::endl;
	response.SendResponse(connection.connection_socket_);
	Log("Response sent");
	// response sent: reset the connection for potentially more requests on
	// this socket
	connections_[connection.connection_socket_] =
			Connection(is_running_, connection.connection_socket_,
					   connection.server_listening_socket_);
	return !Utils::Get().IsErrorCode(response.GetCode());
}
