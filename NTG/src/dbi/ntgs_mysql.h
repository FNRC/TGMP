/*
 * ntgs_mysql.h
 *
 *  Created on: Dec 8, 2014
 *      Author: tangzhihua
 */

#ifndef NTGS_MYSQL_H_
#define NTGS_MYSQL_H_
#include <mysql.h>

#include <pthread.h>
#include <zdb.h>
#include "../global.h"
#include "../utils/url.h"

MYSQL_RES *res_ptr;
MYSQL_ROW sqlrow;

///*数据库*/
//struct database_pool{
//	URL_T url;
//	Connection_T con;
//};
//
//#define T database_pool_p
//typedef struct database_pool* T;

typedef struct mysql_conn_st {
	MYSQL conn;
	int dbconnected;
	pthread_mutex_t mysql_lock;
	pthread_cond_t mysql_cond;
}mysql_t;

typedef struct page_st {
	url_t* url;/*页面的url*/
	ResultSet_T result_set;/*页面内嵌的资源*/
}page_t;

extern void insert_wait_user(Connection_T con, int num, int off_num , int browse_num);
extern void insert_user(Connection_T con, int num );
extern void insert_log(Connection_T con,  void *user, struct timeval *tstart,struct timeval *tend, long count);
extern ConnectionPool_T init_connection_pool(const char *url_str);
extern void free_connection_pool(ConnectionPool_T pool);
extern page_t * get_page(Connection_T con, int index);
extern void free_page(page_t * page);
extern void creat_log_t(char *ip);//
extern char  *getlocalip();
extern void creat_log_t_ip(Connection_T con,char *tableName);


#endif /* NTGS_MYSQL_H_ */
