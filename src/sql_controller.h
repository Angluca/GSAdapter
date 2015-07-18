#ifndef SQL_CONTROLLER_H
#define SQL_CONTROLLER_H

#include <string>
#include <mysql/mysql.h>

#include "noncopyable.h"
#include "mutex_lock.h"

class SqlController
{
	NONCOPYABLE(SqlController);
public:
	//constructor and destructor
	SqlController(void);
	~SqlController(void);

	/* prototype */
	const std::string& ip() {return ip_;}
	const std::string& user() {return user_;}
	const std::string& pwd() {return pwd_;}
	const std::string& db() {return db_;}

	/* method */
	std::string GetErrorMsg();

	bool Open(const std::string &host, const std::string &user, const std::string &pwd, const std::string &db);
	void Close();
	void ReleaseRes();

	bool Query(const std::string &sql);
	bool Excute(const std::string &sql);

	int RowCount();
	MYSQL_ROW NextRow();
	MYSQL_ROW row() {return sql_row_;}

	MutexLock*  get_lock() {return &lock_;}
	bool KeepConnect();
private:
	MYSQL *sql_conn_;
	MYSQL_RES *sql_res_;
	MYSQL_ROW sql_row_;

	std::string ip_;
	std::string user_;
	std::string pwd_;
	std::string db_;
	MutexLock lock_;

	unsigned long loop_;
};



#endif
