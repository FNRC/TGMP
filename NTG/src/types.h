/*
 * types.h
 *
 *  Created on: Oct 16, 2014
 *      Author: tangzhihua
 */

#ifndef TYPES_H_
#define TYPES_H_

#define MAX_BUFFER 		2048//缓存区大小
#define MESSAGE_SIZE 	1024//请求消息大小

#define URL_SIZE        1024/*URL的长度限制*/
#define USER_MAX     	user_num/*模拟的用户数*/
#define MAX_PAGE_NUM page_num//最大的页面数
#define MAX_THREAD_NUM 40/*线程池中最大的线程数*/
//这些时间都是以秒为单为（毫秒）
#define UM_TIME         1500     /*最大用户时间*/
#define UM_BROWSE_TIME 	800/*用户最大浏览时间,超过这一时间为离线*/

/*
 * 队列的扫描周期(单位为毫秒)
 */
#define OFF_CYCLE  1000/*离线队列扫描周期*/
#define BR_CYCLE 1 /*浏览队列扫描周期*/

/*
 定义用户的属性类型的宏
 用户修改指定的用户属性
 */
#define P_ID  		1
#define P_TYPE  	2
#define P_STATE 	3
#define P_URL 		4
#define P_TIME		5

/*
 * 用户类型
 */
#define WEB_USER		1   /*web用户*/
#define STREAM_USER 2 /*流媒体用户*/

/*
 * 用户的状态
 */
#define	US_OFFLINE 	1/*用户的离线状态*/
#define US_BROWSE 	2/*用户的浏览状态*/
#define US_WAIT		3/*用户等待线程服务的状态*/
#define US_LIVE 	4/*用户与远程服务器交互的状态*/
/*LIVE状态的子状态*/
//根据情况再定

/*
 * 队列的类型
 */
#define Q_A			1 /*队列A是按时间的升序队列*/
#define Q_B 		2 /*队列B是等待线程服务的队列，采用先进先出策略*/



// Various reasons of error when getting a page
#define nbAnswers 16
enum Error
{
  success,
  noDNS,
  noConnection,
  forbiddenRobots,
  timeout,
  badType,
  tooBig,
  err30X,
  err40X,
  earlyStop,
  duplicate,
  fastRobots,
  fastNoConn,
  fastNoDns,
  tooDeep,
  urlDup
};

/*
 * 接收数据的类型
 */
enum data_type{
	DATA_HTTP,/*http数据*/
	DATA_RTSP,/*rtsp数据*/
};

#endif /* TYPES_H_ */
