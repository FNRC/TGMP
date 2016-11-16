/*
 * web.h
 *
 *  Created on: Aug 27, 2014
 *      Author: tangzhihua
 */

#ifndef WEB_H_
#define WEB_H_
#include <ctype.h>
#include "../lib/unp.h"
#include "../global.h"

extern void web_behavior(void * user);
extern int  web_builed( const char * host);
extern void web_request(int sockfd, char *msg, size_t msg_size);
extern ssize_t  web_receive(int sockfd, char *buf, size_t buf_size);
extern  struct user_ops web_user_ops ;


#endif /* WEB_H_ */
