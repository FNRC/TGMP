/*
 * threadpool.c
 *
 *  Created on: Jul 28, 2014
 *      Author: tangzhihua
 */
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "threadpool.h"
#include "http.h"
#include "../types.h"
#include "../dbi/ntgs_mysql.h"


static void *thread_routine(void *arg);

static user_t * get_user_from_wait();
static void reset_user(user_t * user);
static void replace_user(user_t *user);

static void* init_pool_parser(char * buffer, int user_type);
static void free_pool_parser(parser_t* parser);
static void re_pool_parser(parser_t *parser);
static int pool_read_parse(int socket, parser_t * parser , long *count);

/*
 *pool_init()  线程池初始化
 *参数：
 *		max_thread_num-->最大的线程数
 *	返回：无
 */
void pool_init(int max_thread_num) {

	/*为线程池分配内存资源*/
	pool_queue = calloc(1, sizeof(b_queue));
	/*初始化互斥锁和条件变量*/
	pthread_mutex_init(&(pool_queue->mutex), NULL);
	pthread_cond_init(&(pool_queue->ready), NULL);
	/*初始化线程池的一些属性*/
	pool_queue->head = pool_queue->tail = NULL;
	pool_queue->max_thread_num = max_thread_num;
	pool_queue->cur_size = 0;
	pool_queue->shutdown = 0;
	pool_queue->threadid = (pthread_t *) malloc(
			max_thread_num * sizeof(pthread_t));/*为线程池分配资源*/
	/*创建所有线程*/
	int i;
	for (i = 0; i < max_thread_num; i++) {
		pthread_create((pthread_t *) &(pool_queue->threadid[i]), NULL,
				thread_routine, NULL);
	}
}

/*
 * max_thread_num() 销毁线程池，等待队列中的任务不会再被执行，
 * 但是正在运行的线程会一直把任务运行完后再退出。
 * 參數：无
 * 返回：int
 * 		销毁成功返回0，失败返回-1
 */
int pool_destroy() {

	if (pool_queue->shutdown)
		return -1;/*防止两次调用*/
	pool_queue->shutdown = 1;
	/*唤醒所有等待线程，线程池要销毁了*/
	pthread_cond_broadcast(&(pool_queue->ready));
	/*阻塞等待线程退出，否则就成僵尸了*/
	int i;
	for (i = 0; i < pool_queue->max_thread_num; i++)
		pthread_join(pool_queue->threadid[i], NULL);
	free(pool_queue->threadid);
	/*销毁等待队列*/
	user_t *head = NULL;
	while (pool_queue->head != NULL) {
		head = pool_queue->head;
		pool_queue->head = pool_queue->head->next;
		free(head);
	}
	/*条件变量和互斥量也别忘了销毁*/
	pthread_mutex_destroy(&(pool_queue->mutex));
	pthread_cond_destroy(&(pool_queue->ready));

	free(pool_queue);
	/*销毁后指针置空是个好习惯*/
	pool_queue = NULL;
	return 0;
}
/*
 * thread_routine() 线程例程
 *参数：void类型指针
 *返回：void类型指针
 */
