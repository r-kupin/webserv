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

