//Async functionallity
var nedgedb = require('nedgedb');
var db =new nedgedb.database("mydsn","pmanyam","");

var collection = db.collection("customer");

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
		*/
		collection.find({},[],{},100,function(err,res){
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