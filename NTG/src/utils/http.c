/*
 * http1.c
 *
 *  Created on: Oct 19, 2014
 *      Author: tangzhihua
 */
#include <error.h>
#include "http.h"
#include "../types.h"
#include "text.h"
#include "gstring.h"

static int http_parse_startline(http_parser* parser);
static int http_parse_header(http_parser* parser);
static void print_parser(http_parser *parser);
//static int http_parse_header30x(http_parser *parser);
/*
 * http_request_message 函数 用于构造http请求消息
 * 参数：
 * 		message->接收消息
 * 		size ->message指向空间的大小
 * 		method->请求方法
 * 		url->指向
 * 		isclose ->是否关闭长链接
 * 	返回：实际消息的大小
 */

int http_request_message(char *message, int size, enum http_method method,
		const url_t *url, int isclose) {
	char *m;
	char *http_t = "HTTP/1.1";
	int num;
	/*判断请求的方法*/
	if (method == HTTP_GET)
		m = "GET";
	else
		m = "POST";
	/*请求消息的不变部分*/
	char * accept = "User-Agent: Mozilla/5.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
		"Accept-Language: en-us,en;q=0.5\r\n"
		"Accept-Encoding: gzip,deflate\r\n"
		"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n";
	char * con_close = "Connection: close\r\n";
	//	/*构造http请求消息*/
	num = snprintf(message, size, "%s %s %s\r\nHost: %s:%d\r\n%s", m,
			url->file, http_t, url->host, url->port, accept);
	if (isclose)
		num += snprintf(message + num, size - num, "%s", con_close);
	num += snprintf(message + num, size - num, "%s", "\r\n");
	return num;
}
/*
 * http_parser_init 函数初始化http_parser
 * 参数：
 * 		parser->http_parser指针
 * 		buffer-->缓存区指针
 * 	返回：无
 */
void init_http_parser(http_parser *parser, char *buffer) {
	parser->h_com.type = DATA_HTTP;
	parser->h_com.buffer = buffer;
	parser->h_com.pos = 0;
	parser->h_com.posParse = buffer;
	parser->h_com.area = buffer;
	parser->h_com.isconnect = 1;
	parser->ischunk = 0;
	parser->content_length = -1;
	parser->chunk_size = -1;
	parser->state = ANSWER;
	parser->h_com.parse_enter = (void *)enter_http_parser;
}
/*
 * http_parser_enter() http解析器的入口
 * 参数：
 * 		parser-->http解析器指针
 * 		size-->接收的数据大小
 * 	返回：int
 * 		0 -->继续解析
 * 		1-->解析出错退出
 * 		2-->解析完成
 *
 */
