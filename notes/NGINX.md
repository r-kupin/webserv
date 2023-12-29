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
4. When a client connects to the server, the server accepts the connection and creates a new thread or worker process to handle the request_.
5. The server reads the incoming request_ from the client and parses it to determine the requested resource and any additional information, such as headers or query parameters.
6. If the request_ includes a cookie, the server extracts the session ID from the cookie and checks if it corresponds to an existing session. If a session exists, the server retrieves the session data from a storage medium such as a file or database. Otherwise, the server generates a new session ID and creates a new session.
7. The server checks if the requested resource is a static file or a dynamic content generator, such as a CGI script or a PHP script.
8. If the requested resource is a static file, the server reads the file from disk and sends it back to the client in the response. If a session ID cookie is present, the server adds the session ID to the cookie and sets an expiry date.
9. If the requested resource is a dynamic content generator, the server invokes the appropriate handler module, such as mod_cgi or mod_php, to generate the content. If a session ID cookie is present, the server passes the session data to the handler as an environment variable or input parameter.
10. After generating the response, the server adds a Set-Cookie header to the response containing the session ID and expiry date, if a session was created or updated.
11. The server sends the response back to the client and closes the connection. The thread or worker process is terminated or returned to the pool.
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
	IP blocking, and request_ filtering.

18. The server can log requests and responses for debugging and analysis
	purposes.

19. To parse the nginx configuration file, the server reads and parses the
	configuration file manually, following the syntax and semantics of the nginx configuration file format.

20. The server extracts the relevant settings from the configuration file and
	uses them to configure the server's behavior, such as the server's root_ directory, port number, and other settings.



- Root <- *parent*
	- Root <- current = parent
		- Root
			- A
			- B <- Same Address as current -
		- A
			- A
			- B <- New location
		- C <- New location
	- A <- current != parent
		- A
		- B