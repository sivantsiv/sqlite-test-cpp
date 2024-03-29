// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <sstream>
#include <chrono>
#include <thread>
#include "RecordProcessor.h"
#include "cpptest\cpptest.h"

const int dataID = 42;
const int userDelay = 5;

// Unit tests implementation
class dbTest : public Test::Suite 
{
	// Use case 1 - retrieve a pattern call with a specified identifier, e.g. 42
	void test_select_by_id()      
	{ 
		Records selected = processor.selectRecord("SELECT * FROM RECORDS WHERE ID = 42");
		if (selected.size() != 0)
		{
			std::stringstream sstream(selected[0][0]);
			int id = 0;
			sstream >> id;
			TEST_ASSERT(id == dataID);
		}
		else
		{
			// Test failed, nothing was found
			TEST_ASSERT(0 == 1);
		}
	}

	// Use case 2 - list all pattern calls with a specified name, e.g. “myPattern”
	void test_select_by_name()    
	{ 
		std::string name = "myPattern";

		Records selected = processor.selectRecord("SELECT * FROM RECORDS WHERE NAME='myPattern'");
		if (selected.size() == 2)
		{
			TEST_ASSERT(!name.compare(selected[0][1]));
			TEST_ASSERT(!name.compare(selected[1][1]));
		}
		else
		{
			// Test failed, nothing was found
			TEST_ASSERT(0 == 1);
		}
	}

	// Use case 3 - list all pattern calls with a specified path, e.g. “src/patterns/Functional.pat”
	void test_select_by_file()    
	{
		std::string file_path = "src/patterns/Functional.pat";

		Records selected = processor.selectRecord("SELECT * FROM RECORDS WHERE FILE='src/patterns/Functional.pat'");
		if (selected.size() == 2)
		{
			TEST_ASSERT(!file_path.compare(selected[0][2]));
			TEST_ASSERT(!file_path.compare(selected[1][2]));
		}
		else
		{
			// Test failed, nothing was found
			TEST_ASSERT(0 == 1);
		}
	}

	// Use case 4 - list all pattern calls which are skipped, i. e. when the “called” flag is false
	void test_select_called()     
	{
		Records selected = processor.selectRecord("SELECT * FROM RECORDS WHERE CALLED = 1");
		if (selected.size() == 2)
		{
			std::stringstream sstream(selected[0][3]);
			int called = 0;
			sstream >> called;
			TEST_ASSERT(called == 1);

			sstream.str(selected[1][3]);
			sstream >> called;
			TEST_ASSERT(called == 1);
		}
		else
		{
			// Test failed, nothing was found
			TEST_ASSERT(0 == 1);
		}
	}

	// Use case 5 - list all pattern calls which are not skipped, i. e. when the “called” flag is true
	void test_select_not_called() 
	{
		Records selected = processor.selectRecord("SELECT * FROM RECORDS WHERE CALLED = 0");
		if (selected.size() == 2)
		{
			std::stringstream sstream(selected[0][3]);
			int called = 0;
			sstream >> called;
			TEST_ASSERT(called == 0);

			sstream.str(selected[1][3]);
			sstream >> called;
			TEST_ASSERT(called == 0);
		}
		else
		{
			// Test failed, nothing was found
			TEST_ASSERT(0 == 1);
		}
	}

public:
	dbTest() 
	{ 
		TEST_ADD(dbTest::test_select_by_id); 
		TEST_ADD(dbTest::test_select_by_name);
		TEST_ADD(dbTest::test_select_by_file);
		TEST_ADD(dbTest::test_select_called);
		TEST_ADD(dbTest::test_select_not_called);
	}

protected:
	RecordProcessor processor; 

	virtual void setup() 
	{ 
		// Setup initial state of the database/table/records
		processor.createTable();
		processor.insertRecord("INSERT INTO RECORDS (ID,NAME,FILE,CALLED) "  \
							   "VALUES (42, 'myPattern', 'src/patterns/Functional.pat', 0 ); ");
		processor.insertRecord("INSERT INTO RECORDS (ID,NAME,FILE,CALLED) "  \
							   "VALUES (43, 'myPattern', 'src/patterns/Functional.pat', 0 ); ");
		processor.insertRecord("INSERT INTO RECORDS (ID,NAME,FILE,CALLED) "  \
							   "VALUES (85, 'srcPattern', 'src/patterns/NoneFunctional.pat', 1 ); ");
		processor.insertRecord("INSERT INTO RECORDS (ID,NAME,FILE,CALLED) "  \
							   "VALUES (86, 'thePattern', 'src/patterns/NoneFunctional.pat', 1 ); ");
	};

	// Release allocate resources/clear test data
	virtual void tear_down() 
	{
		processor.deleteTable();
	};
};

// Testing RecordProcessor class
int main()
{
	dbTest unit_tests;
	Test::TextOutput output(Test::TextOutput::Verbose);
	unit_tests.run(output);
	// Allow the user to see unit tests output - optional
	std::this_thread::sleep_for(std::chrono::seconds(userDelay));

	return 0;
}
