//Async Crud Implementation
var nedgedb = require('nedgedb');

var db =new nedgedb.database("mydsn","pmanyam","");

var collection = new db.collection("customer");


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