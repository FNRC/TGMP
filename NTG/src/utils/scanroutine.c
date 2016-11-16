/*
 * scanroutine.c
 *
 *  Created on: Sep 4, 2014
 *      Author: tangzhihua
 */
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include "../global.h"
#include "../dbi/ntgs_mysql.h"

//static void print_user(user_t *user);
/*
 *scan_routine() 扫描例程
 *参数：arg -> 一个指向a_queue的指针
 *返回：无
 */
void * scan_routine(void *arg) {
	user_t *user = NULL;
	int size;
	a_queue *queue = (a_queue*) arg;
	struct timespec tm;
	struct timeval tend, tstart;
	long count=0;
	double usec;

	/*线程无限环*/
	for (;;) {
		/*开始时间*/
//		if (gettimeofday(&tstart, NULL) == -1) {
//			fprintf(stderr, "Failed to get start time\n");
//		}
		/*休眠指定时间*/
		tm.tv_sec = queue->cycle/1000;
		tm.tv_nsec = (queue->cycle%1000)*1000000;
		nanosleep (&tm, NULL);
		int flag = 1;
		while(queue->type == 1 && flag) {
			Connection_T con = ConnectionPool_getConnection(pool);
			if (con == NULL){
				continue;
			}
//			insert_wait_user(con, USER_MAX, queue->cur_size, offline_queue->cur_size);
//			insert_user(con, queue->cur_size);
			Connection_close(con);
			flag = 0;
		}

//		sleep(queue->cycle);
//		printf("sleep 完成---------------------------\n");
		fflush(stdout);
		pthread_mutex_lock(&queue->mutex);
		while (queue->cur_size == 0)
			pthread_cond_wait(&queue->cond, &queue->mutex);
		/******************************/
//		count++;
//		if (gettimeofday(&tstart, NULL) == -1) {
//			fprintf(stderr, "Failed to get start time\n");
//		}
		/*****************************/

		/***********************************************/
//		if (gettimeofday(&tend, NULL) == -1) {
//			fprintf(stderr, "Failed to get end time\n");
//		}
//		usec = (tend.tv_sec - tstart.tv_sec)*1000000 + ( tend.tv_usec - tstart.tv_usec);
//		printf("scanroutine-----1233-----time: %.6f(ms)\n",usec/1000 );
		/***********************************************/


		assert(queue->cur_size != 0);
		assert(queue->head.next != NULL);
		/*遍历所有成员使time值减一*/
		user = queue->head.next;
		/*
		 * 	此处可以采用一种整体删除和整体插入的方式优化设计
		 * 	多用户插入时，如何唤醒多个线程
		 */
		while (user != NULL) {/*还有用户*/

			user->time -= (queue->cycle);
			if (user->time <= 0) {/*用户的浏览时间结束*/

				queue->head.next = user->next;/*从队列中删除用户*/
//				printf("当前大小--------->%d\n", queue->cur_size);
				fflush(stdout);
				size = queue->cur_size--;
				user->next = NULL;
				insert_queue(pool_queue, Q_B, user);/*将用户转移到线程池中的等待队列*/
//				printf("扫描周期%d\t", queue->cycle);
//				//	print_user(user);
//				if (size == 0)
//					break;
				user = queue->head.next;/*保存下一个扫描用户*/
				continue;
			}
			user = user->next;/*指向下一个用户*/
		}

		/**
		 * 写入离线用户数
		 */



		/***********************************************/
//		if (gettimeofday(&tend, NULL) == -1) {
//			fprintf(stderr, "Failed to get end time\n");
//		}
//		usec = (tend.tv_sec - tstart.tv_sec)*1000000 + ( tend.tv_usec - tstart.tv_usec);
//		printf("scanroutine----------time: %.6f(ms)\n",usec/1000 );
		/***********************************************/
//		printf("扫描结束后队列大小--------->\t%d\n", queue->cur_size);
		fflush(stdout);
		pthread_mutex_unlock(&queue->mutex);
	}
	/*这一句应该是不可达的*/
	pthread_exit(NULL);
}
//void print_user(user_t *user) {
//	printf("%d, \t%d, \t%d, \t%d,\t%x,  \t%s\n", user->id, user->state,
//			user->type, user->time, (unsigned int) user->next, user->url);
//}