int enter_http_parser(http_parser* parser, int size) {
	if (parser->state == BODY) {/*不对http的内容进行处理*/
		parser->h_com.pos = 0;
		return 0;
	}
	parser->h_com.pos += size;/*pos表示接收到的位置*/
	parser->h_com.buffer[parser->h_com.pos] = '\0';
	/*对头部的解析*/
	char *posn = NULL;
	while (parser->h_com.posParse < parser->h_com.buffer + parser->h_com.pos)/*本次接收的数据buffer+pos解析结束的位置*/
	{
//		printf("reading....num....>%d\n", i++);
		int size;
//		print_parser(parser);
		size = parser->h_com.pos - (parser->h_com.area - parser->h_com.buffer);//获取接收到的数据长度
		if (parser->state == CHUNK && (parser->chunk_size > 0)) {//chunk_size>0说明还有数据
			if ((parser->chunk_size - size) >= 0) {
				parser->chunk_size -= size;
				parser->h_com.pos = 0;
				parser->h_com.area = parser->h_com.posParse
						= parser->h_com.buffer;
				return 0;
			} else {
				parser->h_com.area = parser->h_com.posParse
						= (parser->h_com.area + parser->chunk_size);
				parser->chunk_size = -1;
			}
		}
//		print_parser(parser);
//		printf("pos ------->%d\n", parser->h_com.pos);
//		printf("posParse----->%d\n",
//				parser->h_com.posParse - parser->h_com.buffer);
		posn = strchr(parser->h_com.posParse, '\n');

		/*********************************************/
//		if (posn != NULL) {
//			printf("posn  size---->%d\n", posn - parser->h_com.area);
//			printf("posn size of posParse---> %d\n",
//					posn - parser->h_com.posParse);
//			printf("posn---->%d\n", posn - parser->h_com.buffer);
//			char buf[1024];
//			snprintf(buf, (posn - parser->h_com.area) + 1, "%s",
//					parser->h_com.area);
//			printf("line1---->%s", buf);
//		}
//		printf("pos ---no---->%d\n", parser->h_com.pos);
		/********************************/
		switch (parser->state) {
		case ANSWER:/*响应*/
			if (posn != NULL) {
				parser->h_com.posParse = posn;/*移动解析位置*/
				if (http_parse_startline(parser)) {/*解析状态行*/
					return 1;
				}
				parser->h_com.area = ++parser->h_com.posParse;/*跳过'\n'*/
			} else {/*没有接收完*/
				if (parser->h_com.pos > MAX_LINE) {/*判断是否超过最大行*/
					return 1;
				}
			}
			break;
		case HEADERS:/*头域*/
			if (posn != NULL) {
				parser->h_com.posParse = posn;
				if (http_parse_header(parser)) {/*头域解析*/
					return 1;
				}
				parser->h_com.area = ++parser->h_com.posParse;
			} else {
				/*判断是否超过最大行*/
				if (parser->h_com.pos - (parser->h_com.posParse
						- parser->h_com.buffer) > MAX_LINE) {
					return 1;
				}
				//剩余数据的前移
				parser->h_com.pos = gstrbmove(parser->h_com.buffer,
						parser->h_com.posParse);
				parser->h_com.posParse = parser->h_com.area
						= parser->h_com.buffer;
				return 0;
			}
			break;
		case CHUNK:/*采用chunk编码*/
			//			printf("chunk_size--------->%d\n", parser->chunk_size);
			//			printf("ischunk------%d\n", parser->ischunk);
			if (posn != NULL && parser->ischunk) {/*ischunk = 1 说明chunk编码实体还未结束*/
				parser->h_com.posParse = posn;
//				printf("area----->%d\n",
//						parser->h_com.area - parser->h_com.buffer);
				int len = gstrhextonum(parser->h_com.area);
				if (len > 0) {
					parser->chunk_size = len + 2;/*\r\n*/
				} else {/*chunk编码实体结束*/
					parser->ischunk = 0;
					parser->chunk_size = 0;
				}
				parser->h_com.area = ++parser->h_com.posParse;
			} else if (posn != NULL) {/*进入trailer解析*/
				parser->h_com.posParse = posn;
				size = parser->h_com.posParse - parser->h_com.area;//待解析行的大小
				if (size < 2) {/*空行*/
					//					printf("接收完毕\n");
					return 2;
				}
				parser->h_com.area = ++parser->h_com.posParse;
			} else {
				/*判断是否超过最大行*/
				if (parser->h_com.pos - (parser->h_com.posParse
						- parser->h_com.buffer) > MAX_LINE) {
//					printf("max_line---\n");
					return 1;
				}
				/*数据前移*/
//				printf("数据前移\n");
				parser->h_com.pos = gstrbmove(parser->h_com.buffer,
						parser->h_com.posParse);
//				printf("gstrbmove-----size=%d\n", parser->h_com.pos);
				parser->h_com.posParse = parser->h_com.area
						= parser->h_com.buffer;
//				printf("continue-----------\n");
				return 0;
			}
			break;
		case CONTENT:/*采用content-encoding编码*/
			parser->content_length -= size;
			parser->h_com.pos = 0;
			parser->h_com.area = parser->h_com.posParse = parser->h_com.buffer;
			/*当以\r\n结尾时，可能content_length<0*/
			if (parser->content_length <= 0) /*数据接收完*/
				return 2;
			return 0;
			break;
		case BODY:/*没有长度确定方式*/
			parser->h_com.pos = 0;
			break;
		default:
			return 0;
		}
	}
	return 0;
}

/*
 * http_parse_cmdline() 解析http的命令行（响应状态行）
 * 参数：parser-->http解析器指针
 * 返回：int
 * 		0-->继续
 * 		1-->出错
 */
