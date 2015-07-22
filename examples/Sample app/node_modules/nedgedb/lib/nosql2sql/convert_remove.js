var condition;
var stack=[];

function decode(obj){
	var dec = "";
	var i;
	var first_key= Object.keys(obj)[0];
	var col = first_key;
	obj=obj[first_key];
	
	first_key = Object.keys(obj)[0];
	if(typeof(obj [first_key])=="string"){
		obj[first_key] = "'"+obj[first_key]+"'";
	}
	else if(typeof(obj [first_key])=="object"){
		for (i in obj[first_key]){
			if(typeof(obj [first_key][i])=="string"){
				obj[first_key][i] = "'" + obj[first_key][i] +"'";
			}
		}
	}
	if(first_key=="$eq"){
		if(typeof(obj [first_key])=="object"){
			dec = col + " IN " + "(";
			for (i in obj [first_key]){
				dec = dec + obj [first_key][i];
				if(i!=obj [first_key].length-1 ){
					dec = dec + " , ";
				}
			}
			dec = dec+")";
		}
		else{
			dec = "(" + col + "=" +obj [first_key]+")";
		}
	}
	else if(first_key =="$neq"){
		if(typeof(obj [first_key])=="object"){
			dec = "(";
			for (i in obj [first_key]){
				dec = dec + col + "<>" + obj [first_key][i];
				if(i!=obj [first_key].length-1 ){
					dec = dec + " AND ";
				}
			}
			dec = dec+")"
		}
		else{
			dec = "(" + col + "<>" + obj [first_key]+")";
		}
	}
	else if(first_key =="$gt"){
		dec = col + ">" + obj [first_key];
	}
	else if(first_key =="$lt"){
		dec = col + "<" + obj [first_key];
	}
	else if(first_key =="$gte"){
		dec = col + ">=" + obj [first_key];
	}
	else if(first_key =="$lte"){
		dec = col + "<=" + obj [first_key];
	}
	else if(first_key == "$in"){
		dec = col + " BETWEEN " + obj[first_key][0] + " AND " + obj[first_key][1];
	}
	else if(first_key == "$notin"){
		dec = col + "<=" + obj[first_key][0] + " AND " + col + ">=" + obj[first_key][1];
	}
	
	return dec;
}

function traverse(query) {
	condition = condition+"( ";
	var i;
	for (i in query) {
		if(Object.keys(query[i])[0] == "$and"){
			stack.push("AND");
			traverse(query[i].$and);
			
			if(i!=stack.length-1)  condition = condition + stack[stack.length -1] ;
			condition = condition + " ";
		}
		else if(Object.keys(query[i])[0] == "$or"){
			stack.push("OR");
			traverse(query[i].$or);
			
			if(i!=stack.length-1)  condition = condition + stack[stack.length -1] ;
			condition = condition + " ";
		}
		else{
			condition = condition + decode(query[i]);
			if(i!=query.length-1)  condition = condition + stack[stack.length -1] ;
			condition = condition + " ";
		}
	}
	stack.pop();
	condition = condition+" )";
}



var convert = function(tablename,query,schema){
	condition="";
	stack=[];
	var final_query = "DELETE FROM "+schema+tablename+" ";
	var where_exists="";
	
	if(Object.keys(query)[0]){
		where_exists="WHERE";
		if(Object.keys(query)[0] == "$and"){
		stack.push("AND");
		traverse(query.$and);
		}
		else if(Object.keys(query)[0] == "$or"){
			stack.push("OR");
			traverse(query.$or);
		}
		else{
			condition = condition + decode(query);
		}
	}
	final_query = final_query+" "+where_exists+" "+condition;
	//console.log(final_query);
	return final_query;
}

module.exports = convert;