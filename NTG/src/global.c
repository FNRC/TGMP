/*
 * global.c
 *
 *  Created on: Oct 16, 2014
 *      Author: tangzhihua
 */

#include "global.h"
#include <assert.h>
///*
// * 用户数
// */
int user_num = 1000;
int page_num = 3;



static void aq_init(a_queue * aq,int type,  int cycle);
static user_t * search_insert_point(user_t * head, user_t * end, int value);
static user_t * user_init(user_t * user, int id, int type, int time);
/*
 * init() 初始化函数
 * 参数：无
 * 返回：无
 *
 */
void init(void) {

	/*为离线队列和浏览队列分配空间*/
	offline_queue = calloc(1, sizeof(a_queue));
	browse_queue = calloc(1, sizeof(a_queue));
	/*初始化队列*/
	aq_init(offline_queue,1 ,OFF_CYCLE);
	aq_init(browse_queue,0 ,BR_CYCLE);

	/*给用户分配内存空间*/
	user_t *puser = calloc(USER_MAX, sizeof(user_t));
	/*初始化用户并加入队列*/
	srand((unsigned) time(NULL));
	int i = 0;
	for (; i < USER_MAX; ++i) {
		int time = 1 + rand() % UM_TIME;/*生成[1,720]间的随机数*/
		user_init(puser, i, WEB_USER, time);/*初始化用户*/
		/*根据用户状态，将用户分别插入到相应队列*/
		if ((time > UM_BROWSE_TIME)&& (offline_queue->cur_size < USER_MAX/4)) {/*大于UM_BROWSE_TIME（ms）将用户加入到离线队列*/
			insert_queue(offline_queue, Q_A, puser);
		} else {
			insert_queue(browse_queue, Q_A, puser);
		}
		++puser;
	}
}

/*
 * aq_init() A队列初始化
 * 参数：
 * 		aq--> 队列指针
 * 返回：无
 *
 */
static void aq_init(a_queue * aq,int type, int cycle) {
	pthread_mutex_init(&aq->mutex, NULL);
	pthread_cond_init(&aq->cond, NULL);
	aq->head.next = NULL;
	aq->tail = aq->tag_user = NULL;
	aq->cur_size = aq->temp_value = aq->shutdown = 0;
	aq->type = type;
	aq->cycle = cycle;
}

/*
 *user_init() 初始化用户的属性
 *参数：
 *		user-->用户的指针(已分配内存空间)
 *		id-->用户的id
 *		type-->用户的类别(WEB_USER和STREAM_USER)
 *		state-->用户的状态
 *		url-->访问的URL
 *		time-->初始化用户的浏览时间
 *	返回：用户的指针
 *	注：在分配用户空间下调用，
 */
static user_t * user_init(user_t * user, int id, int type, int time) {
	user->id = id;
	user->type = type;
	user->time = time;
	if (time > UM_BROWSE_TIME)
		user->state = US_OFFLINE;
	else
		user->state = US_BROWSE;

	if (type == WEB_USER) {/*如果为Web用户*/
		user->ops = &web_user_ops;/*初始化用户的操作函数*/
	} else {/*为流媒体用户*/

	}
	user->next = NULL;
	return user;
}

/*
 * set_user_property() 设置用户的属性
 * 参数：
 * 		user-->用户指针
 * 		p_type-->用户的属性类型
 * 		value-->属性值
 * 	返回：空
 * 	注：此处采用倒序的方式，按修改频率排序
 */
void set_user_property(user_t *user, int p_type, void * p_value) {

	switch (p_type) {
	case P_TIME:
		user->time = *(int *) p_value;
		break;
	case P_URL:
		strcpy(user->url, p_value);
		break;
	case P_STATE:
		user->state = *(int *) p_value;
		break;
	case P_TYPE:
		user->type = *(int*) p_value;
		break;
	case 1:
		break; /*用户的id属性一般不用设置，此处不提供设置*/
	default:
		break;
	}
}

/*
 *insert_queue()函数		将指定元素依user对象的time值
 *按升序插入到队列。
 *参数：
 *		queue-->队列指针
 *		type-->队列的类型（Q_A和Q_B）
 *		user-->插入对象指针
 *	返回：	void
 */
void insert_queue(void * queue, int type, user_t* user) {

	switch (type) {
	case Q_A: {
		a_queue* a_q = queue;
		pthread_mutex_lock(&a_q->mutex);
		int size = a_q->cur_size++;
		/*user对象插入队列*/
		if (size == 0) { /*队列为空时*/
			a_q->head.next = user;
			a_q->tail = user;/*修改队列的tail属性*/
			pthread_mutex_unlock(&a_q->mutex);
			pthread_cond_signal(&a_q->cond);/*通知定时刷新线程*/
			return;
		}
		/*队列不为空时*/
		/*获取元素的插入位置*/
		user_t * ip = search_insert_point(&a_q->head, a_q->tail, user->time);
		if (ip == a_q->tail) {/*当插入队列尾部时*/
			ip->next = user;
			a_q->tail = user;
		} else {
			user->next = ip->next;
			ip->next = user;
		}
		pthread_mutex_unlock(&a_q->mutex);
	}
		break;
	case Q_B: {/*线程池队列*/
		b_queue * thread_pool = queue;
		pthread_mutex_lock(&thread_pool->mutex);
		user_t *member = thread_pool->head;
		if (member != NULL) { /*等待队列不为空*/
			thread_pool->tail->next = user;
			thread_pool->tail = user;
		} else { /*等待队列为空*/
			thread_pool->head = user;
			thread_pool->tail = user;
		}
		thread_pool->cur_size++;
		assert(thread_pool->cur_size != 0);
		assert(thread_pool->head != NULL);
		pthread_mutex_unlock(&(thread_pool->mutex));
		/*好了，等待队列中有任务了，唤醒一个等待线程；
		 注意如果所有线程都在忙碌，这句没有任何作用*/
		pthread_cond_signal(&(thread_pool->ready));
	}
		break;
	}
	return;
}

/*
 * search_insert_point() 函数在一个按升序排列的单链表（head）
 * 查找value值在链表中的合适插入位置，保证该链表依旧按升序排列
 * 参数：
 * 		head -->单链表头指针
 * 		end -->链表的最后一个元素指针
 * 		value -->查找值
 * 返回：插入位置
 *
 * 注：单链表只能向后插入,该函数是非线程安全的
 */
static user_t * search_insert_point(user_t * head, user_t * end, int value) {
	/*这两条语句可以不用*/
	if (end->time <= value)
		return end;/*如果比最后一个还要大就直接发后*/

	user_t* tmp_user = head;
	while (tmp_user != end) {/*已比较到最后一个退出循环*/
		if (tmp_user->next->time >= value) {
			return tmp_user;
		}
		tmp_user = tmp_user->next;
	}
	return end;/*不可能执行到这一步*/
}

