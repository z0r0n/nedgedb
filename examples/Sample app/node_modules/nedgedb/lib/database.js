var addon = require('../build/Release/addon');

var collection = require('./collection.js');

var database = function(dsn_name,username,password,schema){
	this.dsn_name=dsn_name;
	this.username=username;
	this.password=password;
	this.schema="";
	if(!schema && schema!=""){
		this.schema="PUB.";
	}
	else if(schema!=""){
		this.schema = schema+".";
	}
}

database.prototype.connect = function(callback) {
	addon.connect(this.dsn_name, this.username, this.password,function(err){
		if(err == 0 || err == 1) {
			err = 0;
		}
		else{
			err="ERROR: CANNOT CONNECT"
		}
		if(callback){
			callback(err);
		}
	});
}

database.prototype.connectSync = function(callback) {
	addon.connectSync(this.dsn_name, this.username, this.password,function(err){
		if(err == 0 || err == 1) {
			err = 0;
		}
		else{
			err="ERROR: CANNOT CONNECT"
		}
		if(callback){
			callback(err);
		}
	});
}

database.prototype.collection = function(tablename){
	return new collection(tablename,this.schema);
};

database.prototype.disconnect = function(callback) {
	addon.disconnect(function(err) {
		if(err == 0 || err == 1) {
			err = 0;
		}
		else {
			err = "ERROR: AT DISCONNECT";
		}
		
		if(callback) {
			callback(err);
		}
	});
}


database.prototype.disconnectSync = function(callback) {
	addon.disconnectSync(function(err) {
		if(err == 0 || err == 1) {
			err = 0;
		}
		else {
			err = "ERROR: AT DISCONNECT";
		}
		
		if(callback) {
			callback(err);
		}
	});
}

module.exports = database;