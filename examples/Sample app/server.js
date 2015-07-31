var express = require('express');
var app = express();
var nedgedb = require('nedgedb');
var db =new nedgedb.database("mydsn","pmanyam","");
var bodyparser = require('body-parser');

var collection = db.collection("customer");
var order = { "CustNum":0};

app.use(express.static(__dirname+"/public"));
app.use(bodyparser.json());

app.get('/showrecords',function(req,res){
	var rec_find = collection.find({},[],order);
	res.json(rec_find.records);

});


app.post('/showrecords',function(req,res){
	collection.insert(req.body,function(err){
		if(err){
			throw err;
		}
		else{
  			res.json({});
			console.log("Record Inserted");
		}
	})
});

app.delete('/showrecords/:id',function(req,res){
	var id = req.params.id;
	var query = {CustNum:{"$eq":id}};
	collection.remove(query,function(err){
		if(err){
			throw err;
		}
		else{
	  		res.json({});
			console.log("Record Deleted");
		}
	})
});

app.put('/showrecords/:id', function (req, res) {
  var record = req.body;
  var query = {CustNum:{"$eq":record.CustNum}};
  collection.update(record,query,function(err){
	if(err){
		throw err;
	}
	else{
  		res.json({});
		console.log("Record Updated");
	}
  })
});

var server = app.listen(8888,function(){
	console.log("Connection initializing . . . . ");
	var err = db.connectSync();
	if(err){
		throw err;
	}
	else{
		console.log("Connection established");
	}

	console.log("Server listening at %s", server.address().port);
});	