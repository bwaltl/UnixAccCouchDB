/*
 * accCouchDB.cpp
 *
 *  Created on: Nov 7, 2012
 *      Author: wtl
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pillowtalk.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "UnixAccCouchDB.h"

#define STRING_LENGTH 256
#define LOGGING_ENABLED 1
#define OPERATION_FAILED 0
#define OPERATION_SUCCEDED 1
#define LOGGING_PATH "/home/wtl/Documents/log.txt"

static volatile int gNumberOfHeartbeats = 0;
int recValue = 0;
LVUserEventRef* rwer;
char dbPath[256];

int main(void) {
	return 1;
}

//___________________________________________________________________________
/** Register new device in CouchDB */
int registerAtCouchDB(const char* server, const char* name, const char* title,
		int* types, int types_count) {

	log_stringMessage("Register new device", name);

	pt_init();

	// build types array
	pt_node_t* types_array = pt_array_new();
	int i = 0;
	for (i = 0; i < types_count; i++) {
		pt_array_push_back(types_array, pt_integer_new(types[i]));
	}

	log_stringMessage("Type array built", name);
	//create a new document
	pt_node_t* root = pt_map_new();

	pt_map_set(root, "title", pt_string_new(title));
	pt_map_set(root, "_id", pt_string_new(name));
	pt_map_set(root, "name", pt_string_new(name));
	char* timestamp = getTimestamp();
	pt_map_set(root, "timestamp", pt_string_new(timestamp));
	pt_map_set(root, "types", types_array);

	char* keyPath = (char*) malloc(
			sizeof(char) * (strlen(server) + strlen(name) + 1));
	sprintf(keyPath, "%s/%s", server, name);
	log_stringMessage("Insert node", keyPath);

	pt_response_t* response = NULL;
	response = pt_put(keyPath, root);
	if (response->response_code != 201) {
		log_stringMessage("Device register failed", keyPath);
		log_intMessage("Device register failed", response->response_code);
		return OPERATION_FAILED;
	}

	free(timestamp);
	pt_free_response(response);
	pt_free_node(root);

	pt_cleanup();
	log_stringMessage("New device successfully registered", name);
	return OPERATION_SUCCEDED;
}

//___________________________________________________________________________
/** Insert new data document in CouchDB */
int insertStringData(const char* server, char* source, const char* data) {

	char message[256];
	sprintf(message, "%s is inserting data", source);
	log_stringMessage(message, data);

	pt_init();

	//create a new document
	pt_node_t* root = pt_map_new();

	pt_map_set(root, "source", pt_string_new(source));
	char* timestamp = getTimestamp();
	pt_map_set(root, "timestamp", pt_string_new(timestamp));
	pt_map_set(root, "data", pt_string_new(data));
	char id[STRING_LENGTH];
	sprintf(id, "data_%s%s", source, getTimestampasID());
	pt_map_set(root, "_id", pt_string_new(id));

	char* keyPath = (char*) malloc(
			sizeof(char) * (strlen(server) + strlen(id) + 1));
	sprintf(keyPath, "%s/%s", server, id);
	log_stringMessage("keyPath", id);

	pt_response_t* response = NULL;
	response = pt_put(keyPath, root);
	if (response->response_code != 201) {
		log_stringMessage("Data insertion failed", keyPath);
		log_intMessage("Data insertion failed", response->response_code);
		return OPERATION_FAILED;
	}

	free(timestamp);
	pt_free_response(response);
	pt_free_node(root);

	pt_cleanup();
	return OPERATION_SUCCEDED;
}

