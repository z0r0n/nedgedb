/*
NODE ADDON FOR ODBC Connectivity

***********Release Notes**************
->	Built in 
->	Node version : 12.0
->	ODBC driver version : 6.1
**************************************
*/

#include <uv.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#include <v8.h>
#include <node.h>
#include <cstdio>
#include <cstdlib>
#include <sql.h>
#include <sqlext.h>
#include <iostream>

using namespace v8;

SQLHENV	  henv  = SQL_NULL_HENV;   // Environment
SQLHDBC   hdbc  = SQL_NULL_HDBC;   // Connection handle
SQLHSTMT  hstmt = SQL_NULL_HSTMT;  // Statement handle

//	Functions Defined
void connect(const FunctionCallbackInfo<Value>&);
void connectAsync(uv_work_t*);
void connectAfter(uv_work_t*, int);
void connectSync(const FunctionCallbackInfo<Value>&);

void disconnect(const FunctionCallbackInfo<Value>&);
void disconnectAsync(uv_work_t*);
void disconnectAfter(uv_work_t*, int);
void disconnectSync(const FunctionCallbackInfo<Value>&);

void select(const FunctionCallbackInfo<Value>&);
void selectAsync(uv_work_t*);
void selectAfter(uv_work_t*, int);
void selectSync(const FunctionCallbackInfo<Value>&);

void query(const FunctionCallbackInfo<Value>&);
void queryAsync(uv_work_t*);
void queryAfter(uv_work_t*, int);
void querySync(const FunctionCallbackInfo<Value>&);

void extract_error(SQLHANDLE handle, SQLSMALLINT type) {
    SQLSMALLINT i = 0;
    SQLINTEGER native;
    SQLCHAR state[7];
    SQLCHAR text[256];
    SQLSMALLINT len;
    SQLRETURN retcode;
    do
    {
        retcode = SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text), &len );
        if (SQL_SUCCEEDED(retcode)) printf("%s:%d:%d:%s\n", state, i, native, text);
    }
    while( retcode == SQL_SUCCESS );
}

struct connect_data {
	Persistent<Function> cb;
	char dsn[10], uname[20], pass[20];
	int result;
};

struct disconnect_data {
	Persistent<Function> cb;
	int result;
};

struct select_data {
	Persistent<Function> cb;
	char query[1000];
	int result;
	char *records;
	unsigned int limit;
};

struct query_data {
	Persistent<Function> cb;
	char query[1000];
	int result;
};

typedef struct {
	SQLUSMALLINT index;
	unsigned char *name;
	SQLSMALLINT len;
	SQLSMALLINT type;
	SQLULEN size;
} Column;

//  CONNECT ASYNC
void connect(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	unsigned int i;

	// initialize work request
	uv_work_t* work_req = (uv_work_t *) (calloc(1, sizeof(uv_work_t)));

	// initialize our data (baton)
	connect_data* data = (connect_data *) (calloc(1, sizeof(connect_data)));

	// extract argument 1 (DSN-name)
	v8::String::Utf8Value param1(args[0]->ToString());
	std::string dsnRaw = std::string(*param1);
	for(i=0;i<dsnRaw.length();i++) data->dsn[i] = dsnRaw[i]; data->dsn[i] = '\0';

	// extract argument 2 (username)
	v8::String::Utf8Value param2(args[1]->ToString());
	std::string unameRaw = std::string(*param2);
	for(i=0;i<unameRaw.length();i++) data->uname[i] = unameRaw[i]; data->uname[i] = '\0';

	// extract argument 3 (password)
	v8::String::Utf8Value param3(args[2]->ToString());
	std::string passRaw = std::string(*param3);
	for(i=0;i<passRaw.length();i++) data->pass[i] = passRaw[i]; data->pass[i] = '\0';

	// define callback
	Local<Function> callback = Local<Function>::Cast(args[3]);
	data->cb.Reset(isolate, callback);
	
	// link data baton to the data address in work request
	work_req->data = data;
	
	uv_queue_work(uv_default_loop(), work_req, connectAsync, (uv_after_work_cb)connectAfter);
}

