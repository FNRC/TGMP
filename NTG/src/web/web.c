/*
 * web.c
 *
 *  Created on: Aug 26, 2014
 *      Author: tangzhihua
 */
#include <time.h>
#include "../types.h"
#include "../global.h"
#include "web.h"

/*
 * web用户函数操作集
 */
struct user_ops web_user_ops =
{
		.builed = web_builed,
		.request = NULL,
		.receive = web_receive,
		.behavior = web_behavior,
};
/*
 * web_builed() 与远程服务端建立web链接
 * 参数：
 * 		url-->url指针
 *返回：socket套接字
 */

int web_builed(const char * host )
{
	return (Tcp_connect(host, "80"));
}

void web_request(int sockfd, char *msg, size_t msg_size)
{
	Write(sockfd, msg, msg_size);
}

ssize_t web_receive(int sockfd, char *buf, size_t buf_size)
{
	ssize_t n;
//	n = Read(sockfd, buf, sizeof(buf));
//	return (n);
	while ((n = read(sockfd, buf, buf_size)) > 0)
	{
		buf[n] = '\0';
//		printf("%s\n", buf);
	}
	return n;
}

void web_behavior(void * u) {
	/*初始化用户并加入队列*/
	user_t* user;
	user = (user_t*)u;
	srand((unsigned) time(NULL));
	user->time = 1 + rand() % UM_TIME;/*生成[1,720]间的随机数*/
	user->page_id = 1+ rand() %MAX_PAGE_NUM;
	//strncpy(user->url, url, strlen(url));
//	snprintf(user->url, URL_SIZE,"%s","http://hongyan.cqupt.edu.cn/web/");
}

