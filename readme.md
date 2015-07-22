# Nedgedb
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

## Configuring OpenEdge
Proserve starts the openedge on a specific port number with your username
```bash
proserve "databasename" -S "portnumber" -H "username 
```
Prodb copies a database to your user
```bash
prodb "databasename_to_be_copied" "databasename_to_be_copied_to"
```
## Sample Programs
##### Note : Replace the dsn_name, user_name, password and table_name in the following sample programs with the actual credentials of your openedge database.
### Sync Implementation
```javascript
var nedgedb = require('nedgedb');

var db = new nedgedb.database("dsn_name","user_name","password");  // Fourth attribute is the schema name which is taken as PUB by default.Give an empty string as the fourth attribute for using without any schema name

var records=[];
var collection;

db.connectSync(function(err,res){
	if(err){
		throw err;
	}
	else{
		console.log("\nConnected Successfully");
		collection = db.collection("table_name");
	}
});

var query = { "column1" : { "$eq" : 22222} };
var list = [column1,column2...];
var order = {CustNum : 1};
var limit = 30;
/*
	collection.find()  attributes:
	1.query		(Where clause of SQL given in json similar to mongodb)
	2.list		(All the columns that are to be fetched to be given in an array)
	3.order		(Sorted order of the given records to be given in json similat to mongodb)
	4.limit		(SQL_limit)
	5.callback  (A cursor to records is returned if the callback is not used)
	If the callback is not given it everything in find happens in Sync(similar to findSync)
*/
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
### Async Implementation
```javascript
var nedgedb = require('nedgedb');
var db =new nedgedb.database("dsn_name","user_name","password");

var collection = db.collection("table_name");

db.connect(function(err){
	if(err){
		throw err;
	}
	else{
		console.log("connected Successfully\n");
		/*
			collection.find()  attributes:
				1.query		(Where clause of SQL given in json similar to mongodb)
				2.list		(All the columns that are to be fetched to be given in an array)
				3.order		(Sorted order of the given records to be given in json similat to mongodb)
				4.limit		(SQL_limit)
				5.callback  (A cursor to records is returned if the callback is not used)
				If the callback is not given it everything in find happens in Sync(similar to findSync)
				
			If query is an empty json, all records are selected ans similarly for list
			If order is an empty json, the sotring will be based on primary key
			If the limit is negative then all the records are fetched
		*/
		collection.find({},[],{},-1,function(err,res){
			if(err){
				throw err;
			}
			else{
				console.log("Length of received Result = ",res.length,"\n");
			}
			db.disconnect(function(err,res){
				if(err){
					throw err;
				}
				else{
					console.log("Disconnected Successfully\n");
				}
			})
			console.log("Disconnect is Async\n");
		});
		
		console.log("Find is Async\n");
	}
});

console.log("\nConnect is Async\n");
```
### Async CRUD Implementation
```javascript
//Async Crud Implementation
var nedgedb = require('nedgedb');

var db =new nedgedb.database("dsn_name","user_name","password");

var collection = db.collection("table_name");


query = { "CustNum" : { "$eq" : 22222}  };

var record = {
	CustNum : '22222',
	Name : 'puneeth'
}

var record2 = {
	CustNum : 22222,
	Name : "Raju"
}

var list = [ "CustNum","Name"];
var order = {CustNum : 1};

db.connect(function(err){
	if(err){
		throw err;
	}
	else{
		console.log("Connected Successfully\n");
	}
	var res = collection.find(query,list,order);
	if(res.err){
		throw res.err;
	}
	else{
		console.log("Records with custnum 22222 => ",res.records,"\n\n");
	}
	/*
		collection.insert("record to be inserted",callback(err))
	*/
	collection.insert(record,function(err){   
		if(err){
			throw err;
		}
		else{
			console.log("New Record Inserted\n");
		}
		
		res = collection.find(query,list,order);
		if(res.err){
			throw res.err;
		}
		else{
			console.log("Records with custnum 22222 => ",res.records,"\n\n");
		}
	
		/*
			collection.update((record to be inserted),(condition for selecting which records are to be updated),callback(err))
		*/
		collection.update(record2,query,function(err){
			if(err){
				throw err;
			}
			else{
				console.log("Record Updated\n");
			}
			res = collection.find(query,list,order);
			if(res.err){
				throw res.err;
			}
			else{
				console.log("Records with custnum 22222 => ",res.records,"\n\n");
			}
			
			/*
				collection.remove((condition for selecting which records are to be deleted),callback(err))
			*/
			collection.remove(query,function(err){
				if(err){
					throw err;
				}
				else{
					console.log("Record Deleted\n");
				}
				res = collection.find(query,list,order);
				if(res.err){
					throw res.err;
				}
				else{
					console.log("Records with custnum 22222 => ",res.records,"\n\n");
				}
				db.disconnect(function(err){
					if(err){
						throw err;
					}
					else{
						console.log("Disconnected Successfully\n");
					}
				});
				
				console.log("\n\nDisconnecting Asynchronously\n");
			});
			console.log("\n\nDeleting Asynchronously\n");
		});
		console.log("\n\nUpdating Asynchronously\n");
	});
	console.log("\n\nInserting Asynchronously\n");
});
console.log("\nConnecting Asynchronously\n");
```
###Cursor Usage
```javascript
//Cursor Implementation
var nedgedb = require('nedgedb');

var db = new nedgedb.database("dsn_name","user_name","password");

var collection = db.collection("table_name");

var print_res = function(err,res){
	if(err){
		throw err;
	}
	else{
		console.log(res.CustNum);
		console.log();
	}
}

db.connectSync(function(err){
	if(err){
		throw err;
	}
	else{
		console.log("\nConnected Successfully\n");
	}
});

/*
	If the callback was not provided for the collection.find or collection.findSync a cursor to the records is returned
	Cursor functionalities:
	--> cursor.err => Error in fetching records
	--> cursor.cur_record => Current record number(-1  if no records are fetched)
	--> cursor.records => Fetched Records in an array
	--> cursor.len => No.of records fetched
	--> cursor.record(callback(err,res)) gives the present record in res and err in accessing the record in err.
			err = "No_Records" if no records are fetched.
				= "End_Of_Records" if present record number was more than the number of fetched records
				= "Negative" if a negative record number was attempted to access
				= error in fetching otherwise
				= 0 if everything was fine
	--> cursor.next(offset) skips offset number of records. offset can be +ve or -ve. If no offset is given a default of 1 is taken
	--> cursor.rewind() rewinds the cursor
	--> cursor.end() takes the cursor to end
*/
var cursor = collection.findSync({},[],{});

cursor.record(print_res);

cursor.next();
cursor.record(print_res);

cursor.rewind();
cursor.record(print_res);

cursor.end();
cursor.record(print_res);

cursor.rewind();
cursor.next(4);
cursor.record(print_res);


db.disconnectSync(function(err){
	if(err){
		throw err;
	}
	else{
		console.log("\nDisconnectred Successfully\n");
	}
});
```
