Minimalist re-implementation of nginx web server.
# HowTo
## Run
1. Compile with `make`
2. Launch as `webserv [ path_to_config ]`
3. Connect to the servers on ports defined in the config with any HTTP network-accessing app
## Test
1. Prepare test library 
    ```shell
	  git clone git@github.com:google/googletest.git test/lib
	  mkdir test/lib/build && cd test/lib/build 
	  cmake ..
	```
2. Get back to project's root
3. Run `make test`
# Features
## Done
- Choose the [port](#listen) and [host](#server_name) of each server.
- Setup default [error_pages](#error_page).
- Setup routes with one or multiple of the following rules/configuration [return](#return)
	- Define a list of accepted HTTP methods for the route.
	- Define a HTTP redirection.
	- Define a directory or a file from where the file should be searched
- Set a default file to answer if the request is a directory ([index](#index)).
- Make it work with POST and GET methods.
- Your server must be compatible with the web browser of your choice
- Your HTTP response status codes must be accurate.
- You server must have default error pages if none are provided.
- You must be able to serve a fully static website.
- Limit client body size.
## ToDo
- Turn on or off directory listing. (?)
- Your server must be able to listen to multiple ports 
- A request to your server should never hang forever.
- The first server for a host:port will be the default for this host:port (that means it will answer to all the requests that don’t belong to an other server).
- Clients must be able to upload files
- Make the route able to accept uploaded files and configure where they should be save
- Execute CGI based on certain file extension (for example .php).
# Config
Like `nginx.conf` but with less functional supported. This project follows philosophy of forward compatibility - meaning that all valid configs for WebServ will be also valid for NGINX, and will work in exact same way.
Feel free to consult the test configs provided in `test/test_resources`. 
## Config structure
Config consists of **contexts** and **directives**.
### Contexts
Context  is a block defined in a following way:
``` nginx
context_name [ ARG ] {
	...
}
```
For now, webserv supports the following contexts:
#### Server
The main context of the instance of the HTTP server. At least one should be defined in the config. 
Server context can't be empty - it should contain mandatory server-level directives: 
- *server_name* (unique)
- *listen* (unique)

Optional directives:
- *client_max_body_size* (unique)

Server also can predefine root location with optional directives:
- *root* (unique)
- *index*
- *error_page*

Inside server context multiple **location** sub-contexts can be defined, to handle specific requests.
```nginx
server {
	listen 4281;  
    server_name localhost;  
    root /var/www;

	location / { ... }
}
```
#### Location
Location sets configuration depending on a request URI. 
Locations can be defined inside of the server or parent location context (nested locations). Server matches the request URI against all defined location, and then assigns handling to the location with the closest matching *address*.
Location context should be defined with a single argument, which is *address*.
```nginx
location address {
	...
}
```
The *address* - is the absolute path from the **root** location, there are no relative paths. It means, that if location *"loc_n"* should be placed inside location *"/loc_1"* - the address should be defined as follows: *"/loc_1/loc_n"*, regardless of whether the super-context is *location /loc_1*, *location /* or *server*:
```nginx
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
But it can't be defined in any context, apart of the mentioned above:
```nginx
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
```nginx
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
 However one super-location/server can't contain multiple sub-locations with the same addresses:
 ```nginx
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
```nginx
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
Locations can be empty, or contain following directives:
- *root* (unique)
- *index*
- *return* (unique)
- *error_page*
Locations can also contain sub-contexts:
- *limit_except* (unique)
- nested *location*
#### Limit_except
Limits access to location. Defined only inside a location with one or more *HTTP* methods:
```nginx
limit_except METHOD {
	...
}
```
The `METHOD` parameter can be one of the following: `GET`, `POST`,  or  `DELETE`, as subject requires. The *limit_except* is a first thing being checked upon the access to the *location*. If request method is not allowed - server immediately responds with *403 Forbidden*.
Limit_except can't be empty, and should contain following directives:
- *deny*
- *allow*

Depending on intention of prohibiting or allowing access. 
Limit_except can't have any sub-contexts.
### Directives
Directive is a single-line instruction defined in a following way:
```nginx
directive [ ARG1 ] [ ARG... ];
```
#### Server-level directives
##### listen
Has only one *arg* which sets the port, used by the server for requests listening.
##### server_name
Should define server's host name, but only works for *localhost* right now
#### Location-level directives
##### root
Can have only one arg, which is a path for a location, or server's root directory. For example:
```nginx
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
- URI `/loc_1/text.txt` will be handled by path, constructed as `path to executable` + `/loc_1`
- URI `/loc_1/loc_2/text.txt` will be handled by path, constructed as `parrent's root` + `/loc_2`
##### index
May have multiple args that define files that will be used as an index - meaning - shown when location get's accessed by address, following with `/`. Files are checked in the specified order - left to right. The last element of the list can be a file with an absolute path - meaning, path not from the current location's root - but from **root**-location's root.
```nginx
index index_X.html index_1.html index_2.html;
```
Indexes are checked in the following order: 
###### defined in current location
1. Return first index found
2. Return *403* if none of specified files exists & is accessible
###### not defined in current location, but in parent location
Check parent location in the same way, except parent *index filenames* specified in *parent* are expected to be located at the *current location's root*:
```nginx
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
```nginx
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
##### return
Directive, responsible for redirection. Stops processing request and returns the specified code to a client. Should have one or two args.
```nginx
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
- If return has 1 argument, it is should be a `return code` or `address`.  
- If return has 2 arguments, it is should be a `return code` and `address` or `custom message` - depending on the `return code` value.
- There can't be more than 2 args, and `code` can't be the second arg. 
- The redirect, if only address specified, is done with *302* code
##### error_page
Similar to *index* - it is possible to define custom error pages for each location.
Error_page directive expects one or more `error code`(s) followed by a `filename` of the error page, that should be sent to the client in case if one of the specified errors will happen.
In case, if error page is not defined, or defined file doesn't exist - webserv would *auto generate default error page automatically*.
Example:
```nginx
error_page 403 404 /error.html;
```
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
//-------------------redirect related  
    int                     return_code_;  
    std::string             return_internal_address_;  
    std::string             return_external_address_;  
    std::string             return_custom_message_;  
  
    std::string             root_;  
    std::string             full_address_; // address from the root path
    std::string             address_; // particular location's address
    std::string             body_file_; // address of file being sent to client
    l_loc_it                parent_; // root location's "parent" points on itself
    bool                    ghost_;
```
## Setting up servers
## [Request](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages#http_requests) handling
HTTP request is a message sent by a client to a server:

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
-  Last Step in Address (`addr_last_step_`): The contents of the address after the last `/` in URI
- Index Request (`index_request_`): flag indicating whether the request is for the default index resource. WebServ, automatically serves a default file (e.g., index.html) when the path points to a directory meaning if address ends with `/`.
- Fragment (`fragment_`): the fragment identifier, often used in conjunction with anchors in HTML documents. It points to a specific section within the requested resource.
- Parameters (`params_`): additional parameters sent with the request. In the URL, these are typically query parameters (e.g., `?key1=value1&key2=value2`).
### Headers (`headers_`)
HTTP headers provide additional information about the request, such as the type of client making the request, the preferred response format, authentication information, etc.
### Body (`body_`)
The body of the HTTP request, which contains additional data sent to the server. This is particularly relevant for POST requests or other methods where data is sent in the request body.
## [Response](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages#http_responses) creating
Right after the creation of the `ClientRequest` server starts generating response, which involves 2 steps: creating a synthetic location and creating a `ServerResponse` class
### Creating a synthetic location
Depending on compliance between what was requested and what is being found creates a synthetic location - a copy of the location that was found in [ServerConfig](#ServerConfiguration), but with altered return code, and  redirect-related fields, or with a body file set.
In order to determine what should be returned, server performs some checks:
#### Access permission
server checks whether access to requested location is prohibited with [`limit_except`](#Limit_except)
#### Redirection
If access is allowed, server then checks defined [redirection](#return), and bounces client with internal or external redirect, or returns specified code and custom message.
#### File system
If no redirection is defined, server proceeds with checking for the existence of the requested resource.
There are 2 types of requests - for file and for index. If `path` part of the URL ends with `/` - this is an index request, otherwise - file request. That requests are handled differently.
##### Synthetic location for file request
If `path` part of the URL has something after the last `/` symbol, it is assumed that it is a name of the file, that should be located in the root directory of the location, that preceded the filename. Depending on the result of the filesystem check server finishes response location:
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
##### Synthetic location for index request
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
### Creating `ServerResponse` class
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
