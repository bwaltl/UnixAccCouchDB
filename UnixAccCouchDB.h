/*
 * accCouchDBC.h
 *
 *  Created on: Nov 11, 2012
 *      Author: wtl
 */


#ifndef ACCCOUCHDBC_H_
#define ACCCOUCHDBC_H_

#include <extcode.h>

#ifdef __cplusplus
extern "C" {
#endif

int main(void);

int createDB(const char* server);
int writeValue2DB(const char* server, const char* key, int value);
int readInt(const char* server, const char* key);
int initChangesFeed();
int SendEvent(LVUserEventRef *rwer);
int Abort();
//int callback(pt_node_t* node);
//int callback_non_cont(pt_node_t* node);

//handles the register process of a labview device
int registerAtCouchDB(const char* server, const char* name, const char* title, int* types, int types_count );
int insertStringData(const char* server, char* source, const char* data);
int insertStringNotification(const char* server, char* source, const char* data);


#ifdef __cplusplus
}
#endif

void PopulateStringHandle(LStrHandle lvStringHandle,char* stringData);
void PopulateIntHandle(LStrHandle lvStringHandle,int stringData);
void writeInt2LogFile(char* msg, int value);
void writeChar2LogFile(char* data);
void log_stringMessage(const char* message, const char* value);
void log_intMessage(const char* message, const int value);
char* getTimestamp();
char* getTimestampasID();

#endif /* ACCCOUCHDBC_H_ */
