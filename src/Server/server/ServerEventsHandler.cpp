/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerEventsHandler.cpp                            :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2024/03/22 14:06:09 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include "../connection/request/RequestExceptions.h"
#include "ServerExceptions.h"
#include "Server.h"

/**
 *  @return false means that we a stopping processing of the current request
 * without sending of the response.
 *  It either happens because we already processed all available data, and we
 * should come back later, when more data will be available - or because
 * client closed connection and there is nowhere to send the response.
 */
bool Server::ProcessHeaders(int client_sock, Connection &connection) {
    try {
        connection.request_.Init(client_sock);
        Log("Got client request:\n", std::cout);
        std::cout << connection.request_ << std::endl;
        connection.url_headers_done_ = true;
    } catch (const ZeroRead &e) {
        // socket is closed on the client's side. Remove connection
        CloseConnectionWithLogMessage(client_sock,
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
        CloseConnectionWithLogMessage(client_sock, "IO failed");
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

bool Server::ProcessBody(int client_sock, Connection &connection) {
    try {
        connection.location_ = ProcessRequest(connection.request_,
                                              std::cout, client_sock);
        Log("Request processed", std::cout);
        connection.body_done_ = true;
    } catch (const ZeroRead &) {
        CloseConnectionWithLogMessage(client_sock,
                                      "Client closed connection while we were "
                                      "reading curl file metadata");
        return false;
    } catch (const ZeroReadUpload &) {
        CloseConnectionWithLogMessage(client_sock,
                                      "Client closed connection while we were "
                                      "uploading file");
        return false;
    } catch (const EwouldblockEagain &) {
        return false;
    } catch (const EwouldblockEagainUpload &) {
        return false;
    } catch (const SendContinueFailedException &) {
        Log("Can't send 100 Continue. Seems like client won't receive "
            "our response as well");
        return false;
    } catch (const ReadFromSocketFailedException &) {
        CloseConnectionWithLogMessage(client_sock, "IO failed");
        return false;
    } catch (const IOFailedException &) {
        CloseConnectionWithLogMessage(client_sock, "IO failed");
        return false;
    } catch (const ClientRequest::RequestException &) {
        Log("Request misconfigured");
        connection.location_.SetReturnCode(BAD_REQUEST);
        connection.body_done_ = true;
    }
    return true;
}

void Server::Respond(int client_sock, const Connection &connection) {
    ServerResponse response(config_.GetServerName(), config_.GetPort());

    response.ComposeResponse(connection.location_);
    Log("Prepared response:\n", std::cout);
    std::cout << response << std::endl;
    response.SendResponse(client_sock);
    Log("Response sent\n", std::cout);
    // response sent: reset the connection for potentially more requests on
    // this socket
    connections_[client_sock] = Connection();
}
