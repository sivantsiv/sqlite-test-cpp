#pragma once

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
	sqlite3 * m_Db;
	Records   m_Records;

public:
	RecordProcessor();
	RecordProcessor(const std::string& filename);

	bool createTable();
	bool insertRecord(const std::string& sql);
	Records& selectRecord(const std::string& sql);
	bool deleteTable();

	~RecordProcessor();
};
