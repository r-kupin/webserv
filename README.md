Minimalist re-implementation of nginx web server.
# HowTo
1. Compile with `make`
2. Launch as `webserv [ path_to_config ]`
3. Connect to the servers on ports defined in the config with any HTTP network-accessing app
# Config
Like `nginx.conf` but with less functional supported. Feel free to consult the test configs provided in `test/test_resources`. 
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
Locations also can be defined implicitly:
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
Location can be empty, or contain following directives:
- *root* (unique)
- *index*
- *return* (unique)
- *error_page*

It can also contain sub-contexts:
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

todo -> If location and all it's super-locations have no root definition at all - server will respond with *500 Internal Server Error*, if access is not restricted, and redirect not set.
##### index
May have multiple args that define files that will be used as an index - meaning - shown when location get's accessed by address. Files are checked in the specified order. The last element of the list can be a file with an absolute path.
Each location has implicit index `index.html` that's being checked if index directive isn't defined explicitly.
```nginx
index index_X.html index_1.html index_2.html;
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
In case, if error page is not defined, or defined file doesn't exist - webserv would auto generate default error page automatically.
Example:
```nginx
error_page 403 404 /error.html;
```
# How it actually works?
## Init
### Arg check
In order to work, server needs a config, which should be passed as a one optional argument. If such argument is present, server will try to create a `Config` object which is intended to store `Node`s, each one dedicated to a particular parameter.
In case if provided address doesn't exist, *isn't readable*, if config made with mistakes or there were no arguments at all - server will try to load a default config, performing the same checks as for the custom one.
## Config parsing
In the runtime all the data stored inside of the object `Config`, which hosts `Nodes`, so all `Config`'s methods are dedicated to store, access and modify them.
In `src/Config/Config.h`
## Setting up servers
## Request handling
## Response creating
