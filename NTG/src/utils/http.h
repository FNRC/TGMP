/*
 * http1.h
 *
 *  Created on: Oct 19, 2014
 *      Author: tangzhihua
 */

#ifndef HTTP1_H_
#define HTTP1_H_
#include "../lib/unp.h"
#include "url.h"
#include "../global.h"
#include "../types.h"

//#define ANSWER 0
//#define HEADERS 1
//#define HEADERS30X 2
//#define HTML 3

enum http_state {
	ANSWER, HEADERS, HEADERS30X,BODY,CHUNK,CONTENT
};
enum http_method {
	HTTP_GET, HTTP_POST
};

#define MAX_LINE 256

#define CONNECTION "connection"
#define CONTENT_LENGTH "content-length"
#define TRANSFER_ENCODING "transfer-encoding"

#define CHUNKED "chunked"
#define KEEP_ALIVE "keep-alive"
#define CLOSE "close"



struct http_parser {
	struct parser_common h_com;
	// beginning of the current interesting area
	uint32_t ischunk: 1;
	uint32_t  state:8;
	int32_t content_length;
	int32_t chunk_size;
};

typedef struct http_parser http_parser;



int http_request_message(char *message, int size, enum http_method method,
		const url_t *url, int isclose);
extern void init_http_parser(http_parser *parser, char *buffer);
extern int enter_http_parser(http_parser* parser, int size);

#endif /* HTTP1_H_ */
