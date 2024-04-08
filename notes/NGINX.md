- ### Request for the /`address` 
	- Path `server_root`/`address` exists
		- Return `301` and send to `address`/
	- Path `server_root`/`address` does not exists
		- Return `404` 
- ### Request for the /`address`/
	-  Path `server_root`/`address`/ exists
		- `location` defined with the /`address`
			- implicitly requesting `index` 
				- `index` exists and readable - `200`
				- `index` file doesn't exists - `200` and show the parent index
				- `index` file exists, but not defined - `304`, and show index of super location
				- `index` file defined, but not exist - `403`, and show index of super location
			- root defined as:
				- `root_address`/ server looks for `server_default_resource_directory`/`address`/`root_address`
				- `root_address` - same
				- /`root_address` - /`address`/`root_address`
			- has `deny all`  - returns `403` 
			- `return` `statuscode` `location`  - works as expected 
		- location not defined
			- server returns `304`, and show index of super location
	-  Path `default` /`address` not exists
		- `location` not defined
			-  return parent's (root's) `404`
		- `location` defined with the /`address`
			- has `deny all`  - returns `403` 
			- `return` `statuscode` `location`  - works as expected 
			- `root`, `index` - are ignored, returns `404`
			- if `error_page` is defined - `304` and show parent's index

- At least 1 `server` block
	- needs only 1 `listen` directive specifying a valid port - fails if port is wrong
	- May have 1 or more `location` directives
		- Location needs  to have its `address` and at least one of those:
			- `Address`
				- If `location` has explicitly defined root - server looks for a parent root location + `address`
				- If `location` doesn't has explicitly defined root
					- If `location` 's address is an existing directory: 
			- Sub-location
			- `limit-except` block

uri ends:
/`location`
/`location`/
/`location`/`file.txt`











































1. The server starts by reading the nginx configuration file, which specifies the port number, server root_ directory, and any other settings.
2. The server creates a listener socket and binds it to the port number specified in the configuration file.
3. The server enters an infinite loop, waiting for incoming connections using poll().
4. When a client connects to the server, the server accepts the connection and creates a new thread or worker process to handle the raw_request_.
5. The server reads the incoming raw_request_ from the client and parses it to determine the requested resource and any additional information, such as headers or query parameters.
6. If the raw_request_ includes a cookie, the server extracts the session ID from the cookie and checks if it corresponds to an existing session. If a session exists, the server retrieves the session data from a storage medium such as a file or database. Otherwise, the server generates a new session ID and creates a new session.
7. The server checks if the requested resource is a static file or a dynamic content generator, such as a CGI script or a PHP script.
8. If the requested resource is a static file, the server reads the file from disk and sends it back to the client in the response. If a session ID cookie is present, the server adds the session ID to the cookie and sets an expiry date.
9. If the requested resource is a dynamic content generator, the server invokes the appropriate handler module, such as mod_cgi or mod_php, to generate the content. If a session ID cookie is present, the server passes the session data to the handler as an environment variable or input parameter.
10. After generating the response, the server adds a Set-Cookie header to the response containing the session ID and expiry date, if a session was created or updated.
11. The server sends the response back to the client and closes the connection. The thread or worker process is terminated or returned to the pool_.
12. The server periodically saves the session data to a storage medium to ensure that sessions persist across server restarts.
13. The server can be configured to use SSL/TLS encryption for secure
	connections.

14. The server can be configured to use virtual hosting to serve multiple
	domains from the same server.

15. The server can be configured to use URL rewriting to map requests to
	different resources or to implement clean URLs.

16. The server can be configured to use caching to improve performance and
	reduce server load.

17. The server can implement advanced security features such as rate limiting,
	IP blocking, and raw_request_ filtering.

18. The server can log requests and responses for debugging and analysis
	purposes.

19. To parse the nginx configuration file, the server reads and parses the
	configuration file manually, following the syntax and semantics of the nginx configuration file format.

