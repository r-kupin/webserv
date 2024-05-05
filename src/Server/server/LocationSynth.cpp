/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    LocationSynth.cpp                                  :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/12/21 18:31:13 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <algorithm>
#include <csignal>
#include <sys/wait.h>
#include "Server.h"

/**
 * Depending on compliance between what was requested and what is being
 * found, server creates a synthetic location - a copy of the location that
 * was found, but with altered return code, and redirect-related fields, or
 * with a body file set.
 * @param request
 * @return not-exact copy of a location found
 */
Location Server::ProcessRequest(Connection &connection) const {
    ClientRequest &request = connection.request_;

    Srch_c_Res res = config_.FindConstLocation(request.GetAddress());
    l_loc_c_it found = res.location_;
    Location synth(*found);

    if (RequestBodyExceedsLimit(found, request)) {
        Log("client intended to send too large body", connection.server_listening_socket_);
        synth.SetReturnCode(BODY_TOO_LARGE);
    } else if (AccessForbidden(found, request.GetMethod())) {
        // limit_access rule prohibits request
        Log("access forbidden by rule", connection.server_listening_socket_);
        synth.SetReturnCode(ACCESS_FORBIDDEN);
    } else if (found->return_code_ == 0) {
        // return redirection rule isn't set
        if (!found->uploads_path_.empty()) {
            HandleUpload(request, connection.connection_socket_, found, synth);
        } else if (found->is_cgi_) {
            HandleCGI(connection, res, found, synth);
        } else {
            HandleStatic(request, res, found, synth);
        }
    }
    return synth;
}

Location &Server::HandleCGI(const Connection &connection, const Srch_c_Res &res,
                            const l_loc_c_it &found, Location &synth) const {
    std::string address = found->root_ + res.leftower_address_;

    if (Utils::CheckFilesystem(address) == COMM_FILE) {

        int pipe_out[2];
        if (pipe(pipe_out) == -1) {
            Log("Failed to create pipe for CGI execution");
            ThrowException("pipe failed");
        }

        connection.active_cgis_++;
        pid_t pid = fork();
        // Child process
        if (pid == 0) {
            ChildCGI(connection, address, pipe_out);
        } else if (pid > 0) {
            ParentCGI(synth, pipe_out, pid, connection.active_cgis_);
        } else {
            ThrowException("fork failed");
        }
    }
    return synth;
}

void Server::ParentCGI(Location &synth, const int *pipe_out, pid_t pid, int &active_cgis) const {
    // Parent process closing unused write end of the pipe
    close(pipe_out[1]);
    char buffer[1024];

    // Read CGI output from the child process
    ssize_t bytes_read;
    waitpid(pid, NULL, 0);
    active_cgis--;
    while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        synth.return_custom_message_ += buffer;
    }
    synth.return_code_ = 200;
    close(pipe_out[0]);
}

void Server::ChildCGI(const Connection &connection, const std::string &address, const int *pipe_out) const {
    // Redirect stdout to pipe_out (write end of the pipe)
    close(pipe_out[0]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[1]);

    // Set environment variables
    std::vector<std::string> env_strings;
    std::vector<char*> env;
    std::string method = "REQUEST_METHOD=" + Utils::ExtractMethod(connection.request_.GetMethod());
    std::string query_string = "QUERY_STRING=" + connection.request_.GetQueryString();
    env_strings.push_back(method);
    env_strings.push_back(query_string);
    if (connection.request_.HasHeader("Content-Type")) {
        env_strings.push_back(connection.request_.GetHeaderValue("Content-Type"));
    }

    for (size_t i = 0; i < env_strings.size(); ++i)
        env.push_back(const_cast<char*>(env_strings[i].c_str()));
    env.push_back(NULL);

    // Execute the CGI script
    char *args[] = { const_cast<char*>(address.c_str()), NULL };
    execve(address.c_str(), args, env.data());

    // Exit on failure
    _exit(1);
}

void Server::HandleStatic(const ClientRequest &request, const Srch_c_Res &res,
                          const l_loc_c_it &found, Location &synth) const {
    // It seems like there is no reason to even read the body because it's
    // not clear how should static file handle it ?
    std::string address = found->root_ + res.leftower_address_;
    int fs_status = Utils::CheckFilesystem(address);
    if (fs_status == ELSE) {
        // something exist on specified address, but it is neither a file nor a directory
        Log(address + " is neither a file nor a directory.. "
                      "I don't know what to do with it..");
        synth.SetReturnCode(REQUESTED_FILE_IS_NOT_A_FILE);
    } else {
        if (request.IsDirectoryRequest()) {
            // request's address part of URI ends with "/"
            SynthIndex(synth, res, fs_status);
        } else {
            // request's address part of URI has a filename after the last "/"
            SynthFile(synth, res, fs_status, request.GetAddress());
        }
    }
}

void Server::HandleUpload(ClientRequest &request, int socket,
                          l_loc_c_it &found, Location &synth) const {
    if (request.GetMethod() == POST) {
        // Try to perform upload
        int upload_status = CheckUploadRequest(request, found, socket);
        synth.SetReturnCode(upload_status);
        if (synth.return_code_ == OK) {
            synth.return_custom_message_ = "Upload successful";
        }
    } else {
        Log("only POST method should be used for upload locations");
        synth.SetReturnCode(UNAPROPRIATE_METHOD);
    }
}

void Server::SynthFile(Location &synth, const Srch_c_Res &res, int fs_status,
                       const std::string &request_address) const {
    const l_loc_c_it &found = res.location_;
    std::string address = found->root_ + res.leftower_address_;
    // request's address part of URI has an address after last "/" check with
    // leftower-address
    if (fs_status == NOTHING) {
        Log("open() \"" + address + "\" failed");
        synth.SetReturnCode(NOT_FOUND);
    } else if (fs_status == DIRECTORY) {
        // redirect to index request
        synth.SetReturnCode(REDIRECT);
        synth.return_internal_address_ = request_address + "/";
    } else {
        synth.body_file_ = address;
        synth.SetReturnCode(OK);
    }
}

bool Server::RequestBodyExceedsLimit(l_loc_c_it found,
                                     const ClientRequest &request) const {
    if (request.GetMethod() == POST || request.GetMethod() == DELETE) {
        if (found->client_max_body_size_ < request.GetDeclaredBodySize()) {
            return true;
        }
    }
    return false;
}

bool Server::AccessForbidden(l_loc_c_it found, Methods method) const {
    if (found->limit_except_.except_.empty() ||
        found->limit_except_.except_.find(method) !=
            found->limit_except_.except_.end() ||
        (found->limit_except_.except_.find(method) ==
            found->limit_except_.except_.end() &&
        found->limit_except_.allow_all_)) {
        return false;
    }
    return true;
}