void connectAsync(uv_work_t* req) {
	SQLRETURN retcode;

	connect_data* data = (connect_data *)(req->data);
	
	// Allocate a connection handle
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT ALLOCATING CONNECTION HANDLE\n");
		extract_error(hdbc, SQL_HANDLE_DBC);
	}

	// Connect to the data source
	retcode = SQLConnect(hdbc, (SQLCHAR *) data->dsn, SQL_NTS, (SQLCHAR *) data->uname, SQL_NTS, (SQLCHAR *) data->pass, SQL_NTS);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLCONNECT\n");
		extract_error(hdbc, SQL_HANDLE_DBC);
	}
	
	data->result = retcode;
}

void connectAfter(uv_work_t* req, int status) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	connect_data* data = (connect_data *)(req->data);

	Local<Value> argv[1] = { Number::New(isolate, data->result)};
	Local<Function> cb = Local<Function>::New(isolate, data->cb);
	cb->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	free(data);
	free(req);
}

//	CONNECT SYNC
void connectSync(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	SQLCHAR dsn[10], uname[20], pass[20];
	unsigned int i;

	SQLRETURN retcode;

	// extract argument 1 (DSN-name)
	v8::String::Utf8Value param1(args[0]->ToString());
	std::string dsnRaw = std::string(*param1);
	for(i=0;i<dsnRaw.length();i++) dsn[i] = dsnRaw[i]; dsn[i] = '\0';
	
	// extract argument 2 (username)
	v8::String::Utf8Value param2(args[1]->ToString());
	std::string unameRaw = std::string(*param2);
	for(i=0;i<unameRaw.length();i++) uname[i] = unameRaw[i]; uname[i] = '\0';
	
	// extract argument 3 (password)
	v8::String::Utf8Value param3(args[2]->ToString());
	std::string passRaw = std::string(*param3);
	for(i=0;i<passRaw.length();i++) pass[i] = passRaw[i]; pass[i] = '\0';
	
	// define callback
	Local<Function> cb = Local<Function>::Cast(args[3]);
	
	// Allocate a connection handle
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT ALLOCATING CONNECTION HANDLE\n");
		extract_error(hdbc, SQL_HANDLE_DBC);
	}

	// Connect to the data source
	retcode = SQLConnect(hdbc, dsn, SQL_NTS, uname, SQL_NTS, pass, SQL_NTS);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLCONNECT\n");
		extract_error(hdbc, SQL_HANDLE_DBC);
	}
	
	Local<Value> argv[1] = { Number::New(isolate, retcode)};
	cb->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}

//  DISCONNECT ASYNC
void disconnect(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	// define callback
	Local<Function> callback = Local<Function>::Cast(args[0]);

	// initialize work request
	uv_work_t* work_req = (uv_work_t *) (calloc(1, sizeof(uv_work_t)));
  
	// initialize our data (baton)
	disconnect_data* data = (disconnect_data *) (calloc(1, sizeof(disconnect_data)));
	
	data->cb.Reset(isolate, callback);

	work_req->data = data;
	
	uv_queue_work(uv_default_loop(), work_req, disconnectAsync, (uv_after_work_cb)disconnectAfter);
}

void disconnectAsync(uv_work_t* req) {
	SQLRETURN retcode;
	
	disconnect_data* data = (disconnect_data *)(req->data);

    // Free handles
    // Statement
    if (hstmt != SQL_NULL_HSTMT)
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    // Connection
	retcode = SQLDisconnect(hdbc);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLDISCONNECT\n");
		extract_error(hdbc, SQL_HANDLE_DBC);
	}
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
/*
    // Environment
    if (henv != SQL_NULL_HENV)
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
*/	
	data->result = retcode;
}

void disconnectAfter(uv_work_t* req, int status) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	disconnect_data* data = (disconnect_data *)(req->data);

	Local<Value> argv[1] = { Number::New(isolate, data->result)};
	Local<Function> cb = Local<Function>::New(isolate, data->cb);
	cb->Call(isolate->GetCurrentContext()->Global(), 1, argv);

	free(data);
	free(req);
}

//	DISCONNECT SYNC
void disconnectSync(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	SQLRETURN retcode;

	// define callback
	Local<Function> cb = Local<Function>::Cast(args[0]);
	
    // Free handles
    // Statement
    if (hstmt != SQL_NULL_HSTMT)
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

    // Connection
	retcode = SQLDisconnect(hdbc);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLDISCONNECT\n");
		extract_error(hdbc, SQL_HANDLE_DBC);
	}
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
/*
    // Environment
    if (henv != SQL_NULL_HENV)
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
*/	

	Local<Value> argv[1] = { Number::New(isolate, retcode)};
	cb->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}