/** Insert new data document in CouchDB */
int insertStringNotification(const char* server, char* source, const char* data) {

	char message[256];
	sprintf(message, "%s is inserting data", source);
	log_stringMessage(message, data);

	pt_init();

	//create a new document
	pt_node_t* root = pt_map_new();

	pt_map_set(root, "source", pt_string_new(source));
	char* timestamp = getTimestamp();
	pt_map_set(root, "timestamp", pt_string_new(timestamp));
	pt_map_set(root, "data", pt_string_new(data));
	char id[STRING_LENGTH];
	sprintf(id, "notification/%s%s", source, getTimestampasID());
	pt_map_set(root, "_id", pt_string_new(id));

	char* keyPath = (char*) malloc(
			sizeof(char) * (strlen(server) + strlen(id) + 1));
	sprintf(keyPath, "%s/%s", server, id);

	pt_response_t* response = NULL;
	response = pt_put(keyPath, // insert key,value - pair
			root);
	if (response->response_code != 201) {
		log_stringMessage("Data insertion failed", keyPath);
		log_intMessage("Data insertion failed", response->response_code);
		return OPERATION_FAILED;
	}

	free(timestamp);
	pt_free_response(response);
	pt_free_node(root);

	pt_cleanup();
	return OPERATION_SUCCEDED;
}

//___________________________________________________________________________
/** Create Couch Database */
int createDB(const char* server) {

	pt_init();

	log_stringMessage("Creating database", server);

	pt_response_t* response = NULL;
	response = pt_delete(server); // delete DB
	pt_free_response(response);
	response = pt_put(server, NULL ); // create DB
	assert(response->response_code == 201);
	pt_free_response(response);

	pt_cleanup();
	return 1;
}

int writeValue2DB(const char* server, const char* key, int value) {

	char* keyPath = (char*) malloc(
			sizeof(char) * (strlen(server) + strlen(key) + 1));
	sprintf(keyPath, "%s/%s", server, key);

	log_stringMessage("Writing value to db", key);
	log_intMessage("Writing value to db", value);

	pt_init();

	//create key value pair
	pt_node_t* root = pt_map_new();

	pt_map_set(root, "_id", pt_string_new(key));
	pt_map_set(root, "data", pt_integer_new(value));

	pt_response_t* response = NULL;
	response = pt_put(keyPath, // insert key,value - pair
			root);
	assert(response->response_code == 201);
	pt_free_response(response);
	pt_free_node(root);

	free(keyPath);

	pt_cleanup();
	return 1;
}

int readInt(const char* server, const char* key) {
	char* keyPath = (char*) malloc(
			sizeof(char) * (strlen(server) + strlen(key) + 1));
	sprintf(keyPath, "%s/%s", server, key);

	pt_response_t* response = NULL;

	response = pt_get(keyPath);
	assert(response->response_code == 200);

	pt_node_t* doc = response->root;
	const char* id = pt_string_get(pt_map_get(doc, "_id"));
	assert(!strcmp(id, key));

	int value = pt_integer_get((pt_map_get(doc, "value")));

	pt_free_response(response);

	pt_cleanup();

	return value;
}

//___________________________________________________________________________
/** Callback functions */
int callback_non_cont(pt_node_t* node) {
	pt_printout(node, " ");
	//cout << endl;
	return 0;
}

int callback_send(char* value) {
	LStrHandle newStringHandle;
	//Allocate memory for a LabVIEW string handle using LabVIEW's
	//memory manager functions.
	newStringHandle = (LStrHandle) DSNewHandle(
			strlen(value) + STRING_LENGTH * sizeof(uChar));
	PopulateStringHandle(newStringHandle, value);

	//Post event to Event structure. Refer to "Using External Code
	//with LabVIEW manual for information about this function.
	PostLVUserEvent(*rwer, (void *) &newStringHandle);

	return 1;
}

int callback(pt_node_t* node) {
	if (pt_is_null(node)) {
		//log_intMessage("Heartbeat received", gNumberOfHeartbeats);
		if (gNumberOfHeartbeats == -2) {
			log_stringMessage("Quitting callback after heartbeat", "-2");
			return -1;
		}
		return 0;
	} else {
	}
	gNumberOfHeartbeats = 0;
	char document[STRING_LENGTH];
	sprintf(document, "%s", dbPath);
	pt_printout(node, " ");
	const char* astr = pt_string_get(pt_map_get(node, "id"));
	char* fullDoc = document;
	if (astr) {
		if (strncmp(astr, "notification_", strlen("notification_") != 0))
				return 1;

		log_stringMessage("Received notification", astr);
		fullDoc = (char*) malloc(
				sizeof(char) * (strlen(document) + strlen(astr) + 1));
		sprintf(fullDoc, "%s%s\0", document, astr);
	}
	log_stringMessage("Callback invoked", fullDoc);
	pt_response_t* temp = pt_get(fullDoc);
	char* value = pt_string_get(((pt_map_get(temp->root, "data"))));
	log_stringMessage("Callback invoked", value);

	if (value == NULL )
		return 1;

	callback_send(value);
	pt_free_response(temp);
	if (astr) {
		free(fullDoc);
	}
	return 1;
}

