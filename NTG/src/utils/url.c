/*
 * url.c
 *
 *  Created on: Oct 16, 2014
 *      Author: tangzhihua
 */
#include "url.h"
#include <stdlib.h>
/*
 * paser_url函数
 * 		解析url为host、port和file三部分
 * 	参数：
 * 		str_url -> 指向url的字符指针
 * 		str_len -> 字符串的长度
 * 	返回：
 * 		url_t 指针
 * 	该函数才用里动态内存分配
 * 	注：该函数目前只实现了对http协议url的解析
 */

url_t* paser_url(const char * str_url, int str_len) {
	url_t* url;
	char *ptr;
	int len, i;
	ptr = (char *) str_url;
	if (str_len != 0) {
		len = str_len;
	}
	/*url检查,对url的合法并没用检测*/
	if (ptr == NULL)
		return NULL;
	if (strstr(ptr, "http://")) { //以http://开头
		ptr += 7;
		len -= 7;
	}

	char *p = ptr;
	url = Calloc(1, sizeof(url_t));

	i = 0;
	while (i < len) {
		switch (p[i]) {
		case ':':
			if (url->host == NULL) { //先发现了':'
				url->host = Calloc(i + 1, sizeof(char));
				strncpy(url->host, ptr, i);
				url->host[i + 1] = '\0';
			}
			i++;
			while (p[i] >= '0' && p[i] <= '9') { //获取端口
				url->port = 10 * url->port + p[i] - '0';
				i++;
			}
			break;
		case '/':
			if (url->host == NULL) { //先发现了'/'
				url->host = Calloc(i + 1, sizeof(char));
				strncpy(url->host, ptr, i);
				url->host[i] = '\0';
			}
			url->file = Calloc(len - i + 1, sizeof(char));
			strncpy(url->file, p + i, len - i);
			url->file[len - i] = '\0';
			i = len;
			break;
		default:
			i++;
			break;
		}
	}
	/*以下为无port和file的情况，如“www.baidu.com”*/
	if (url->host == NULL) {
		url->host = Calloc(len + 1, sizeof(char));
		strncpy(url->host, ptr, len);
		url->host[len] = '\0';
	}
	if (url->port == 0) {
		url->port = 80;
	}
	if (url->file == NULL) {
		url->file = Calloc(2, sizeof(char));
		strcpy(url->file, "/");
		url->file[1] = '\0';
	}
	char *s = url->file;
	char *flag;
	while (*s != '\0') {
		switch (*s) {
		case '/':
			flag = s;
			break;
		default:
			;
		}
		s++;
	}
	url->pre_path = Calloc(strlen(url->file) + 1, sizeof(char));
	strncpy(url->pre_path, url->file, flag - url->file + 1);
	url->pre_path[flag - url->file + 1] = '\0';

//	if()
//	url->pre_path[flag - url->file+1] = '\0';
//		url->pre_path = strdup(url->file);
//		printf("%s\n",url->pre_path);
	return url;
}
/*
 * setfileofurl 用于设置url对象的file属性
 * 参数：
 * 	url--->url对象指针
 * 	file-->目标file指针
 * 	file_len-->file的长度
 * 返回：无
 */
void setfileofurl(url_t* url, const char *file) {
	int len = 0;
	if (url->file != NULL) {
		free(url->file);
	}
	len = strlen(file) + strlen(url->pre_path);
	url->file = Calloc(len + 1, sizeof(char));
	strcpy(url->file, url->pre_path);
	strcat(url->file, file);
	url->file[len] = '\0';
}

/*
 * free_url函数 用于释放url_t及其所属的资源
 * 参数：url_t 指针
 * 返回：无
 */
void free_url(url_t *url) {
	if (url == NULL)
		return;
	/*释放url所属的资源*/
	if (url->host != NULL)
		free(url->host);
	if (url->file != NULL)
		free(url->file);
	if (url->pre_path != NULL)
		free(url->pre_path);
	free(url);
	return;
}
