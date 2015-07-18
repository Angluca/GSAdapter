#include "sql_controller.h"
#include "utils.h"

#include <mysql/mysql.h>
#include <stdio.h>



//constructor
SqlController::SqlController(void):
	sql_conn_(NULL)
	,sql_res_(NULL)
	,sql_row_(NULL)
	,ip_("")
	,user_("")
	,pwd_("")
	,db_("")
	,loop_(0)
{
}

//destructor
SqlController::~SqlController(void)
{
	Close();
}

std::string SqlController::GetErrorMsg()
{
	char buf[1024] = {0};
	snprintf(buf, 1024, "Sql error code: %d, Msg: %s", mysql_errno(sql_conn_), mysql_error(sql_conn_));

	return buf;
}

bool SqlController::Open(const std::string &host, const std::string &user, const std::string &pwd, const std::string &db)
{
	sql_conn_ = mysql_init(NULL);
	sql_conn_ = mysql_real_connect( sql_conn_,
			host.c_str(),
			user.c_str(),
			pwd.c_str(),
			db.c_str(),
			MYSQL_PORT, NULL, 0);

	if(!sql_conn_) return false;
	Excute("SET NAMES utf8");
	return true;
}

void SqlController::Close()
{
	ReleaseRes();
	if(sql_conn_) {
		mysql_close(sql_conn_);
		sql_conn_ = NULL;
	}
}

void SqlController::ReleaseRes()
{
	if(sql_res_) {
		mysql_free_result(sql_res_);
		sql_res_ = NULL;
	}
}

bool SqlController::KeepConnect()
{
	if(! mysql_ping(sql_conn_) ) {
		return true;
	}
	//if(sql_conn_) return true;

	Close();
	return Open(ip_, user_, pwd_, db_);
}

bool SqlController::Query(const std::string &sql)
{
	if(!KeepConnect()) {
		return false;
	}
	//KeepConnect();
	if(mysql_real_query(sql_conn_, sql.c_str(), sql.length())) {
		return false;
	}
	DEBUG_MSG("sql query : %s", sql.c_str());
	ReleaseRes();
	sql_res_ = mysql_store_result(sql_conn_);
	if(!sql_res_) {
		return false;
	}
	return true;
}

bool SqlController::Excute(const std::string &sql)
{
	if(!KeepConnect()) {
		return false;
	}
	//KeepConnect();
	DEBUG_MSG("sql excute : %s", sql.c_str());
	return mysql_real_query(sql_conn_, sql.c_str(), sql.length());
}

int SqlController::RowCount()
{
	int ret = 0;
	if(sql_res_) {
		ret = mysql_num_rows(sql_res_);
	}
	return ret;
}

MYSQL_ROW SqlController::NextRow()
{
	if(!sql_res_) return NULL;
	sql_row_ = mysql_fetch_row(sql_res_);
	return sql_row_;
	//MYSQL_ROW row = mysql_fetch_row(sql_res_);
	//return row;
}




