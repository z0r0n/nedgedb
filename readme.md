# nedgedb
Node module for Progress OpenEdge database with nosql front end interface.

## Description
This node library helps you perform CRUD operations on OpenEdge Database both synchronuosly or asynchronously. The node 
module communicates with the ODBC drivers installed which in turn fetches records from the database. The records are 
fetched as JSON objects. 

## Installation
```bash
npm install nedgedb
```
### For linux 
Requires installed unixODBC
  * Ubuntu/Debian `$ sudo apt-get unixodbc unixodbc-dev`
  * Fedora/Red Hat/CentOS `$ sudo yum install unixODBC unixODBC-devel libtool-ltdl libtool-ltdl-devel`
  * Proper odbc configuration, follow the steps here to correctly configure your odbc installation
    * http://knowledgebase.progress.com/articles/Article/P21252
    
## Sample Programs
#Sync Implementation
```javascript
var nedgedb = require('nedgedb');

var db = new nedgedb.database("dsn_name","user_name","password");

var records=[];
var collection;

db.connectSync(function(err,res){
	if(err){
		throw err;
	}
	else{
		console.log("\nConnected Successfully");
		collection = new db.collection("table_name");
	}
});

var query = { "column1" : { "$eq" : 22222} };
var list = [column1,column2...];
var order = {CustNum : 1};
var limit = 30;

collection.findSync(query,list,order,limit,function(err,res){			
	records.push.apply(records,res);
});
console.log("Total number of records Received = ",records.length);

db.disconnectSync(function(err,res){
	if(err){
		throw err;
	}
	else{
		console.log("Disconnected Successfully\n");
	}
});

```
