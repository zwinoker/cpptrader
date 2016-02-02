/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include <mysql.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

int main(void)
{

MYSQL mysql;

mysql_init(&mysql);
mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"your_prog_name");
if (!mysql_real_connect(&mysql,"localhost","root","","stockmarket",0,NULL,0))
{
    fprintf(stderr, "Failed to connect to database: Error: %s\n",
          mysql_error(&mysql));
}


MYSQL_ROW row;
MYSQL_RES *result;


const char *testquery = "select * from AAPL_";

mysql_real_query(&mysql, testquery, strlen(testquery));

unsigned int num_fields;
unsigned int i;

result = mysql_use_result(&mysql);

num_fields = mysql_num_fields(result);
while ((row = mysql_fetch_row(result)))
{
   unsigned long *lengths;
   lengths = mysql_fetch_lengths(result);
   for(i = 0; i < num_fields; i++)
   {
       printf("[%.*s] ", (int) lengths[i],
              row[i] ? row[i] : "NULL");
   }
   printf("\n");
}

mysql_free_result(result);
mysql_close(&mysql);


// try {
//   sql::Driver *driver;
//   sql::Connection *con;
//   sql::Statement *stmt;
//   sql::ResultSet *res;

//   /* Create a connection */
//   driver = get_driver_instance();
//   cout << "test1" << endl;
//   con = driver->connect("tcp://127.0.0.1:3306", "root", "star4wars");
//   cout << "test2" << endl;
//   /* Connect to the MySQL test database */
//   con->setSchema("test");

//   stmt = con->createStatement();
//   res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
//   while (res->next()) {
//     cout << "\t... MySQL replies: ";
//      Access column data by alias or column name 
//     cout << res->getString("_message") << endl;
//     cout << "\t... MySQL says it again: ";
//     /* Access column fata by numeric offset, 1 is the first column */
//     cout << res->getString(1) << endl;
//   }
//   delete res;
//   delete stmt;
//   delete con;

// } catch (sql::SQLException &e) {
//   cout << "# ERR: SQLException in " << __FILE__;
//   cout << "(" << __FUNCTION__ << ") on line "
//      << __LINE__ << endl;
//   cout << "# ERR: " << e.what();
//   cout << " (MySQL error code: " << e.getErrorCode();
//   cout << ", SQLState: " << e.getSQLState() << " )" << endl;
// }

// cout << endl;

return EXIT_SUCCESS;
}
