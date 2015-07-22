var cursor = function(err,res){
	this.err=err;
	this.cur_record=0;
	this.records=res;
	this.len=res.length;
	if(this.len==0){
		this.cur_record=-1;
	}
}

cursor.prototype.record = function(callback) {
	var err;
	if(this.len==0){
		if(this.err){
			err = this.err;
		}
		else{
			err = "No_Records";
		}
	}
	else if(this.cur_record>=this.len){
		err = 'End_Of_Records';
	}
	else if(this.cur_record<0){
		err='Negative';
	}
	else{
		err = this.err;
	}
	
	if(err)  {
		callback(err,0);
	}
	else{
		callback(err,this.records[this.cur_record]);
	}
}

cursor.prototype.next = function(offset){
	if(offset || offset==0){
		if(this.cur_record<this.len){
			this.cur_record+=offset;
		}
	}
	else{
		this.cur_record++;
	}
}

cursor.prototype.rewind = function(){
	this.cur_record=0;
	if(this.len==0){
		this.cur_record=-1;
	}
}

cursor.prototype.end = function(){
	this.cur_record=this.len - 1;
}

module.exports = cursor;