//  GET COLUMNS FOR TABLE
Column* GetColumns(SQLSMALLINT *colCount) {
	SQLRETURN retcode;
	int i;

	//always reset colCount for the current result set to 0;
	*colCount = 0; 

	//get the number of columns in the result set
	retcode = SQLNumResultCols(hstmt, colCount);

	Column *columns = new Column[*colCount];

	for(i=0;i<*colCount;i++) {
		columns[i].index = i + 1;
		columns[i].name = new unsigned char[200];
		columns[i].name[0] = '\0';

		//get name and type of column
		retcode = SQLDescribeCol( hstmt, columns[i].index, columns[i].name, (SQLSMALLINT) 200, &columns[i].len, &columns[i].type, &columns[i].size, NULL, NULL);
		if(!SQL_SUCCEEDED(retcode)) {
			printf("ERROR WHEN RETRIEVING ERROR\n");	// irony !! :p 
		}
		
		switch(columns[i].type) {
			case SQL_INTEGER :
			case SQL_SMALLINT :
			case SQL_TINYINT :
			case SQL_BIGINT :  columns[i].type = SQL_C_LONG; break;
			case SQL_DOUBLE :
			case SQL_FLOAT :
			case SQL_REAL :	columns[i].type = SQL_C_DOUBLE; break;
			case SQL_VARCHAR :
			case SQL_CHAR :
			case SQL_DECIMAL :
			case SQL_NUMERIC :
			case SQL_LONGVARCHAR : columns[i].type = SQL_C_CHAR; break;
		}
	}
	
//	printf("Datatypes\nSQL_CHAR = %d\nSQL_VARCHAR = %d\nSQL_LONGVARCHAR = %d\nSQL_WCHAR = %d\nSQL_WVARCHAR = %d\nSQL_WLONGVARCHAR = %d\nSQL_DECIMAL = %d\nSQL_NUMERIC = %d\nSQL_SMALLINT = %d\nSQL_INTEGER = %d\nSQL_REAL = %d\nSQL_FLOAT = %d\nSQL_DOUBLE = %d\n\n\n",SQL_CHAR, SQL_VARCHAR, SQL_LONGVARCHAR, SQL_WCHAR, SQL_WVARCHAR, SQL_WLONGVARCHAR, SQL_DECIMAL, SQL_NUMERIC, SQL_SMALLINT, SQL_INTEGER, SQL_REAL, SQL_FLOAT, SQL_DOUBLE);
	
	return columns;
}

//	SELECT ASYNC
void select(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
		
	unsigned int i;
	int temp;

	// initialize work request
	uv_work_t* work_req = (uv_work_t *) (calloc(1, sizeof(uv_work_t)));
  
	// initialize our data (baton)
	select_data* data = (select_data *) (calloc(1, sizeof(select_data)));

	// extract argument 1 (Query)
	v8::String::Utf8Value param1(args[0]->ToString());
	std::string queryRaw = std::string(*param1);
	for(i=0;i<queryRaw.length();i++) data->query[i] = queryRaw[i]; data->query[i] = '\0';
	
	//extracting SQL limit
	temp = (int)args[1]->NumberValue();
	if(temp < 0){
		data->limit = 4294967294;
	}
	else{
		data->limit = temp;
	}
	
	
	// define callback
	Local<Function> callback = Local<Function>::Cast(args[2]);

	data->cb.Reset(isolate, callback);
	
	
	work_req->data = data;
	
	uv_queue_work(uv_default_loop(), work_req, selectAsync, (uv_after_work_cb)selectAfter);
}

