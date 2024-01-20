#ifndef RECORDPROCESSOR_H
#define RECORDPROCESSOR_H

#include <windows.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include "sqlite3\sqlite3.h"

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

class RecordProcessor
{
private:
	sqlite3 * p_db;
	Records   records;

public:
	RecordProcessor();
	RecordProcessor(const char* filename);

	bool createTable();
	bool deleteTable();
	bool insertRecord(const char* sql);
	Records& selectRecord(const char* sql);

	~RecordProcessor();
};

#endif