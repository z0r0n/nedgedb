//Cursor Implementation
var nedgedb = require('nedgedb');

var db = new nedgedb.database("mydsn","pmanyam","");

var collection = new db.collection("customer");

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