static int http_parse_startline(http_parser* parser) {
	int len_size;
	len_size = parser->h_com.posParse - parser->h_com.area;
	if (len_size >= 12) {/*判断条件  HTTP/1.1 200 OK \r\n*/
		switch (parser->h_com.buffer[9]) {/*获取状态码的第一个数*/
		case '2':
			parser->state = HEADERS;
			break;
		case '3':
			parser->state = HEADERS30X;
			break;
		default:
			errno = err40X;
			return 1;
		}
	} else {
		if (len_size < 2) {//去掉可能遗留的\r\n
			return 0;
		}
		errno = earlyStop;
		return 1;
	}
	return 0;
}
/*
 * http_parse_header() 解析http响应200状态数据的头部域
 * 参数：parser-->http解析器指针
 * 返回：int
 * 		0-->继续
 * 		1-->出错
 * 注：保证已经得到完整的一行，该函数只对行操作。行的保证由调用者提供
 */
static int http_parse_header(http_parser* parser) {
	int size;
	char *p;
	size_t len;
	//posParse --> 指向当前行的结尾(换行字符位置)
	//area --> 指向当前行的开始位置
	size = parser->h_com.posParse - parser->h_com.area;//待解析行的大小
	if (size < 2) {/*\r\n\r\n 代表头域的结束*/
		if ((parser->content_length == -1) && (parser->ischunk == 0)) {
			parser->state = BODY;
			return 0;
		}
		if (parser->content_length != -1) {
			parser->state = CONTENT;
		}
		if (parser->ischunk) {
			parser->state = CHUNK;
		}
	} else {/*头域解析*/
		char buffer[size + 1];
		snprintf(buffer, size, "%s", parser->h_com.area);
		//		buffer[size] = '\0';
		/*判断相关标识*/
		if (is_startWithIgnoreCase(CONNECTION, buffer)) {//判断connection标识
			len = strlen(CONNECTION);
			p = gstrim_st(buffer + len + 1);
			if (is_startWithIgnoreCase(CLOSE, p)) {
				parser->h_com.isconnect = 1;//设置重连标志
			}
		} else if (is_startWithIgnoreCase(CONTENT_LENGTH, buffer)) {//判断content-length标识
			len = strlen(CONTENT_LENGTH);
			parser->content_length = gstrtonum(buffer + len + 1);
		} else if (is_startWithIgnoreCase(TRANSFER_ENCODING, buffer)) {//判断transfer-encoding标识
			len = strlen(TRANSFER_ENCODING);
			p = gstrim_st(buffer + len + 1);
			if (is_startWithIgnoreCase(CHUNKED, p)) {//为chunked标识
				parser->ischunk = 1;
			} else {//其他标识暂不处理
				return 1;
			}
		}
	}
	return 0;
}

static void print_parser(http_parser *parser) {
	printf("%d,   %d,   %d , chunk_size= %d\nisconnect--------->%d\n",
			parser->state, parser->ischunk, parser->content_length,
			parser->chunk_size, parser->h_com.isconnect);
}
/*
 * http_parse_header30x() 解析http响应3XX状态数据的头部域
 * 参数：parser-->http解析器指针
 * 返回：int
 * 		0-->继续
 * 		1-->出错
 * 	注：尚未实现
 */
//static int http_parse_header30x(http_parser *parser) {
//
//	if (parser->h_com.posParse - parser->area < 2) {
//		// end of http headers without location => err40X
//		errno = err40X;
//		return 1;
//	} else {
////		if (is_startWithIgnoreCase("location: ", parser->area)) {
////			int i = 10;
////			while (area[i] != ' ' && area[i] != '\n' && area[i] != '\r') {
////				i++;
////			}
////			if (notCgiChar(area[i])) {
////				area[i] = 0; // end of url
////				// read the location (do not decrease depth)
////				url *nouv = new
////				url(area + 10, here->getDepth(), base);
////
////				manageUrl(nouv, true);
////				// we do not need more headers
////			}
////			errno = err30X;
////			return 1;
////		}
//	}
//	return 0;
//}