void selectAsync(uv_work_t* req) {
	SQLRETURN retcode;	
	SQLSMALLINT colCount = 0;
	SQLLEN indicator;
	SQLINTEGER *intData[30];
	SQLCHAR *charData[30];
	SQLDOUBLE *doubleData[30];
	int i,j,k,l;
	unsigned int m;
	select_data* data = (select_data *)(req->data);
	
	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT ALLOCATING STATEMENT HANDLE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}

	// select statement
	retcode = SQLPrepare(hstmt, (SQLCHAR *)data->query, SQL_NTS);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLPREPARE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}

	// Get Column names and types
	Column* columns = GetColumns(&colCount);
	
	for(i=0;i<colCount;i++) {
		if(columns[i].type == SQL_C_LONG) {
			intData[i] = (SQLINTEGER *) malloc (columns[i].size+1);
			retcode = SQLBindCol(hstmt, columns[i].index, columns[i].type, intData[i], columns[i].size, &indicator);
		}
		else if(columns[i].type == SQL_C_DOUBLE) {
			doubleData[i] = (SQLDOUBLE *) malloc (columns[i].size+1);
			retcode = SQLBindCol(hstmt, columns[i].index, columns[i].type, doubleData[i], columns[i].size, &indicator);
		}
		else if(columns[i].type == SQL_C_CHAR) {
			charData[i] = (SQLCHAR *) malloc (columns[i].size+1);
			retcode = SQLBindCol(hstmt, columns[i].index, columns[i].type, charData[i], columns[i].size, &indicator);
		}
		
		if(!SQL_SUCCEEDED(retcode)){
			printf("ERROR AT BINDING COLUMNS\n");
			extract_error(hstmt, SQL_HANDLE_STMT);
			break;
		}
	}

	// Execute the statement handle
	retcode = SQLExecute (hstmt);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLEXECUTE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}
		
/*	*****Build Comments******
	This was the original implementation for constructing the javascript object (as JSON) for the obtained records,
	similar to the synchronous implementation. But due to the errors popping up by the unwarranted use of *isolate 
	context for making objects, the input records are converted to string and parsed back to JSON.
	
	The *isolate context is used in the following lines marked with *** for convenience. 
	

	// Array of Records
***	data->records = Array::New(data->isolate);
	Local<Object> obj;
	// Fetch and print each row of data until SQL_NO_DATA returned.
	for(i=0;; i++) {
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_NO_DATA) {
			break;
		}
		
		// Individual Records
***		obj = Object::New(data->isolate);
		
		for(j=0;j<colCount;j++) {
			if (columns[j].type == SQL_C_LONG) {
***				obj->Set(String::NewFromUtf8(data->isolate, (CHAR *) columns[j].name), Integer::New(data->isolate, (INT) intData[j]));
			} 
			else if(columns[j].type == SQL_C_CHAR) {
***				obj->Set(String::NewFromUtf8(data->isolate, (CHAR *) columns[j].name), String::NewFromUtf8(data->isolate, (CHAR *) charData[j]));
			}
		}
		data->records->Set(i,obj);
	}
	*/
		
	// A string is created to parse as JSON later
	std::string rec;
	char intConvert[20];	// To convert int to char strings
	char doubleConvert[20];	// To convert double to char strings
	char charConvert[200];	// To accommodate the use of "" double quotes in strings
	
	// Array of Records created in cpp strings to append dynamically
	rec.append("[ ");
	
	// Fetch and print each row of data until SQL_NO_DATA returned.
	for(m=0;m<data->limit; m++) {
		retcode = SQLFetch(hstmt);
		if (retcode == SQL_NO_DATA) {
			break;
		}
		if(!SQL_SUCCEEDED(retcode)){
			printf("ERROR AT SQLFETCH\n");
			extract_error(hstmt, SQL_HANDLE_STMT);
			break;
		}
		
		if(m!=0) {
			rec.append(", ");
		}
		rec.append("{ ");
		// Individual Records
		for(j=0;j<colCount;j++) {
			if (columns[j].type == SQL_C_LONG) {
				if(j!=0) rec.append(", "); 
				rec.append(" \"");
				rec.append((CHAR *) columns[j].name);
				rec.append("\": ");
				sprintf(intConvert, "%ld", (long) *intData[j]);
				rec.append(intConvert);
			} 
			else if (columns[j].type == SQL_C_DOUBLE) {
				if(j!=0) rec.append(", "); 
				rec.append(" \"");
				rec.append((CHAR *) columns[j].name);
				rec.append("\": ");
				sprintf(doubleConvert, "%lf", (double) *doubleData[j]);
				rec.append(intConvert);
			} 
			else if(columns[j].type == SQL_C_CHAR) {
				if(j!=0) rec.append(", "); 
				rec.append(" \"");
				rec.append((CHAR *) columns[j].name);
				rec.append("\": ");
				rec.append(" \"");
				
				k=0;l=0;
				while(charData[j][l]!='\0') {
					if(charData[j][l] == '"') {
						charConvert[k] = '\\';
						k++;
					}
					charConvert[k] = charData[j][l];
					k++;l++;
				}
				charConvert[k]='\0';
				rec.append(charConvert);
				rec.append("\" ");
			}
		}
		rec.append("} ");
	}
	rec.append("] ");
	
	// Convert to char array to be sent to the js
	data->records = (char *)malloc((rec.length()+3)*sizeof(char));
	for(i=0;rec[i]!='\0';i++) {
		data->records[i] = rec[i];
	}
	data->records[i]='\0';
	
	data->result = retcode;
	
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}

