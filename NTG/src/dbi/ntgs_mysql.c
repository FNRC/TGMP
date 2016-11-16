/*
 * ntgs_mysql.c
 *
 *  Created on: Dec 8, 2014
 *      Author: tangzhihua
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zdb.h>
#include "ntgs_mysql.h"
#include "time.h"
#include "../global.h"

//MYSQL_RES *res_ptr;
//MYSQL_ROW sqlrow;
#define T Connection_T
static int get_page_id(T con, char * url);
//extern char intablename;
/*
 * get_page_id 获取指定url的页面id
 */
static int get_page_id(T con, char * url) {
	int p_id = -1;
	ResultSet_T rst;
	rst = Connection_executeQuery(con, "select id from page_t where url = '%s'",
			url);
	if (ResultSet_next(rst)) {
		p_id = ResultSet_getInt(rst, 1);
		return p_id;
	}
	return p_id;
}

ConnectionPool_T init_connection_pool(const char *url_str) {

	ConnectionPool_T pool = NULL;
	URL_T url = URL_new(url_str);
	pool = ConnectionPool_new(url); //新建一个连接池结构体，并分配内存
	ConnectionPool_start(pool);
	return pool;
}

void free_connection_pool(ConnectionPool_T pool) {
	URL_T url = ConnectionPool_getURL(pool);
	ConnectionPool_free(&pool);
	URL_free(&url);
}

int add_page(T con, page_t * page) {
	//	int res, id = -1;
	//	ResultSet_T rst;
	//
	//	id = get_page_id(con, page->url);
	//	if (id > 0)
	//		return id;
	//	Connection_execute(con, "insert  into page_t value(null, '%s')",page->url);
	//	id = get_page_id(con, page->url);
	/*
	 * 如果resource不为空，添加到数据库
	 */
	return 0;
}

page_t * get_page(T con, int index) {
	page_t* page = NULL;
	ResultSet_T rst;
	page = Calloc(1, sizeof(page_t));

	rst = Connection_executeQuery(con, "select url from page_t where id = '%d'",
			index);//页面是单个资源
	if (!ResultSet_next(rst))
		goto e_result_set;
	page->url = paser_url(ResultSet_getString(rst, 1),
			ResultSet_getColumnSize(rst, 1));
	page->result_set = Connection_executeQuery(con,
			"select file_path from resource_t where page_id = '%d'", index);//页面中资源又多个资源


	return page;

	e_result_set: free(page);
	return NULL;
}

void free_page(page_t * page) {
	if (page->url != NULL)
		free_url(page->url);
	free(page);
}

/**
 * 插入获取的信息表单
 */
void insert_log(T con, void*u, struct timeval *tstart, struct timeval *tend,
		long count) {
	long usec;
	user_t* user;
	user = (user_t*) u;
	usec = (tend->tv_sec - tstart->tv_sec) * 1000000
			+ (tend->tv_usec - tstart->tv_usec);
//	printf("insert_log\n");

	time_t t;	///修改成了获取系统当前的时间值
	int time1;
	time1=time(&t);
	/*获取创建的table名称*/
//char tablename1=*getlocalip();这个函数有问题

/**
 *往tablename中插入数据
 */
	char insql[255]="insert into ";
	strcat(insql,intablename);
	char insql1=" values(null, '%d','%d','%s','%ld.%ld','%ld','%ld')";
	strcat(insql,insql1);
	printf(insql);
	printf("\n");
	Connection_execute(con,
				insql,
				user->id, user->type, user->url, usec / 1000, usec % 1000,
				count / 1024,time1);
		printf("时间%ld",time1);


//	Connection_execute(con,
//			"insert into log_t values(null, '%d','%d','%s','%ld.%ld','%ld',unix_timestamp()) ",
//			user->id, user->type, user->url, usec / 1000, usec % 1000,
//			count / 1024);
//	printf("好12321/");
}

void insert_user(T con, int num) {
//	printf("insert_user\n");
	Connection_execute(con,
			"insert into offusers_t value(null, '%d',UNIX_TIMESTAMP())", num);
}

void insert_wait_user(T con, int num, int off_num, int browse_num) {
//	printf("insert_wait_user\n");
	Connection_execute(con,
			"insert into users_t value(null, '%d', '%d', '%d', '%d',UNIX_TIMESTAMP())",
			num, off_num, num - off_num, num - off_num - browse_num);
}


/**
 *根基本机IP创建log_T_IP table
 */








//static MYSQL_RES* excute_query(char * query);
//static int get_page_id(char * url);
///*
// * database_init   数据库的初始化
// */
//int database_init(mysql_t * mysql, char *name, char * password, char *database) {
//	//	if (mysql->dbconnected)
//	//		return 1;
//	if (mysql == NULL)
//		mysql = Calloc(1, sizeof(mysql_t));
//
//	mysql_library_init(0, NULL, NULL);
//
//	mysql_init(&mysql->conn);
//	pthread_mutex_init(&mysql->mysql_lock, 0);
//	pthread_cond_init(&mysql->mysql_cond, 0);
//
//	if (!mysql_real_connect(&mysql->conn, "localhost", name, password, "foo",
//			0, NULL, 0)) {
//		fprintf(stderr, "Connection failed\n");
//		if (mysql_errno(&mysql->conn)) {
//			fprintf(stderr, "Connection error %d:%s\n",
//					mysql_errno(&mysql->conn), mysql_error(&mysql->conn));
//		}
//		return -1;
//	}
//	printf("Connection success\n");
//	return 0;
//}/*database_init*/
//
//void database_free() {
//	mysql_close(&db->conn);
//}/*database_free*/
//
//int add_page(page_t * page) {
//	MYSQL_RES *res_ptr;
//	MYSQL_ROW mysqlrow;
//	int res, id = -1;
//	char qs[384];
//	char es[256];
//
//	id = get_page_id(page->url);
//	if (id > 0)
//		return id;
//	mysql_real_escape_string(&db->conn, es, page->url, strlen(page->url));
//	sprintf(qs, "insert  into page_t value(null, '%s')", es);
//	res = mysql_query(&db->conn, qs);
//	if(res) return id;
//	id = get_page_id(page->url);
//	/*
//	 * 如果resource不为空，添加到数据库
//	 */
//	return 0;
//}
//int delete_page_of_id(int page_id);
//int delete_page_of_url(char *url);
//int get_page(page_t* page);

//
//static MYSQL_RES* excute_query(char * query) {
//	int res;
//	MYSQL_RES * res_ptr = NULL;
////	if (db->conn == NULL)
////		return res_ptr;
//	pthread_mutex_lock(&db->mysql_lock);
//	res = mysql_query(&db->conn, query);
//	if (res) {
//		fprintf(stderr, "select error:%s\n", mysql_error(&db->conn));
//		pthread_mutex_unlock(&db->mysql_lock);
//		return res_ptr;
//	}
//	res_ptr = mysql_store_result(&db->conn);
//	pthread_mutex_unlock(&db->mysql_lock);
//	return res_ptr;
//}
//
//static int excute_update(char *update) {
//	MYSQL_RES* res_ptr;
//	int rows = -1;
//	int res;
//	res = mysql_query(&db->conn, update);
//	if(res) {
//		fprintf(stderr, "select error:%s\n", mysql_error(&db->conn));
//		return rows;
//	}
//	res_ptr = excute_query("SELECT ROW_COUNT()");
//
//	return rows;
//}
//
//int add_resource(int page_id, char *rs_path);
//int delete_resource(char *rs_path);
//void* get_resource(int page_id);