int Abort() {
	gNumberOfHeartbeats = -2;
	log_intMessage("Aborting changes feed", gNumberOfHeartbeats);
	return 1;
}

int initChangesFeed(LVUserEventRef* local_rwer, char* server, char* database) {
	//cout << "starting...";

	pt_init();
	pt_changes_feed cf = pt_changes_feed_alloc();
	gNumberOfHeartbeats = 0;

	log_stringMessage("Initializing changes feed", database);
	sprintf(dbPath, "%s/%s/", server, database);

	rwer = local_rwer;
	pt_changes_feed_config(cf, pt_changes_feed_continuous, 1);
	pt_changes_feed_config(cf, pt_changes_feed_req_heartbeats, 1000);
	pt_changes_feed_config(cf, pt_changes_feed_callback_function, &callback);
	pt_changes_feed_run(cf, server, database);

	pt_changes_feed_free(cf);
	pt_cleanup();
	return 1;
}

//___________________________________________________________________________
/** Utility functions */

char* getTimestamp() {
	time_t t = time(NULL );
	struct tm tm = *localtime(&t);

	char* datetime = (char*) malloc(sizeof(char) * 256);

	sprintf(datetime, "%04d-%02d-%02dT%02d:%02d:%02d", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec); /*writes*/

	return datetime;
}

char* getTimestampasID() {
	time_t t = time(NULL );
	struct tm tm = *localtime(&t);

	char* datetime = (char*) malloc(sizeof(char) * 256);

	sprintf(datetime, "%04d%02d%02dT%02d%02d%02d", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec); /*writes*/

	return datetime;
}

//___________________________________________________________________________
/** String manipulation */
void PopulateStringHandle(LStrHandle lvStringHandle, char* stringData) {
	//Empties the buffer
	memset(LStrBuf(*lvStringHandle), '\0', STRING_LENGTH);

	//Fills the string buffer with stringData
			sprintf((char*) LStrBuf(*lvStringHandle), "%s", stringData);

	//Informs the LabVIEW string handle about the size of the size
			LStrLen (*lvStringHandle) = strlen(stringData);

			return;
		}

void PopulateIntHandle(LStrHandle lvStringHandle, int stringData) {
	//Empties the buffer
	memset(LStrBuf(*lvStringHandle), '\0', STRING_LENGTH);

	//Fills the string buffer with stringData
			sprintf((char*) LStrBuf(*lvStringHandle), "%d", stringData);

	//Informs the LabVIEW string handle about the size of the size
			LStrLen (*lvStringHandle) = strlen((char*) LStrBuf(*lvStringHandle));

			return;
		}

//___________________________________________________________________________
	/** Write to Logfile */
void log_stringMessage(const char* message, const char* value) {
	if (LOGGING_ENABLED == 0)
		return;

	time_t t = time(NULL );
	struct tm tm = *localtime(&t);

	FILE *file;
	file = fopen(LOGGING_PATH, "a+");

	fprintf(file, "%d-%d-%d %d:%d:%d -- %s: %s\r\n", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
			message, value); /*writes*/
	fclose(file); /*done!*/

}

void log_intMessage(const char* message, const int value) {
	if (LOGGING_ENABLED == 0)
		return;

	time_t t = time(NULL );
	struct tm tm = *localtime(&t);

	FILE *file;
	file = fopen(LOGGING_PATH, "a+");

	fprintf(file, "%d-%d-%d %d:%d:%d -- %s: %d\r\n", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
			message, value); /*writes*/
	fclose(file); /*done!*/

}