void selectAfter(uv_work_t* req, int status) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	select_data* data = (select_data *)(req->data);

	Local<Value> argv[2] = { Number::New(isolate, data->result), String::NewFromUtf8(isolate, data->records) }; 
	Local<Function> cb = Local<Function>::New(isolate, data->cb);
	cb->Call(isolate->GetCurrentContext()->Global(), 2, argv);
	
	free(data->records);
	free(data);
	free(req);
}

//	SELECT SYNC
void selectSync(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	char query[1000];
	unsigned int i,limit;
	int j,temp;
	SQLRETURN retcode;	
	SQLSMALLINT colCount = 0;
	SQLLEN indicator;
	SQLINTEGER *intData[30];
	SQLCHAR *charData[30];
	SQLDOUBLE *doubleData[30];

	
	
	// extract argument 1 (Query)
	v8::String::Utf8Value param1(args[0]->ToString());
	std::string queryRaw = std::string(*param1);
	for(i=0;i<queryRaw.length();i++) query[i] = queryRaw[i]; query[i] = '\0';
	
	//extracting SQL limit
	temp = (int)args[1]->NumberValue();
	if(temp < 0){
		limit = 4294967294;
	}
	else{
		limit = temp;
	}
	
	// define callback
	Local<Function> cb = Local<Function>::Cast(args[2]);
	
	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT ALLOCATING STATEMENT HANDLE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}

	// select statement
	retcode = SQLPrepare(hstmt, (SQLCHAR *)query, SQL_NTS);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLPREPARE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}

	Column* columns = GetColumns(&colCount);

	for(j=0;j<colCount;j++) {
		if(columns[j].type == SQL_C_LONG) {
			intData[j] = (SQLINTEGER *) malloc (columns[j].size+1);
			retcode = SQLBindCol(hstmt, columns[j].index, columns[j].type, intData[j], columns[j].size, &indicator);
		}
		else if(columns[j].type == SQL_C_DOUBLE) {
			doubleData[j] = (SQLDOUBLE *) malloc (columns[j].size+1);
			retcode = SQLBindCol(hstmt, columns[j].index, columns[j].type, doubleData[j], columns[j].size, &indicator);
		}
		else if(columns[j].type == SQL_C_CHAR) {
			charData[j] = (SQLCHAR *) malloc (columns[j].size+1);
			retcode = SQLBindCol(hstmt, columns[j].index, columns[j].type, charData[j], columns[j].size, &indicator);
		}
		
		if(!SQL_SUCCEEDED(retcode)){
			printf("ERROR AT BINDING COLUMNS\n");
			extract_error(hstmt, SQL_HANDLE_STMT);
			break;
		}
	}
	// Execute the statement handle
	retcode = SQLExecute (hstmt);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLEXECUTE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}
	
	// Array of Records
	Local<Array> result = Array::New(isolate);
	Local<Object> obj;

	// Fetch and print each row of data until SQL_NO_DATA returned.
	for(i=0;i<limit; i++) {
		retcode = SQLFetch(hstmt);
		
		if (retcode == SQL_NO_DATA) {
			break;
		}
		if(!SQL_SUCCEEDED(retcode)){
			printf("ERROR AT SQLFETCH\n");
			extract_error(hstmt, SQL_HANDLE_STMT);
			break;
		}
		
		// Retrieve individual Records and store in the object
		obj = Object::New(isolate);
		
		for(j=0;j<colCount;j++) {
			if (columns[j].type == SQL_C_LONG) {
				obj->Set(String::NewFromUtf8(isolate, (char *) columns[j].name), Number::New(isolate, (long) *intData[j]));
			} 
			else if(columns[j].type == SQL_C_DOUBLE) {
				obj->Set(String::NewFromUtf8(isolate, (char *) columns[j].name), Number::New(isolate, (double) *doubleData[j]));
			}
			else if(columns[j].type == SQL_C_CHAR) {
				obj->Set(String::NewFromUtf8(isolate, (char *) columns[j].name), String::NewFromUtf8(isolate, (char *) charData[j]));
			}
		}
		result->Set(i,obj);
	}
	
	Local<Value> argv[2] = { Number::New(isolate, retcode), result };
	cb->Call(isolate->GetCurrentContext()->Global(), 2, argv);
	
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
}