static void* thread_routine(void *arg) {
	int size;
	int sockfd;

	char buff[MAX_BUFFER];
	char msg[MESSAGE_SIZE];

	user_t *user = NULL;
	parser_t *parser;
	page_t* page;
	Connection_T con;

	struct timeval tend, tstart;


	//	printf("starting thread 0x%x\n", (unsigned int) pthread_self());
	/*在线程循环之前床将所存在的表单*/
	//con = ConnectionPool_getConnection(pool);
//	char tablename=*getlocalip();
//	printf("出该见失败11111111111");
//	creat_log_t_ip(con,tablename);
//	printf("出该见失败");
	//Connection_close(con);
	/*线程例程一直循环*/
	while (1) {
		long count=0;
		user = get_user_from_wait(); //从等待队列中获取一个用户
		reset_user(user); //调用用户行为函数
		/*并没有对出错处理*/
		parser = init_pool_parser(buff, user->type);//初始化解析器
		if (parser == NULL) {
			goto e_parser;
		}
		con = ConnectionPool_getConnection(pool);
		if (con == NULL)
			goto e_con;
		page = get_page(con, user->page_id);
		if (page == NULL)
			goto e_page;
		strcpy(user->url, page->url->host);
		strcat(user->url, page->url->pre_path);

		int sh = 0;

		if (gettimeofday(&tstart, NULL) == -1) {
			fprintf(stderr, "Failed to get start time\n");
		}

		do {
			if (sh) {//当是页面的内嵌资源时，需设置url的file
			//				printf("%s\n",ResultSet_getString(page->result_set, 1));
				setfileofurl(page->url,
						ResultSet_getString(page->result_set, 1));
			}

			fflush(stdout);
			if (parser->isconnect) {
				//				if(sockfd > 0)
				//					Close(sockfd);
				if (sockfd > 0) {
					Close(sockfd);
					sockfd = -1;
				}
				sockfd = user->ops->builed(page->url->host); //建立链接
				if (sockfd < 0) { //失败的日志记录
					printf("is faile?\n");
					goto e_sock_con;
				}
				parser->isconnect = 0;
			}

			/*构造请求消息*/
			size = http_request_message(msg, MESSAGE_SIZE, HTTP_GET, page->url,
					0);
			printf("request:----------------->\n%s%s\n", page->url->host,page->url->file);

			/*Write的容错有待处理*/
			Write(sockfd, msg, size); //发送请求

			if (pool_read_parse(sockfd, parser, &count)) {
				printf("error1>> %s%s\n", page->url->host,page->url->file);
				printf("%s\n",msg);
				goto e_parse;
			}
			re_pool_parser(parser);
		} while ((sh = ResultSet_next(page->result_set)));

		if (gettimeofday(&tend, NULL) == -1) {
			fprintf(stderr, "Failed to get end time\n");
		}
		long usec = (tend.tv_sec - tstart.tv_sec)*1000000 + ( tend.tv_usec - tstart.tv_usec);
		printf("%s%s---------------->time-----------%ld(ms)\n",//
				page->url->host,page->url->pre_path, usec/1000 );
		fflush(stdout);
		insert_log(con, user, &tstart, &tend, count);
		printf("完成..................\n");
		fflush(stdout);
		/*清理和释放资源*/
		//		bzero(buff, MESSAGE_SIZE);
		//		bzero(msg, MESSAGE_SIZE);
		e_parse: Close(sockfd);
		sockfd = -1;
		e_sock_con: free_page(page);
		e_page: Connection_close(con);
		e_con: free_pool_parser(parser);
		e_parser: replace_user(user);//重置用户
	}
	/*这一句应该是不可达的*/
	pthread_exit(NULL);
}
/*
 *replace_user 根据用户的time属性，重置用户到浏览或离线队列
 *参数：
 *		user-->用户指针
 *	返回：无
 */
static void replace_user(user_t *user) {
	int time;

	time = user->time;
	if ((time > UM_BROWSE_TIME)&& (offline_queue->cur_size < USER_MAX/4) ) {
		/*大于UM_BROWSE_TIME秒将用户加入到离线队列*/
		insert_queue(offline_queue, Q_A, user);
//		printf("用户%d-->加入离线队列\n", user->id);
//		fflush(stdout);
	} else {
		insert_queue(browse_queue, Q_A, user);
//		printf("用户%d-->加入浏览队列\n", user->id);
//		fflush(stdout);
	}
}

/*
 * get_user_from_wait() 从等待队列获取一个用户
 * 参数：无
 * 返回：指向用户的指针
 * 锁：有
 */
