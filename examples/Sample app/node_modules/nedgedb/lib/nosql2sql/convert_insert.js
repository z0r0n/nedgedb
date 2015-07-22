var convert = function(tablename,record,schema){
	var keys="";
	var values="";
	var final_query = "";
	var cnt=0;
	for (i in record){
		cnt++;
		keys = keys + i;
		values = values +"'"+ record[i]+"'";
		if(Object.keys(record)[cnt]){
			keys = keys +', ';
			values = values + ', ';
		}
	}	
	final_query = "INSERT INTO "+schema + tablename + " (" + keys + ") VALUES (" + values + ")";
	//console.log(final_query);
	return final_query;
}

module.exports = convert;