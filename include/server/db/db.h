#ifndef __DB_H__
#define __DB_H__

#include <mysql/mysql.h>
#include <string>

class MySQL {
 public:
  MySQL(void);
  ~MySQL(void);
  bool connect(void);
  bool update(std::string sql);
  MYSQL_RES *query(std::string sql);

 private:
  MYSQL *m_conn;
};

#endif