#include "stdafx.h"
#include <chrono>
#include <ctime>
#include <thread>
#include "RecordProcessor.h"

const int dbDelay = 1;

// SQL Engine callback
static int callback(void *pData, int argc, char **argv, char **azColName)
{
	Records* records = static_cast<Records*>(pData);
	try {
		records->emplace_back(argv, argv + argc);
	}
	catch (...) {
		// Abort select on failure, don't let exception propogate thru SQLite3 call-stack
		return 1;
	}
	
	char sdate[9];
	std::ofstream log;
	_strdate_s(sdate);
	std::string filename = "log_";
	filename.append(sdate);
	filename.append(".txt");
	for (unsigned int i = 0; i < filename.length(); ++i)
	{
		if (filename[i] == '/' || filename[i] == ':')
			filename[i] = '-';
	}
	log.open(filename.c_str(), std::ios::out | std::ios::app);
	if (log.fail())
	{
		perror(filename.c_str());
		return 2;
	}

	// Generate timestamp for file logging
	auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	bool static first_run = true;
	if (first_run == true)
	{
		log << ctime(&timenow) << std::endl;
		first_run = false;
	}

	for (int i = 0; i < argc; i++)
	{
		log << (std::string)azColName[i] << " = ";
		if (argv[i])
		{
			log << argv[i];
		}
		else
		{
			log << "NULL";
		}
		log << std::endl;
	}
	log << std::endl;
	log.close();

	return 0;
}

// Default constructor
RecordProcessor::RecordProcessor()
{
	// Open database
	std::cout << "Opening MyDb.db ..." << std::endl;
	if (sqlite3_open("MyDb.db", &m_Db))
	{
		// Failed to open database
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(m_Db) << std::endl << std::endl;
		sqlite3_close(m_Db);
	}
	else
	{
		std::cout << "Opened MyDb.db" << std::endl << std::endl;
	}
}

// Class Constructor
RecordProcessor::RecordProcessor(const std::string& filename)
{
	// Open database
	std::cout << "Opening " << filename << "..." << std::endl;
	if (sqlite3_open(filename.c_str(), &m_Db))
	{
		// Failed to open database
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(m_Db) << std::endl << std::endl;
		sqlite3_close(m_Db);
	}
	else
	{
		std::cout << "Opened SQLite database" << std::endl << std::endl;
	}
}

bool RecordProcessor::createTable()
{
	// Create SQL statement 
	std::string sql =	"CREATE TABLE IF NOT EXISTS RECORDS("  \
						"ID INT PRIMARY KEY     NOT NULL," \
						"NAME           TEXT    NOT NULL," \
						"FILE           TEXT    NOT NULL," \
						"CALLED         INT );";

	// Execute SQL statement 
	char *zErrMsg = 0;
	int rc = sqlite3_exec(m_Db, sql.c_str(), nullptr, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		std::cout << "SQL error: " << static_cast<std::string>(zErrMsg) << std::endl;
		sqlite3_free(zErrMsg);
		return false;
	}

	return true;
}

bool RecordProcessor::deleteTable()
{
	// Create SQL statement 
	std::string sql = "DROP TABLE IF EXISTS RECORDS";

	// Execute SQL statement 
	char *zErrMsg = 0;
	int rc = sqlite3_exec(m_Db, sql.c_str(), nullptr, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		std::cout << "SQL error: " << static_cast<std::string>(zErrMsg) << std::endl;
		sqlite3_free(zErrMsg);
		return false;
	}

	return true;
}

// Insert new database record/records
bool RecordProcessor::insertRecord(const std::string& sql)
{
	// Execute SQL statement 
	char *zErrMsg = 0;
	int rc = sqlite3_exec(m_Db, sql.c_str(), nullptr, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		std::cout << "SQL error: " << static_cast<std::string>(zErrMsg) << std::endl;
		sqlite3_free(zErrMsg);
		return false;
	}

	return true;
}

// Select existing database record/records
Records& RecordProcessor::selectRecord(const std::string& sql)
{
	// Execute SQL statement 
	char *zErrMsg = 0;
	Records& ref = m_Records;
	m_Records.resize(0);
	int rc = sqlite3_exec(m_Db, sql.c_str(), callback, &m_Records, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		std::cout << "SQL error: " << static_cast<std::string>(zErrMsg) << std::endl;
		sqlite3_free(zErrMsg);
	}
	else 
	{
		std::cout << m_Records.size() << " record(s) returned" << std::endl;
	}

	return ref;
}

// Class destructor
RecordProcessor::~RecordProcessor()
{
	// Close Database
	std::cout << "Closing SQLite database ..." << std::endl;
	sqlite3_close(m_Db);
	std::cout << "Closed SQLite database" << std::endl << std::endl;

	// Wait until database is closed
	std::this_thread::sleep_for(std::chrono::seconds(dbDelay));
}
