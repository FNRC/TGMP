/*
 * url.h
 *
 *  Created on: Oct 16, 2014
 *      Author: tangzhihua
 */

#ifndef URL_H_
#define URL_H_
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "../lib/unp.h"
typedef struct url url_t;

struct url{
	  char *host;/*主机名*/
	  char *pre_path;
	  char *file;/*请求的文件*/
	  uint16_t port; /*协议端口*/
};
extern void setfileofurl(url_t* url, const char  *file);
extern url_t* paser_url( const char * str_url, int str_len);
extern void free_url(url_t *url);
#endif /* URL_H_ */