20. The server extracts the relevant settings from the configuration file and
	uses them to configure the server's behavior, such as the server's root_ directory, port number, and other settings.

fd_set workflow:

1. Init
	1. Init each server's sockets with **port** & **BACKLOG**=10
		1. Init socket basis (**domain** AF_INET, **service** SOCK_STREAM, **protocol** PROTOCOL, **port**, **interface** INADDR_ANY)
			1. _domain = domain;  
			2. _service = service;  
			3. _protocol = protocol;  
			4. _port = port;  
			5. _interface = interface;
		2. Init sockaddr_in
			1. _address.sin_family = domain;  
			2. _address.sin_port = htons(port); 
			3. _address.sin_addr.s_addr = htonl(interface);
		3. Create socket : int **_socket_fd** = **`socket(domain, service, protocol)`**
	2. Connect each server's socket
		1. **`setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));`**
		2. **`bind(socket_fd, (struct sockaddr *)&address, sizeof(address));`**
	3. Start listening
		1. in socket of each server _listening = **`listen(socket_fd, _backlog)`**
		2. assign server's **fd** = **socket_fd**
2. Run
	1. Init fd_sets **_recv_fd_pool** and  **_send_fd_pool**
		1. clear  before use
			1. **`FD_ZERO(&_recv_fd_pool);`**
			2. **`FD_ZERO(&_send_fd_pool);`**
		2. Add each **serer's fd** to **_recv_fd_pool**
			1.  set **serer's fd** to non-blocking mode **`fcntl(serverFd, F_SETFL, O_NONBLOCK)`**
			2. add to set `_addToSet(serverFd, &_recv_fd_pool)`
				1. modify *max_fd* if ** **serer's fd** exceeds it
				2. **`FD_SET(serverFd, recv_fd_pool)`**;
			3. _max_fd = serverFd
	2. Main loop
		1. Save copies of **fd_pool**s
		2. Block execution until any activity on sockets observed
			1. **`select(_max_fd + 1, &recv_fd_pool_copy, &send_fd_pool_copy, NULL, NULL)`**
		3. On unblock - `for (int `**fd**` = 3; fd <= _max_fd; fd++)` and check for activity on each one
			1. If activity spotted on the side of the *recv_fd_pool*
				1. check that no servers are already working with this **fd** (?) `clientsMap.count(fd) > 0`
					1. Accept connection (**fd**)
						1. **f_d** = **`accept(fd, (struct sockaddr *)&address, (socklen_t *)&address_len)`**
						2. If **f_d** is good - add to **_recv_fd_pool**
							1. modify *max_fd* if **f_d** exceeds it
							2. **`FD_SET(f_d, _recv_fd_pool)`**;
						3. set newly obtained **f_d** to nonblock **`fcntl(f_d, F_SETFL, O_NONBLOCK)`**
						4. save *server*-**f_d** correspondence
				2.  If it's already in base - request is accepted and we have to handle it...
					1. Handle connection (**fd**)
						1. Create request from **fd**
						2. retrieve pointer to server from correspondence by **fd**
						3. make srever generate response
						4. remove **fd** from _recv_fd_pool
							1. modify *max_fd* if **fd** exceeds it
							2. **`FD_CLR(fd, _recv_fd_pool);`**
						5. add **fd** to *_send_fd_pool*
							1. modify *max_fd* if **fd** exceeds it
							2. **`FD_SET(fd, _send_fd_pool)`;**
			2. Activity at _send_fd_pool
					1. Respond (**fd**)
						1. perform **`send(fd, response buffer retrieved from map by "fd")`**
						2. remove **fd** from _send_fd_pool
						3. add **fd** to _recv_fd_pool

Multithreading way:
1. each server is a running thread and has a ref to thread pool
2. when epoll signals new connection: 
	1. accept in server's thread
3. when epoll signals event:
	1. pushes task to the thread pool, task that has references to all data in server required to handle it
	2. task is associated with a buffer, where thread's log stored 
	3. when thread is done - server prints out contents of dedicated buffer