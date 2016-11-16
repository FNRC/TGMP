/*
 * maintest.c
 *
 *  Created on: Oct 18, 2014
 *      Author: tangzhihua
 */
#include "utils/url.h"
#include "types.h"
#include "utils/http.h"
#include "web/web.h"
#include	"lib/unp.h"
#include "utils/gstring.h"
#include <string.h>
#include <strings.h>
#include <zdb.h>
#include "dbi/ntgs_mysql.h"
#include "utils/text.h"
int main2(int argc, char **argv) {

	ConnectionPool_T pool;
	page_t* page;
	pool = init_connection_pool("mysql://localhost/urldb?user=root&password=123456");
	Connection_T con = ConnectionPool_getConnection(pool);
	page =  get_page( con, 2);
//	printf("url = %s\n", page->url->file);
	while(ResultSet_next(page->result_set)){
//		printf("%s\n", ResultSet_getString(page->result_set, 1));
	}

	Connection_execute(con,"insert into log_t value(null, '%d','%d','%s','%d.%d','%d')",
				2, 23, "hongyan.cqupt.edu.cn/web/", 12 , 24, 1023);


//	/*测试is_startWithIgnoreCase*/
//	char * a = "startwithignorecase";
//	char * b = "startWithIgnoreCase";
//	if(is_startWithIgnoreCase(a,b)){
//		printf("true \n");
//	}else{
//		printf("false\n");
//	}
	/*测试3bcopy与memcopy*/
//	int buffer[15];
//	int buffer2[10];
//	int i;
//	bzero(buffer2,10);
//	for (i = 0; i < 15; i++) {
//		buffer[i] = 0;
//	}
//	for (i = 0; i < 15; i++) {
//		printf("%d",buffer[i]);
//	}
//	printf("\n\n");
//	for (i = 5; i < 10; i++) {
//		buffer[i] = i;
////		buffer2[i] = i;
//		printf("%d\n", buffer[i]);
//	}
//	memcpy(buffer, buffer+5, 5);
//	for (i = 0; i < 15; i++) {
//		printf("%d\n", buffer[i]);
//	}
//	printf("\n");
//	for (i = 0; i < 10; i++) {
//		printf("%d", buffer2[i]);
//	}
//	bcopy(buffer, buffer + 5, 4);
//	memcpy(buffer2 + 5, buffer2, 4);
//	printf("\nresult.......\n");
//	for (i = 0; i < 10; i++) {
//		printf("%d", buffer[i]);
//	}
//	printf("\n");
//	for (i = 0; i < 10; i++) {
//		printf("%d", buffer2[i]);
//	}

	/*测试2 gstrtonum*/
	//	char *test = ": 12 31  \r\n";
	//	int rst;
	//	rst = gstrtonum(test);
	//	printf("%d\n",rst);


	/*测试1*/
	//	url_t *url;
	//	int size;
	//	int rcv_len;
	//	int sockfd;
	//	char *str_urls = "http://hongyan.cqupt.edu.cn/web/,"
	//				"admin/upload/pictures/thumbnail_1300252276_0.jpg,"
	//				"admin/upload/pictures/thumbnail_1288163191_0.jpg,"
	//				"admin/upload/pictures/thumbnail_1289192278_0.jpg,"
	//				"admin/upload/pictures/thumbnail_1286288717_0.jpg,"
	//				"admin/upload/pictures/thumbnail_1286288818_0.jpg,"
	//				"admin/upload/pictures/thumbnail_1286288839_0.jpg,"
	//				"admin/upload/pictures/thumbnail_1286288862_0.jpg,";
	//
	//	url = paser_url(str_urls,strlen("http://hongyan.cqupt.edu.cn/web/"));
	//	char msg[MESSAGE_SIZE];
	//	char buf[1024];
	//	size = http_request_message(msg, MESSAGE_SIZE, HTTP_GET, url, 1);
	//	sockfd = web_builed(url->host);
	//	printf("socket--->%d\n",sockfd);
	//	Write(sockfd, msg, size);
	//	printf("message:\n%s",msg);
	//	printf("start receive \n");
	//	while ((rcv_len = read(sockfd, buf, 1024))!=0){
	//		buf[rcv_len] = '\0';
	//		printf("%s",buf);
	//	}


	//	printf("host = %s; port = %d; file = %s", url->host, url->port, url->file);
	return 0;
}
