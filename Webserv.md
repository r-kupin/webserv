The naive re-implementation of nginx web server.
Intended to be used with `webserv.conf_` which address is given a s argument or with the default config, located at `resources/default`.
# Init
## Arg check
In order to work, server needs a config, which should be passed as a one optional argument. If such argument is present, server will try to create a `Config` object which is intended to store `Node`s, each one dedicated to a particular parameter.
In case if provided address doesn't exist, *isn't readable*, if config made with mistakes or there were no arguments at all - server will try to load a default config, performing the same checks as for the custom one.
## Configuration
In the runtime all the data stored inside of the object `Config`, which hosts `Nodes`, so all `Config`'s methods are dedicated to store, access and modify them.
In `src/Config/Config.h`
....
# Server setup









































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