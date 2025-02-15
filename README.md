# HowTo
## Run
1. Compile with `make`
2. Launch as `webserv [ path_to_config ]`
3. Connect to the servers on ports defined in the config with any HTTP network-accessing app
## Test
1. Prepare a test library 
	```shell
	git clone git@github.com:google/googletest.git test/lib
	mkdir test/lib/build && cd test/lib/build 
	cmake ..
	```
2. Get back to project's root
3. Run `make test`
# Features
## Done
- Your program has to take a configuration file as argument, or use a default path.
- Your server must never block.
- The Client can be bounced properly if necessary. (? return, I guess)
- It must be non-blocking
- Use only one poll() (or equivalent) for all the I/O operations between the client and the server (listen included).
- poll() (or equivalent) must check read and write at the same time.
- Never do a read or a write operation without going through poll() (or equivalent).
- No **errno** checking
- Non-blocking IO
- A request to your server should never hang forever.
- Your server must be compatible with the web browser of your choice
- Your HTTP response status codes must be accurate.
- Your server must have default error pages if none are provided.
- You must be able to serve a fully static website.
- Clients must be able to upload files. ([upload_store](#upload_store))
- Make it work with DELETE, POST and GET methods.
- Stress test resilience
- Multiple ports
- Choose the [port](#listen) and [host](#server_name) of each server.
- Set up the server_names or not
- default server for host:port 
- Setup default [error_pages](#error_page).
- Limit [client_max_body_size](#client_max_body_size).
- Setup routes with one or multiple of the following rules/configuration:
	- Define a list of accepted HTTP methods for the route. ([limit_except](#Limit_except))
	- Define an HTTP redirection. ([return](#return))
	- Define a directory or a file from where the file should be searched. ([root](#root))
	- Set a default file to answer if the request is a directory ([index](#index)).
	- CGI ([cgi_address](#cgi_address))
	- Make the route able to accept uploaded files and configure where they should be saved. ([upload_store](#upload_store))
	- Turn on or off directory listing. ([autoindex](#autoindex))
    
# Config
Like `nginx.conf` but with less functional supported. This project follows the philosophy of forward compatibility—meaning that all valid configs for WebServ will be also valid for NGINX and will work in the exact same way *EXCEPT* for the [upload_store](#upload_store) and [cgi_address](#cgi_address) directives. More on that in the dedicated section.
Feel free to consult the test configs provided in `test/test_resources`. 
## Config structure
Config consists of **contexts** and **directives**.
### Contexts
Context is a block defined in the following way:
``` nginx
context_name [ ARG ] {
	...
}
```

For now, webserv supports the following contexts:
#### Server
The main context of the instance of the HTTP server. At least one should be defined in the config. 
Server context can't be empty—it should contain mandatory server-level directives: 
- *[server_name](#server_name)* (unique)
- *[listen](#listen)*
- *[keepalive_timeout](#keepalive_timeout)* (unique)

Server also can predefine root location with optional directives:
- *[root](#root)* (unique)
- *[index](#index)*
- *[error_page](#error_page)*
- *[client_max_body_size](#client_max_body_size)* (unique)
- *[upload_store](#upload_store)* (unique)
- *[autoindex](#autoindex)* (unique)

Inside server context multiple **location** sub-contexts can be defined, to handle specific requests.
```nginx configuration
server {
	listen 4281;  
	server_name localhost;  
	root /var/www;

	location / { ... }
}
```

#### Location
Location sets configuration depending on a request URI. 
Locations can be defined inside of the server or parent location context (nested locations). The Server matches the request URI against all defined locations, and then assigns handling to the location with the closest matching *address*.
Location context should be defined with a single argument, which is *address*.
```nginx configuration
location address {
	...
}
```

The *address* - is the absolute path from the **root** location, there are no relative paths. It means, that if location *"loc_n"* should be placed inside location *"/loc_1"* - the address should be defined as follows: *"/loc_1/loc_n"*, regardless of whether the super-context is *location /loc_1*, *location /* or *server*:
```nginx configuration
# OK
location / {
	location /loc_1 {
		location /loc_1/loc_2 {
			...
		}
	}
}

location /loc_1/loc_3 {
	....
}
```

But it can't be defined in any context, a part of the mentioned above:
```nginx configuration
# NOT OK
location / {
	location /loc_1 {
		location /loc_3 {
			...
		}
	}
	# "/loc_3" should be defined here
}
# or here
```

Redefinition of locations is possible:
```nginx configuration
# OK
location / {
	location / {
		# This will be the actual version of "/"
		location /loc_1 {
			...
		}
	}
	location /loc_1 {
		...
	}
}

location /loc_1 {
	# And this will be the actual version of "/loc_1"
	... 
}
```

However, one super-location/server can't contain multiple sub-locations with the same addresses:
 ```nginx configuration
# NOT OK
location / {
	
	location /loc_1 {
		...
	}
	
	location /loc_1 {
		...
	}
}
```

Locations also can be mentioned, but not defined explicitly:
```nginx configuration
# OK
server {
	listen 4281;  
	server_name localhost;  
	root /var/www;
	
	location /loc_1/loc_2 {
		# definition of "/loc_1/loc_2"
		...
	}
	# no explicit definition of "/loc_1"
}
```

In this project, such locations referred as **ghost** locations. In the example above, request to `localhost:4281/loc_1/` will lead to `403 Forbidden` server response.
Locations can be empty or contain the following directives:
- *[root](#root)* (unique)
- *[client_max_body_size](#client_max_body_size)* (unique)
- *[upload_store](#upload_store)* (unique)
- *[index](#index)*
- *[return](#return)* (unique)
- *[error_page](#error_page)*
- *[autoindex](#autoindex)* (unique)
- *[proxy_pass](#proxy_pass)* (unique)

Locations can also contain sub-contexts:
- *[limit_except](#limit_except)* (unique)
- nested location
#### Limit_except
Limits access to location. Defined only inside a location with one or more *HTTP* methods:
```nginx configuration
limit_except METHOD {
	...
}
```

The `METHOD` parameter can be one of the following: `GET`, `POST`,  or  `DELETE`, as subject requires. The *limit_except* is a first thing being checked upon the access to the *location*. If request method is not allowed - server immediately responds with *403 Forbidden*.
Limit_except can't be empty, and should contain the following directives:
- *deny*
- *allow*

Depending on the intention of prohibiting or allowing access. 
Limit_except can't have any sub-contexts.
### Directives
Directive is a single-line instruction defined in the following way:
```nginx configuration
directive [ ARG1 ] [ ARG... ];
```

#### Server-level directives
##### listen
It Has only one arg which sets **IP**:**port** used to open the socket, on which requests will be monitored. If **IP** is not specified - *localhost* used by default.
##### server_name
The `server_name` directive defines the domain name(s) that the server block should respond to. When server receives an HTTP request, it checks the `Host` header of the request against the `server_name` values configured in each server block to determine which block should handle the request. If there's a match, Nginx forwards the request to that server block, or it uses the default server otherwise. Default server is the first server defiled in config file and listens **host**:**port** on which request was reported.
##### keepalive_timeout
Accepts one number, which is a timeout during which server keeps connection open waiting for data to arrive. Default is 60 sec. It gives no warranty that the connection will be closed in 60 seconds, but the connection will stay open for at least during this time.
#### Location-level directives
##### root
Can have only one arg, which is a path for a location, or server's root directory. For example:
```nginx configuration
server {
	...
	root /var/www; # absolute path
	
	location /loc_0 {}
	
	location /loc_1 { 
		root resources; # relative path
		location /loc_1/loc_2 {} 
	}
}
```

In this case:
- URI with address `/text.txt` would make server look for `text.txt` in `/var/www/`.
- URI `/loc_0/text.txt` will be handled in `/var/www/loc_0/`, because nginx appends location address to parent location's root, if it isn't overridden.
- URI `/loc_1/text.txt` will be handled by path, constructed as `path to executable` + `resources` + `/loc_1`
- URI `/loc_1/loc_2/text.txt` will be handled by path, constructed as `parrent's root` + `/loc_2`

##### client_max_body_size
Should have only one arg, which is a number of bytes.
Sets bounds for request's body size. Works in the following way: while reading client's body, server keeps track of its size. If `client_max_body_size` is defined, and client's body exceeds it - server abandon's further request processing and returns error **413**. If not specified - default value of 1Mb is being applied.
##### upload_store
In this project, its behavior is slightly simplified because this directive is not a part of vanilla nginx, but from a third-party module, more info [here](#Uploads).
 **Works only for requests done with CURL**
Should have only one arg, which is a path to the uploads' directory.
Set's path to upload directory. When the location containing this directive handles POST request, it creates a file in specified directory, and writes the request's body to it. The name of the file being created is it's number: first is `1`, second is `2`, etc. If File already exists or it's not possible to create it - server returns 503
##### index
May have multiple args that define files that will be used as an index - meaning - shown when location get's accessed by address, following with `/`. Files are checked in the specified order - left to right. The last element of the list can be a file with an absolute path - meaning, path not from the current location's root - but from **root**-location's root.
```nginx configuration
index index_X.html index_1.html index_2.html;
```

Indexes are checked in the following order: 
###### defined in current location
1. Return first index found
2. Return *403* if none of specified files exists & is accessible
3. 
###### not defined in current location, but in parent location
Check parent location in the same way, except parent *index filenames* specified in *parent* are expected to be located at the *current location's root*:
```nginx configuration
server {  
	...
	root www;
	index index_X.html index_1.html index_2.html;
	
	location /loc_1 {
	    # Request /loc_1/
	    # Checks www/loc_1/index_X.html first, /loc_4/index.html - then
	    # Returns 403 if both are not accessible
	    index index_X.html /loc_4/index.html;
	}  
	
	location /loc_2 {
	    # Request /loc_2/
	    # Checks www/loc_2/index_X.html, www/loc_2/index_1.html then
	    # www/loc_2/index_2.html
	    # Returns 403 if all are not accessible
	}
}
```

###### no definition up to the root
Default index `index.html` is being checked 
```nginx configuration
server {  
	...
	root www; # webserv
	# no index definition
	
	location /loc_1 {
	    index index_X.html /loc_4/index.html;
	    # Request /loc_1/
	    # Checks www/loc_1/index_X.html first, /loc_4/index.html - then
	    # Returns 403 if both are not accessible
	}  
	
	location /loc_2 {
	    # Request /loc_2/
	    # Checks www/loc_2/index.html
	    # Returns 403 if it is not accessible
	}
}
```

##### cgi_address
Directive, that specifies the executable file that should process request.
```nginx configuration
	location /cgi-bin {
	    cgi_address test_cgi_bash.sh;
	}
```
Works in a following way:
1. On request to the location, containing `cgi_address` directive, server executes specified file in a separate process. It is expected that the executable will :
   1. Acquire data by reading it from `stdin` and provide output by writing it to the `stdout`
   2. Read all data until EOF and only then will write it's output 
2. Client's request in a raw form transferred to the CGI process.
3. All input of the CGI process transferred to the client directly, as soon as server process reads it.

On receiving of the request, server only parses it's headers, in order to determine the server and location. The response generated by the CGI script is not getting processed at all and provided as-is.
All interactions with CGI process are async, meaning the server only writes to CGI when it tries to read and only reads when CGI process writes.   

##### return
Directive, responsible for redirection. Stops processing request and returns the specified code to a client. Should have one or two args.
```nginx configuration
location /redirect_no_code {  
    return /target_location;  
}

location /redirect_internal {  
    return 302 /target_location;  
}  
  
location /redirect_external {  
    return 302 http://example.com;  
}  
  
location /target_location {  
    return 200 "Welcome to the target location!";  
}
```

- If return has 1 argument, it should be a `return code` or `address`.  
- If return has 2 arguments, it should be a `return code` and `address` or `custom message` - depending on the `return code` value.
- There can't be more than 2 args, and `code` can't be the second arg. 
- The redirect, if only address specified, is done with *302* code

##### error_page
Similar to *index* - it is possible to define custom error pages for each location.
Error_page directive expects one or more `error code`(s) followed by a `filename` of the error page, that should be sent to the client in case if one of the specified errors will happen.
In case, if error page is not defined, or defined file doesn't exist - webserv would *auto generate default error page automatically*.
Example:
```nginx configuration
error_page 403 404 /error.html;
```

##### autoindex
Takes a single argument - `on` or `off`.
Is `off` by default, in this case server behaves as usual.
If `on` - server will ignore all implicit or explicit indexes that are present in it's root and would generate *directory listing html* instead. This is basically a page that mimics direct access to the file system - content's of the location's root specifically. All files are accessible via links.
If any subdirectory of the autoindexed location's root is defined as location explicitly - it's own rules would be applied.

##### proxy_pass


# How it actually works?
## Init
### Arg check
In order to work, server needs a config, which should be passed as a one optional argument. If such argument is present, server will try to create a `Config` object which is intended to store `Node`s, each one dedicated to a particular parameter.
In case if provided address doesn't exist, *isn't readable*, if config made with mistakes or there were no arguments at all - server will try to load a default config by the address `resources/nginx.conf`, performing the same checks as for the custom one.
## Setting up Config
### Config
Main class, storing configurations for all servers is `Config`. All its methods are dedicated to parsing config file to list of `ServerConfiguration` classes, each one storing a configuration for each particular server.
### ServerConfiguration
Particular config, the backbone of each server. Contains server-level data, such as *server_name*, *port* and the root of the tree of `Locations`.
`ServerConfiguration`'s functionality is narrowed to function
```c++
LocConstSearchResult    FindConstLocation(const std::string &address) const;
```

that searches the locations tree for a requested location, and returns a `LocSearchResult`, that contains iterator to the closest found location, as well as some additional info.
### Location
Stores data about all [locations](#location) mentioned in config:
```c++
	std::set<ErrPage>       error_pages_;  
	l_loc                   sublocations_;  
//-------------------index related  
	bool                    has_own_index_defined_;  
	bool                    index_defined_in_parent_;  
	l_str                   own_index_;  

	Limit                   limit_except_;
	bool                    autoindex_;  
	std::string             dir_to_list_;
//-------------------redirect related  
	int                     return_code_;  
	std::string             return_internal_address_;  
	std::string             return_external_address_;  
	std::string             return_custom_message_;  
  
	std::string             root_;  
	std::string             full_address_;	// address from the root path
	std::string             address_;	// particular location's address
	std::string             body_file_;	// address of file being sent to client
	l_loc_it                parent_;	// root location's "parent" points on itself
	bool                    ghost_;
```

## ServerManager
ServerManager class is main and the only active element in the single-threaded non-blocking event-driven web server application. It is built around 
- `epoll_fd_` : file descriptor referring to the epoll instance in kernel
- `servers_` : container of the `Server` class
- `host_to_socket_` : map for all open sockets to `Host`s  - IPv4 and port
- `connections_` : connection-to-fd mapping in order to keep data sent by parts

### Setting up 
#### Setting up epoll
**Epoll** is a monitoring system for the file descriptors. It helps to see if I/O is possible on any of them. It's API consists of :
- The *epoll instance* in the kernel is essentially a data structure maintained by the Linux kernel to facilitate event notification for multiple file descriptors. It acts as a central registry where the kernel keeps track of which file descriptors program is interested in monitoring for events, such as data arrival or readiness for reading/writing.
- *epoll fd* is almost regular fd that is used to perform various actions on that instance
- *epoll system calls*
	- `epoll_create()` creates new instance and returns it's fd
	- `epoll_ctl()` performs some actions on given epoll_instance such as adding or removing of file descriptors being monitored
	- `epoll_wait()` blocks execution of the program waiting for events on monitored file descriptors to happen

#### Opening of the sockets for each server
Each of the defined server listens for one or multiple addresses, and the external interface of each address is a **socket**.
It is a data structure that represents an endpoint for communication. It's basically a like a file descriptor used by the kernel to manage network communication between processes. In order to open socket for communication several setup steps are required.
##### Create socket
When the `socket` system call is invoked, the Linux kernel allocates a new file descriptor and initializes data structures for the socket. Internally, this involves allocating memory for the socket control block and setting up pointers to various kernel functions related to socket operations. Additionally, the kernel initializes the socket's state and other attributes, such as its protocol family, type, and communication semantics (e.g., TCP, UDP, stream-oriented, datagram-oriented).
##### Set socket options
After the socket is created, certain options may need to be configured to customize its behavior. For instance, the `SO_REUSEADDR` option, set using the `setsockopt` system call, modifies the socket's behavior regarding address reuse. When this option is set, the kernel updates the socket's internal data structures to indicate that the address and port associated with the socket can be reused even if it's still in a `TIME_WAIT` state from a previous connection. This enables faster server restarts and prevents "address already in use" errors.
##### Bind socket
Binding a socket to a specific address and port is achieved through the `bind` system call. When this call is made, the kernel updates its internal routing tables to include an entry mapping the specified address and port to the socket's file descriptor. This essentially tells the networking subsystem of the kernel to direct incoming packets destined for the specified address and port to the socket for processing. The kernel also checks for permissions and ensures that the requested address and port are available for binding.
##### Start listening
Once the socket is bound to an address and port, it needs to start listening for incoming connections. Invoking the `listen` system call sets the socket's state to listening and configures it to accept incoming connections. Internally, the kernel sets up a queue for pending connections associated with the socket and begins accepting incoming connection requests. The backlog(last) parameter passed to `listen` determines the maximum length of this queue. In my case it is `SOMAXCONN` - a maximum amount of connections, which is **4096** on 42's computers.  As new connection requests arrive, the kernel adds them to the queue, up to the specified backlog limit. If  the queue is full, subsequent connection requests may be rejected.

When socket is set up to listen for the connections it is fully usable. 
##### Add socket to the epoll watchlist
In order to be able to receive notifications on the events happening on multiple listening sockets we need to add it to the epoll instance.
The `epoll_ctl` system call is used to control the behavior of the epoll instance, such as adding or removing file descriptors from its watch list, or changing the events of a file descriptor already in the list.
There are options on how do we want to get notified about events, and what particular events to monitor. This is done, by setting flags to `epoll_event.events`.
The **epoll** API can be used either as an edge-triggered or a level-triggered interface and scales well to large numbers of watched file descriptors.

For example setting the flag `EPOLLIN` would mean that we'll get notified when on the client's end of the communication line `write` or similar operation will be performed. `EPOLLOUT` - same but for `read` or similar.

Alongside with `EPOLLIN` and/or `EPOLLOUT` the `EPOLLET` flag might be specified - if so, notifications on this file descriptor would be **Edge Triggered**. Otherwise the default **Level Triggered** strategy will be applied.
###### Edge Triggered notifications
Events are triggered only if they change the state of the `fd` - meaning that only the first event is triggered and no new events will get triggered until that event is fully handled. 
This design is explicitly meant to prevent `epoll_wait` from returning due to an event that is in the process of being handled (i.e., when new data arrives while the `EPOLLIN` was already raised but `read` hadn't been called or not all of the data was read). The edge-triggered event rule is simple **all same-type (i.e. `EPOLLIN`) events are _merged_ until all available data was processed**. 
In the case of a listening socket, the `EPOLLIN` event won't be triggered again until all existing `listen` "backlog" sockets have been accepted using `accept`.
###### Level Triggered
On the other hand, level triggered events will behave closer to how legacy `select` (or `poll`) operates, allowing `epoll` to be used with older code.
The event-merger rule is more complex: **events of the same type are only merged if no one is waiting for an event** (no one is waiting for `epoll_wait` to return), **or if multiple events happen before `epoll_wait` can return**... otherwise any event causes `epoll_wait` to return.
In the case of a listening socket, the `EPOLLIN` event will be triggered every time a client connects... unless no one is waiting for `epoll_wait` to return, in which case the next call for `epoll_wait` will return immediately and all the `EPOLLIN` events that occurred during that time will have been merged into a single event.
In the case of a byte stream, new events will be triggered every time new data comes in... unless, of course, no one is waiting for `epoll_wait` to return, in which case the next call will return immediately for all the data that arrive util `epoll_wait` returned (even if it arrived in different chunks / events)
###### OneShot mode
The behavior of `EPOLLONESHOT` is such that after a successful call to `epoll_wait(2)` where the specified file descriptor was reported, no new events will be reported by `epoll_wait(2)` on the same file descriptor until you explicitly reactivate it with `epoll_ctl(2)`. You can look at it as a mechanism of temporarily disabling a file descriptor once it is returned by `epoll_wait(2)`.
It does not prevent `epoll_wait(2)` from returning more than one event in the same call for the same file descriptor - in fact, if multiple events are available at the time of the call, they are all combined into the `events` field of `struct epoll_event`, whether or not `EPOLLONESHOT` is in effect for that file descriptor.
In other words, `EPOLLONESHOT` controls under what conditions a file descriptor is _reported_ in a call to `epoll_wait(2)`; it does not play a role in event aggregation and detection.
1. **Edge-triggered mode with EPOLLONESHOT:**
    - In edge-triggered mode, EPOLLONESHOT means that once an event occurs on a file descriptor and is reported by epoll_wait(), the associated file descriptor is deactivated until it is re-armed using epoll_ctl() with EPOLL_CTL_MOD and EPOLLONESHOT again.
    - In this mode, epoll_wait() will report an event only once for a given file descriptor until it is re-armed. Subsequent events on the same file descriptor will not be reported until it is re-armed.

2. **Level-triggered mode with EPOLLONESHOT:**
    - In level-triggered mode, EPOLLONESHOT behaves differently. Once an event occurs on a file descriptor and is reported by epoll_wait(), the associated file descriptor remains active, and epoll_wait() will continue to report events on that file descriptor as long as the condition for the event remains true.
    - In this mode, EPOLLONESHOT doesn't mean the file descriptor is deactivated after one event. Instead, it indicates that epoll_wait() will not report further events on the file descriptor until the current event condition changes and then resets the EPOLLONESHOT flag.

*In summary*:
- With edge-triggered mode, EPOLLONESHOT means the file descriptor is deactivated until rearmed.
- With level-triggered mode, EPOLLONESHOT means the file descriptor remains active, but epoll_wait() will not report further events until the current event condition changes.

```c++
// server.cc
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool oneshot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if(oneshot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}
// reset the fd with EPOLLONESHOT
void reset_oneshot(int epollfd, int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

int main(int argc, char** argv)
{
    if(argc <= 2)
    {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    addfd(epollfd, listenfd, false);
    while(1)
    {
        printf("next loop: -----------------------------");
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if(ret < 0)
        {
            printf("epoll failure\n");
            break;
        }
        for(int i = 0; i < ret; i++)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd)
            {
                printf("into listenfd part\n");
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr*)&client_address,
                     &client_addrlength);
                printf("receive connfd: %d\n", connfd);
                addfd(epollfd, connfd, true);
                // reset_oneshot(epollfd, listenfd);
            }
            else if(events[i].events & EPOLLIN)
            {
                printf("into linkedfd part\n");
                printf("start new thread to receive data on fd: %d\n", sockfd);
                char buf[2];
                memset(buf, '\0', 2);
                // just read one byte, and reset the fd with EPOLLONESHOT, check whether still EPOLLIN event
                int ret = recv(sockfd, buf, 2 - 1, 0);

                if(ret == 0)
                {
                    close(sockfd);
                    printf("foreigner closed the connection\n");
                    break;
                }
                else if(ret < 0)
                {
                    if(errno == EAGAIN)
                    {
                        printf("wait to the client send the new data, check the oneshot memchnism\n");
                        sleep(10);
                        reset_oneshot(epollfd, sockfd);
                        printf("read later\n");
                        break;
                    }
                }
                else {
                    printf("receive the content: %s\n", buf);
                    reset_oneshot(epollfd, sockfd);
                    printf("reset the oneshot successfully\n");
                }
            }
            else 
                printf("something unknown happend\n");
        }
        sleep(1);
    }
    close(listenfd);
    return 0;
}
```

More on that [here](https://linux.die.net/man/7/epoll) and [here](https://stackoverflow.com/questions/41582560/how-does-epolls-epollexclusive-mode-interact-with-level-triggering). If you are super curious about the topic - check out [this](http://www.kegel.com/c10k.html) as well.

When adding a socket to the epoll watch list, the `EPOLL_CTL_ADD` command is used. This command instructs the kernel to add the specified socket to the epoll instance's watch list, associating it with a set of events to monitor (e.g., read, write, error).
#### Creating individual servers
When all sockets for particular **ServerConfiguration** are opened and added to the *epoll instance* and **ServerManager**'s map - **Server** itself can be created.
It's function is to process each individual request according to the rules specified in it's configuration. After setting up the last server **ServerManager** is ready to accept connections.
### Wait for the event to happen
The `epoll_wait` system call is used to wait on the `epoll_fd_` - representing *epoll-instance* for the current server for the events on the registered file descriptors. When invoked, it blocks current thread until one or more file descriptors in the epoll instance's watch list become ready for the specified events, or until a timeout occurs. Upon completion, `epoll_wait` returns information about the ready file descriptors and the events that occurred by placing `epoll_event` structures in events array.
The main information about event is *server_fd* or *client_fd* that was watched by this epoll instance, and particular kind of event - *EPOLLIN* and/or *EPOLLOUT*.
Internally, the kernel efficiently scans the epoll instance's data structures to determine which file descriptors are ready for I/O operations, without the need for iterative polling.
It makes sense to process only 2 types of events: 
- *accepting new connection* - when reported fd is a *server_fd* and  event is *EPOLLIN*
- *handling new data on existing connection* - when reported fd is a *client_fd* and  events are *EPOLLIN* and *EPOLLOUT*. Yes, both at the same time.
### Establish connection
First event ever being reported on the server will be a *new-connection-event*.
#### Accept
**ServerManager** proceeds with accepting incoming connections from clients using the `accept` system call to accept the connection request on reported *server_fd* and create a new socket descriptor *client_fd* specifically for this connection.
#### Add client's fd to epoll
Just like with server's sockets - this way we'll be monitoring the events happening on it.
#### Set client's fd to non-blocking state
After accepting the connection and obtaining the new socket descriptor `client_sock`, the server sets this socket to non-blocking mode using the `fcntl` system call. This step ensures that subsequent I/O operations on this socket will not block the calling thread. It's done by adding `O_NONBLOCK` flag to those already set.
#### Initialize connection
In order to be able to keep the state of the connection, considering that request might be split across multiple events - **ServerManager** initializes **Connection** structure in the`connections_` array, where index of the array is *client_fd* - in order to ensure the quick access.
##### Connection
Container to store all details concerning to communication between server and particular client. Main fields are:
```c++
// to keep track of what is already done
bool                url_headers_done_;  
bool                body_done_;  

// to be able to close ones, that are idle for too long
long                open_time_;
int                 connection_socket_; /* client_fd */
int                 server_listening_socket_; /* server_fd */
std::string         address_; /* host:port */
ClientRequest       request_;
// location that difines rules for the requested address
Location            location_;
```

## [Request](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages#http_requests) handling
When connection is accepted, server is ready to receive HTTP requests.
Essentially, HTTP request it is just a message sent by a client to a server:

![request](https://github.com/r-kupin/webserv/blob/main/notes/request.jpg)

Right upon receival of the connection from client, server reads the contents of client request to the `ClientRequest` class.
```c++
//---Request line
Methods                             method_;
//-----------URL
std::string                         addr_;  
std::string                         addr_last_step_;  
bool                                index_request_;  
m_str_str                           params_;
std::string                         fragment_;  
//---Headers & Body
m_str_str                           headers_;
std::string                         body_;  
```

### Request line
-  Method(`method_`): the HTTP method or verb specifies the type of request being made. WebServ is supposed to handle GET, POST and DELETE methods
### [URL](https://developer.mozilla.org/en-US/docs/Learn/Common_questions/Web_mechanics/What_is_a_URL)
- Path (`addr_`): An absolute path, optionally followed by a `'?'` and query string.
- Last Step in Address (`addr_last_step_`): The contents of the address after the last `/` in URI
- Index Request (`index_request_`): flag indicating whether the request is for the default index resource. WebServ, automatically serves a default file (e.g., index.html) when the path points to a directory meaning if address ends with `/`.
- Fragment (`fragment_`): the fragment identifier, often used in conjunction with anchors in HTML documents. It points to a specific section within the requested resource.
- Parameters (`params_`): additional parameters sent with the request. In the URL, these are typically query parameters (e.g., `?key1=value1&key2=value2`).

### Headers (`headers_`)
HTTP headers provide additional information about the request, such as the type of client making the request, the preferred response format, authentication information, etc. In request processing *Webserv* uses following headers:
#### [Host](#https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Host) 
The **`Host`** request header specifies the host and port number of the server to which the request is being sent, or the virtual host ([`server_name`](#server_name)).
If header is missing -  error code *400* will be returned.
#### [Content-Length](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Length) 
The **`Content-Length`** header indicates the size of the message body, in bytes, sent to the recipient. 
This is important if server is up to process request's body - size of the accessed location is always limited implicitly to 1Mb or explicitly with [`client_max_body_size`](#client_max_body_size) directive.
#### [User-Agent](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/User-Agent)
The **User-Agent** [request header](https://developer.mozilla.org/en-US/docs/Glossary/Request_header) is a characteristic string that lets servers and network peers identify the application, operating system, vendor, and/or version of the requesting [user agent](https://developer.mozilla.org/en-US/docs/Glossary/User_agent). 
This header is being checked when server handles the upload request. Because any client uses different convention of wrapping the file being uploaded - uploads from different clients are handled differently.
For example, here is a request to upload a file `test.txt`:
```sh
$> cat -e test.txt
test$
```

That's what **curl** sends to the server:
```
POST /uploads HTTP/1.1\r\n
Host: localhost:4281\r\n
User-Agent: curl/7.81.0\r\n
Accept: */*\r\n
Content-Length: 191\r\n
Content-Type: multipart/form-data; boundary=------------------------ceae335717f1b7a7\r\n
\r\n
--------------------------ceae335717f1b7a7\r\n
Content-Disposition: form-data; name="file"; filename="test.txt"\r\n
Content-Type: text/plain\r\n
\r\n
test\n
\r\n
--------------------------ceae335717f1b7a7--\r\n
```

*Firefox*:
```
POST /uploads HTTP/1.1\r\n
Host: localhost:4281\r\n
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0\r\n
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n
Accept-Language: en-US,en;q=0.5\r\n
Accept-Encoding: gzip, deflate, br\r\n
Content-Type: multipart/form-data; boundary=---------------------------14556203736442811903568275294\r\n
Content-Length: 353\r\n
Origin: http://localhost:4281\r\n
DNT: 1\r\n
Connection: keep-alive\r\n
Referer: http://localhost:4281/\r\n
Upgrade-Insecure-Requests: 1\r\n
Sec-Fetch-Dest: document\r\n
Sec-Fetch-Mode: navigate\r\n
Sec-Fetch-Site: same-origin\r\n
Sec-Fetch-User: ?1\r\n
\r\n
-----------------------------14556203736442811903568275294\r\n
Content-Disposition: form-data; name="fileToUpload"; filename="test.txt"\r\n
Content-Type: text/plain\r\n
\r\n
test\n
\r\n
-----------------------------14556203736442811903568275294\r\n
Content-Disposition: form-data; name="submit"\r\n
\r\n
Upload File\r\n
-----------------------------14556203736442811903568275294--\r\n
```

*Chrome*:
```
POST /uploads HTTP/1.1\r\n
Host: localhost:4281\r\n
Connection: keep-alive\r\n
Content-Length: 299\r\n
Cache-Control: max-age=0\r\n
sec-ch-ua: "Google Chrome";v="123", "Not:A-Brand";v="8", "Chromium";v="123"\r\n
sec-ch-ua-mobile: ?0\r\n
sec-ch-ua-platform: "Linux"\r\n
Upgrade-Insecure-Requests: 1\r\n
Origin: http://localhost:4281\r\n
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryQwNkA1TP3mE9cVAE\r\n
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36\r\n
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n
Sec-Fetch-Site: same-origin\r\n
Sec-Fetch-Mode: navigate\r\n
Sec-Fetch-User: ?1\r\n
Sec-Fetch-Dest: document\r\n
Referer: http://localhost:4281/\r\n
Accept-Encoding: gzip, deflate, br, zstd\r\n
Accept-Language: en-US,en;q=0.9\r\n
\r\n
------WebKitFormBoundaryQwNkA1TP3mE9cVAE\r\n
Content-Disposition: form-data; name="fileToUpload"; filename="test.txt"\r\n
Content-Type: text/plain\r\n
\r\n
test\n
\r\n
------WebKitFormBoundaryQwNkA1TP3mE9cVAE\r\n
Content-Disposition: form-data; name="submit"\r\n
\r\n
Upload File\r\n
------WebKitFormBoundaryQwNkA1TP3mE9cVAE--\r\n
```

All these clients are supported, but anything else will trigger error *501* response.
### Body (`body_`)
The body of the HTTP request, which contains additional data sent to the server. This is particularly relevant for POST requests or other methods where data is sent in the request body. In case of file upload the body will contain file contents.
## [Response](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages#http_responses) creating
Right after the creation of the `ClientRequest` server starts generating response, which involves 3 steps:
- Finding of the server to which request was sent
- Creating a synthetic location 
- Creating a `ServerResponse` class

### Finding requested server
Request should be processed by a server that:  
* Listens on the socket, on which request was reported  
* Has defined `server_name` that corresponds to the request's value of the `Host` header  
If no server's `server_name` corresponds to the request's value of the `Host` header - return the first server that listens on this socket defined in .conf file
If multiple servers `server_name` corresponds to the request's value of the `Host` header - return the last of them defined in .conf file (sort of overrides previous one)
### Creating a synthetic location
Depending on compliance between what was requested and what is being found - server creates a synthetic location - a copy of the location that was found in [ServerConfig](#ServerConfiguration), but with altered return code, and redirect-related fields, or with a body file set.
In order to determine what should be returned, server performs some checks:
#### Request's body check
If request contains body, it's size will be counted while reading from socket. If size would exceed limit - error code *413* will be returned.
#### Access permission
Server checks whether access to requested location is prohibited with [`limit_except`](#Limit_except)
#### Redirection
If access is allowed, server then checks defined [redirection](#return), and bounces client with internal or external redirect, or returns specified code and custom message.
#### Server-side handling
##### Upload request
If found location contains [upload_store](#upload_store)  - all requests to it will be treated as uploads. They have to:
- Be POST
- Have headers set:
	- `User-Agent`: only **curl** and **Firefox** are tested and supported
	- `Content-Type`: should have a `boundary` delimiter - a unique string that separates the individual parts of the message. `boundary` parameter is used to delineate the boundaries between different parts of the message body. 
	- `Content-Length`: corresponds to the size of the request body, which is bigger then the file itself, because it contains some metadata such as filename.
- Have a body of the certain structure:
	1. Start with delimiter preceded by `\r\n--`
	2. Contain some file metadata (optional) followed by `\r\n\r\n` (mandatory)
	3. Contain actual file contents
	4. End up with delimiter preceded by `\r\n--`

If number of bytes processed corresponds with the value of `Content-Length` and the last thing received was delimiter - request is correct. Otherwise - [400  Bad Request](#400  Bad Request) will be returned.
Before the start of the upload process, server also checks the file being created to store this upload:
- Check that the value of `upload_store` points indeed to the directory where we are supposed to create files
- Check that file intended to store current upload doesn't already exist
- Check that server has permissions to create the file
- Check that server's storage has enough free space to store file
If any of those fails - [503 Service Unavailable](#503) will be returned

##### Proxied request


##### Static request
If found location doesn't contain any directives specifying that request should be uploaded or proxied, the server proceeds with checking for the existence of the requested resource.
There are two types of static requests—for a file and for index. If `path` part of the URL ends with `/` - this is an index request, otherwise - file request. That requests are handled differently.
###### Synthetic location for file request
If `path` part of the URL has something after the last `/` symbol, it is assumed that it is a name of the file, that should be located in the root directory of the location, that preceded the filename. Depending on the result of the file system check server finishes response location:
```c++
if (fs_status == NOTHING) {  
    std::cout << "open() \"" + address + "\" failed" << std::endl;  
    synth.return_code_ = 404;  
} else if (fs_status == DIRECTORY) {  
    // redirect to index request  
    synth.return_code_ = 301;  
    synth.return_internal_address_ = request_address + "/";  
} else {  
    synth.body_file_ = address;  
    synth.return_code_ = 200;  
}
```

###### Synthetic location for index request
At this point, server determines which file should be returned. Server checks index files defined in found location or in parenting ones, or the default `index.html` if nothing were defined at all. More info here:  [index](#index)
Depending on filesystem response status of the directory being requested `fs_status` and of the index file of a particular location - server set's `return_code` and `body_file`:
```c++
if (Utils::CheckFilesystem(index_address) == NOTHING) {  
    // index address not found  
    if (fs_status != DIRECTORY) {  
        // directory, where this index supposed to be doesn't exist  
        std::cout << "\"" + index_address + "\" is not found" << std::endl;  
        synth.return_code_ = 404;  
    } else {  
        // directory exists  but there are no index to return  
        std::cout << "directory index of " + found->root_ +  
                                            "/ is forbidden" << std::endl;  
        synth.return_code_ = 403;  
    }  
} else {  
    // index file found  
    synth.return_code_ = 200;  
    synth.body_file_ = index_address;  
}
```

###### Synthetic location for autoindex request (directory listing)
Creating `ServerResponse` class and sending response back to client.

Just as in case with `ClientRequest` class, `ServerResponse` is intended to contain data, corresponding to different parts of server's response message

![response](https://github.com/r-kupin/webserv/blob/main/notes/response.jpg)

Server creates response in a following way:
1. Composes the top part of the HTTP response, including the status line.
2. Adds standard headers like `Server` and `Date`.
3. Determines the content of the response body based on the `Location` object:
	1. If a custom message is provided, it is used.
	2. If it's an error code:
		1. Checks if a custom error page is defined for the given error code in the `Location` object.
		2. If a custom error page is defined, retrieves and sets it as the response body.
		3. If not, generates a generic error page.
	3. If it's a redirection code:
		1. Generates a redirection page as the response body.
		2. If an external or internal address is provided, sets the `Location` header accordingly.
	4. If a body file is specified, its content is read.
4. Sets additional headers like `Content-Type`, `Content-Length`, and `Connection`.
5. Sends response back to the client.

#  Additional info
## Server response codes implemented
### OK
#### 100 Continue
In case if request's body is large, client might ask server for a confirmation before sending body. Client does it by including `Expect: 100-continue` header in request.
In this case, server will send short message to let client start body upload.
#### 200  OK
This status code is returned when the server successfully processes the request and provides the requested resource. It signifies that the client's request has been fulfilled without any issues.
### Implicit redirect
#### 301 Moved Permanently
This status code is returned when client requests for a static file, but specified address actually points to a directory. In this case, response is also followed by a header `Location`, which value corresponds to request url, followed by '/'
#### 302 Found
If there is a `return http://somewhere.out` directive - response will have 302 code and `Location` header set
### Client side errors
#### 400 Bad Request
If the server cannot process the client's request due to malformed syntax or other errors on the client side, it returns this status code. It indicates that there was an error in the client's request, or method is not *GET*, *POST* or *DELETE*.
#### 403 Forbidden
- If a client has no access to the requested location
- If request method is limited in `limit_except` block
- If a client tries to access directory's index, but the file doesn't exist and `autoindex` is off

#### 404  Not Found
Should I explain it?
#### 405  Method Not Allowed
If client requests location that has `upload_store` set with any method, except of **POST**
#### 413 Payload Too Large
If client intends to send body that exceeds `client_max_body_size` limit
### Server side errors
#### 500 Internal Server Error
- if server requests a file, but what was found is neither a file, nor a directory
- When server fails to perform IO on the socket (It never happens)

#### 501 Not Implemented
If a client tries to upload a file from an unsupported client.
#### 503 Service Unavailable
If a server is unable to create a file intended to store an upload
## Uploads

![one_does_not_simply](https://github.com/r-kupin/webserv/blob/main/notes/one_does_not_simply.jpg)

Typically, no one uses nginx (or any web server) to store files, uploaded by a client on a machine that runs the server. Upload requests are normally being transferred to the  web application's back-end, that decides what to do with the data: save in database, store on the NAS, etc. In summary - server's job is to transfer requests to appropriate back-end and therefore it doesn't have a simple and direct way to handle uploads.
In order to make real nginx store uploaded files, the most intuitive way I found is described below:
1. Download [**nginx-upload-module**](https://www.nginx.com/resources/wiki/modules/upload/) from the official [github page](https://github.com/vkholodkov/nginx-upload-module/tree/master).
2. Add it to installed server following [this guide](https://www.nginx.com/blog/compiling-dynamic-modules-nginx-plus/).
3. Launch server with test configuration.
	```nginx configuration
	server {
		# specify server name, port, root
		location /upload {
			upload_pass   @test;
			upload_store /path/to/upload_directory 1;
		}
		
		location @test {
			return 200 "Hello from test";
		}
	}
	```
4. Use a web client to upload a file. Here's one of the simplest ways: `curl -v -F "file=@file_name" http://server_name:port/upload/ `

As described at module's page:
- [upload_pass](https://www.nginx.com/resources/wiki/modules/upload/#upload-pass "Permalink to this headline"): specifies location to pass request body to. File fields will be stripped and replaced by fields, containig necessary information to handle uploaded files.
- [upload_store](https://www.nginx.com/resources/wiki/modules/upload/#upload-store "Permalink to this headline"): specifies a directory to which output files will be saved to. The directory could be hashed. In this case all subdirectories should exist before starting NGINX.

Important notes:
1. *upload_store* won't work if *upload_pass* is not specified - server will return *403* if there is no index file in uploads directory, or *405* otherwise. Nothing will be stored.
2. In  *upload_directory* subdirectories 0 1 2 3 4 5 6 7 8 9 should exist and be accessible by user, that is used by nginx. I have tested nginx on Debian machine with nginx installed via `apt install`, and the username used by nginx was *www-data*, however, it might be different on other systems and/or if other ways of installation were used. In my case, working *upload_directory* looked like this:
	```
	❯ ls -l
	total 40
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 15:36 0
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 15:37 1
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 15:37 2
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:14 3
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:14 4
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:43 5
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:43 6
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:47 7
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:47 8
	dr-xrwxr-x 2 my_login www-data 4096 Jan 14 16:51 9
	```
 	Otherwise, nginx would be unable to create files to save the uploads and will return *503*
 3. The only request method supported is *POST*
 4. Works in the following way: 
	 1. for each request that posts to *upload_directory* nginx creates file with name which is a numbers of files saved preceded with zeros, so each file being saved has name of 10 characters.
	 2. files are placed in 10 directories, based on the last digit:
		 1. `0000000001`, `0000000021`, etc.. - in `./1`
		 2. `0000000010`, `0000000210`, etc.. -  in `./0`
    3. So the first request's body would be saved to  `./1/0000000001`, second - `./2/0000000002`, etc..