static user_t * get_user_from_wait() {
	user_t *user;

	pthread_mutex_lock(&(pool_queue->mutex));
	/*如果等待队列为0并且不销毁线程池，则处于阻塞状态; 注意
	 pthread_cond_wait是一个原子操作，等待前会解锁，唤醒后会加锁
	 */
	while (pool_queue->cur_size == 0 && !pool_queue->shutdown) {
//		printf("thread 0x%x is waiting\n", (unsigned int) pthread_self());
		pthread_cond_wait(&(pool_queue->ready), &(pool_queue->mutex));
	}
	/*线程池要销毁了*/
	if (pool_queue->shutdown) {
		/*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/
		pthread_mutex_unlock(&(pool_queue->mutex));
//		printf("thread 0x%x will exit\n", (unsigned int) pthread_self());
		pthread_exit(NULL);
	}

//	printf("thread 0x%x is starting to work\n", (unsigned int) pthread_self());

	//printf("等待队列的当前大小%d\n", pool->cur_queue_size);
	/*assert是调试的好帮手*/
//	printf("pool_queue--------------->%d\n", pool_queue->cur_size);

	assert(pool_queue->cur_size != 0);//如果assert的条件返回错误，则终止程序执行
	assert(pool_queue->head != NULL);
	/*等待队列长度减去1，并取出链表中的头元素*/

	user = pool_queue->head;
	pool_queue->head = user->next;
	//	if (user->next == NULL)
	//		pool_queue->tail = NULL;
	user->next = NULL;
	pool_queue->cur_size--;
	fflush(stdout);
	pthread_mutex_unlock(&(pool_queue->mutex));
	return user;
}

static void reset_user(user_t * user) {
	user->ops->behavior(user);
}

/*
 * parser_init 根据不用用户的类型初始化解析器parser
 * 参数：
 * 		buffer-->数据缓冲区
 * 		user_type -->用户类型
 * 	返回：parser的指针
 */
static void * init_pool_parser(char * buffer, int user_type) {
	parser_t*parser = NULL;
	switch (user_type) { //根据不同的类型进行处理
	case WEB_USER:
		parser = Calloc(1, sizeof(http_parser));
		init_http_parser((http_parser*) parser, buffer); //调用http解析器初始化函数
		break;
	case STREAM_USER:
		break;
	default:
		break;
	}
	return parser;
}

static void free_pool_parser(parser_t* parser) {
	if (parser != NULL)
		free(parser);
}
/*
 * re_pool_parser 重置parser的属性，实现重用
 * 参数：
 * 	parser--->指向解析器的指针
 * 返回：无
 */
static void re_pool_parser(parser_t *parser) {
	http_parser * h_parser;

	h_parser = (http_parser*) parser;

	h_parser->h_com.type = DATA_HTTP;
	h_parser->h_com.pos = 0;
	h_parser->h_com.posParse = h_parser->h_com.area = h_parser->h_com.buffer;
	h_parser->state = ANSWER;
	h_parser->ischunk = 0;
	h_parser->content_length = -1;
	h_parser->chunk_size = -1;
}

/*
 * read_parse() 从socket中读取数据并进行解析
 * 参数：
 * 		socket--> 套接字描述符
 * 		parser-->解析器指针
 * 返回：
 * 	0 -->继续
 * 	1-->出错
 */
static int pool_read_parse(int socket, parser_t * parser , long *count) {
//	uint64_t count = 0; //统计接收的数据
	uint32_t rst, size;
	for (;;) {
		int p = parser->pos;
		//		printf("reading .....\n");
		size = read(socket, parser->buffer + p, MAX_BUFFER - p - 1);
		//		printf("read size------%d\n",size);
		*count += size;
		switch (size) {
		case 0:
//			printf("%u-->数据接收完毕:大小(%ld)\n", (unsigned int) pthread_self(),
//					*count);
			return 0;
			break;
		case -1:
			switch (errno) {
			case EAGAIN:
				// Nothing to read now, we'll try again later
				break;
			default:
				return 1;
				break;
			}
			break;
		default:
			/*进行解析处理*/
			rst = parser->parse_enter(parser, size);
			switch (rst) {
			case 0:
				break;
			case 1:
				/*错误处理*/
				//				printf("出错 parse_enter\n");
				return 1;
			case 2:
				return 0;
			}
			break;
		}
	}
	return 0;

}