//	INSERT, UPDATE, DELETE ASYNC
void query(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	unsigned int i;

	// initialize work request
	uv_work_t* work_req = (uv_work_t *) (calloc(1, sizeof(uv_work_t)));
  
	// initialize our data (baton)
	query_data* data = (query_data *) (calloc(1, sizeof(query_data)));

	// extract argument 1 (Query)
	v8::String::Utf8Value param1(args[0]->ToString());
	std::string queryRaw = std::string(*param1);
	for(i=0;i<queryRaw.length();i++) data->query[i] = queryRaw[i]; data->query[i] = '\0';
	
	// define callback
	Local<Function> callback = Local<Function>::Cast(args[1]);
	
	data->cb.Reset(isolate, callback);

	work_req->data = data;
	
	uv_queue_work(uv_default_loop(), work_req, queryAsync, (uv_after_work_cb)queryAfter);
}

void queryAsync(uv_work_t* req) {
	SQLRETURN retcode;

	query_data* data = (query_data *)(req->data);
	
	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT ALLOCATING STATEMENT HANDLE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}
	
	// query statement
	retcode = SQLExecDirect(hstmt, (SQLCHAR *)data->query, SQL_NTS);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLEXECDIRECT\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	
	data->result = retcode;
}

void queryAfter(uv_work_t* req, int status) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	query_data* data = (query_data *)(req->data);

	Local<Value> argv[1] = { Number::New(isolate, data->result)};
	Local<Function> cb = Local<Function>::New(isolate, data->cb);
	cb->Call(isolate->GetCurrentContext()->Global(), 1, argv);
	
	free(data);
	free(req);
}

//	INSERT, UPDATE, DELETE SYNC
void querySync(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);
	
	char query[1000];
	unsigned int i;

	// extract argument 1 (Query)
	v8::String::Utf8Value param1(args[0]->ToString());
	std::string queryRaw = std::string(*param1);
	for(i=0;i<queryRaw.length();i++) query[i] = queryRaw[i]; query[i] = '\0';
	
	// define callback
	Local<Function> cb = Local<Function>::Cast(args[1]);
	
	SQLRETURN retcode;

	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT ALLOCATING STATEMENT HANDLE\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}
	
	// query statement
	retcode = SQLExecDirect(hstmt, (SQLCHAR *)query, SQL_NTS);
	if(!SQL_SUCCEEDED(retcode)){
		printf("ERROR AT SQLEXECDIRECT\n");
		extract_error(hstmt, SQL_HANDLE_STMT);
	}
	
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	
	Local<Value> argv[1] = { Number::New(isolate, retcode)};
	cb->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}

void Init(Handle<Object> exports) {
	SQLRETURN retcode;
	
	// Allocate an environment handle
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if(!SQL_SUCCEEDED(retcode)) {
		printf("ERROR ALLOCATING ENVRONMENT HANDLE\n");
		extract_error(henv, SQL_HANDLE_ENV);
	}
	
	// Set the ODBC version environment attribute
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	if(!SQL_SUCCEEDED(retcode)) {
		printf("ERROR SETTING ENVIRONMENT\n");
		extract_error(henv, SQL_HANDLE_ENV);
	}
	
	// define methods called
	NODE_SET_METHOD(exports, "connect", connect);
	NODE_SET_METHOD(exports, "connectSync", connectSync);
	
	NODE_SET_METHOD(exports, "disconnect", disconnect);
	NODE_SET_METHOD(exports, "disconnectSync", disconnectSync);
	
	NODE_SET_METHOD(exports, "select", select);
	NODE_SET_METHOD(exports, "selectSync", selectSync);
	
	NODE_SET_METHOD(exports, "query", query);
	NODE_SET_METHOD(exports, "querySync", querySync);
}

NODE_MODULE(addon, Init)
