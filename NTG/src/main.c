/*
 * 部署虚拟实验平台,
 * */
#include <zdb.h>
#include "global.h"
#include "lib/unp.h"
#include "utils/threadpool.h"
#include <signal.h>
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/tcp.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <linux/sched.h>
#include <string.h>
#include<sys/timeb.h>
#include <mysql/mysql.h>
static void print_a_property(a_queue * queue);
void * signal_thread(void *arg);
char *creat_log_t_sql(char *tableName);
sigset_t mask;
size_t strlcpy (char *dst, const char *src, size_t dst_sz);

MYSQL mysql;
MYSQL *sg_conn; // mysql 连接
MYSQL_RES *sg_res; // mysql 记录集
MYSQL_ROW sg_row; // 字符串数组，mysql 记录行
const char *sg_host_name="localhost";
const char *sg_user_name = "root";
const char *sg_password = "HPflow123";
const char *sg_db_name = "gzb";
const unsigned int sg_db_port = 3306;
//char intablename;

int main(int argc, char ** argv) {

	pthread_t tid;
	int err;

	struct sigaction sa;
	//屏蔽进程信号
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) < 0) {
		err_sys("sigaction failed");
	}

	//屏蔽线程信号
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);
	sigaddset(&mask, SIGTERM);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0)) {
		err_sys("pthread_sigmask failed");
	}
	if ((err = pthread_create(&tid, NULL, signal_thread, NULL) != 0)) {
		err_quit("can't create thread");
	}



/**
 * 开始读取配置文件
 */

		FILE *configfp;
		FILE *configfpbuf;
		int i;
		char ip[20]={0};
    	char configmsg[500]={0};
	     char configmsgbuf[500]={0};
	     int configi = 0, configj = 0;
	     char *configp[20]= {NULL};
	     char *configbuf=NULL;
	     char *outer_ptr=NULL;

	    if((configfp=fopen("/home/zhs/config.txt","r"))==NULL){
	    	printf("不能打开文件：/home/zhs/config.txt\n");
	    	exit(0);
	    }
	    fgets(configmsg,sizeof(configmsg),configfp);//从文件中获得一串字符到buf中.
	    printf("configmsg=%s\n",configmsg);//打印buf.
	     configbuf=configmsg;

	     while((configp[configi]=strtok_r(configbuf,"*",&outer_ptr))!=NULL)
	      {
	         configi++;
	         configbuf=NULL;
	       }
	     printf("web-IP-is-%s\n",configp[6]);//输出分割字符串，测试用
	     strlcpy(ip,configp[1],sizeof(ip));//中间层坐在的IP地址（数据库）
	//	printf("ip=%s\n",ip);
		 printf("vweb-num-is-%s\n",configp[2]);
		 int num=atoi(configp[2]);
		 //strlcpy(num,configp[1],sizeof(num)); //配置的虚拟用户数
		 printf("Vweb-num=%d\n",num);
/**
 * 读取配置文件按结束
 */

		 /**
		  * 上传本地时间
		  */
		 /*
		 		int cfd,oo;
		 	     int recbytes;
		 	     int sin_size;
		 	     char buffer[1024]={0};
		 	     struct sockaddr_in s_add,c_add;
		 	     unsigned short portnum=7001; //需要连接的端口号
		 	     char ch;
		 	     //int i;
		 	    struct timeb tp;
		 	     ftime(&tp);
		 	     char data[100];
		 	     printf("Hello,welcome to client !\r\n");
		 	     cfd = socket(AF_INET, SOCK_STREAM, 0);///创建socket
		 	     if(-1 == cfd)
		 	      {
		 	             printf("socket fail ! \r\n");
		 	             return -1;
		 	       }
		 	     printf("socket ok !\r\n");
		 	     bzero(&s_add,sizeof(struct sockaddr_in));
		 	     s_add.sin_family=AF_INET;
		 	     s_add.sin_addr.s_addr= inet_addr(ip);//此处绑定web存放的服务器IP地址
		 	     s_add.sin_port=htons(portnum);
		 	     printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);
		 	    oo=connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr));
		 	     if(oo)///连接服务器端
		 	       {
		 	         printf("connect fail !\r\n");
		 	         return -1;
		 	       }
		 	     printf("connect ok !\r\n");
		 	     memset(data,0,100);
		 	     sprintf(data, "%d*",tp.time);
		 	     printf( "time---%d\n ",tp.time);
		 	     printf("buf=%s\n", data);//打印buf
		 	     write(cfd,data,strlen(data));
		 	     printf("the--send--data---is:%s\n",data);
		 	     memset(data,0,100);
		 	     close(oo);
		 	     close(cfd);
*/
		 /**
		  * 上传本地时间结束
		  */

		 /*使用常规方法创建数据表单*/
//		 long hostip=gethostid();
//		 printf("hostip=%s",hostip);





		 char tablename[30]="log_t_";
		 strcat(tablename,ip);//追加读取配置文件中的表单IP-------------------------------
//		 intablename=*tablename;
		 printf("tablename=：%s\n",tablename);
//		 printf("intablename=：%s\n",intablename);

//		intablename=tablename;
//		printf("intablename=：%s\n",intablename);
		if (sinit_mysql()){ //建立数据库连接！！！！！！！！！！！
				  //print_mysql_error(NULL);
				printf("mysql初始换化成功\n");
		  }

		char sql[255]="CREATE TABLE IF NOT EXISTS `";
		strcat(sql,tablename);
		char sql1[] =
				"` (`id` int(11) NOT NULL AUTO_INCREMENT,`user_id` int(11) DEFAULT NULL,`type` varchar(1) DEFAULT NULL,`url` varchar(255) DEFAULT NULL,`rsp_time` varchar(15) DEFAULT NULL,`data_size` bigint(20) DEFAULT NULL,`time` int(50) DEFAULT NULL, PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1";
		strcat(sql,sql1);
//		char logsql=*creat_log_t_sql(tablename);
		printf("sql=%s\n",sql);
	    if(sexecutesql(sql)){
//	    	print_mysql_error(NULL);
	    	printf("创建表单失败\n");
	    }else{
	    	printf("创建表单成功\n");
	    }
	    mysql_close(sg_conn);







	 user_num = num;///模拟的虚拟用户数
	 printf("user_num=%d\n",user_num);
//	fprintf(fp,"%s=%s\n",items[i].key, items[i].value);
/**
 *追加字符串
 */
//		char mysqlStr[255]="mysql://";
//
//		strcat(mysqlStr,ip);
//		char str1[]="/gzb?user=root&password=HPflow123";
//		strcat(mysqlStr,str1);
//
//		printf("%s\n",mysqlStr);

/**
 * 追加字符串结束
 */



	init();
	pool = init_connection_pool(
			"mysql://localhost/gzb?user=root&password=HPflow123");
	printf("123456");
	printf("\n");
//	pool = init_connection_pool(mysqlStr);
	pool_init(MAX_THREAD_NUM);

//	log_pool = init_connection_pool("mysql://localhost/userlog?user=root&password=123456");
//	pool_init(MAX_THREAD_NUM);
	//print_a_property(offline_queque);
	//print_a_property(browse_queue);

	pthread_t *a = calloc(1, sizeof(pthread_t));
	pthread_t *b = calloc(1, sizeof(pthread_t));
	pthread_create(a, NULL, scan_routine, offline_queue);
	pthread_create(b, NULL, scan_routine, browse_queue);

	for (;;) {

	}

	return 0;
}

void * signal_thread(void *arg) {
//	printf("signal thread have already start\n");
	fflush(NULL);
	int err, signo;
	for (;;) {
		err = sigwait(&mask, &signo);
//		printf("singo --->%d\n", signo);
		if (err != 0)
			err_quit("sigwait failed:%s", strerror(err));
		switch (signo) {
		case SIGHUP:
			break;
		case SIGTERM:
			break;
		default:
			err_quit("unexpected signal %d", signo);
		}
	}
	return 0;
}

void print_a_property(a_queue * queue) {
	printf("%u,\t%d,\t%d, \t%d, \t%d\n", (unsigned int) queue->head.next,
			queue->cycle, queue->temp_value, queue->shutdown, queue->cur_size);
	return;
}

size_t strlcpy (char *dst, const char *src, size_t dst_sz)
{
    size_t n;
    for (n = 0; n < dst_sz; n++)
     {
       if ((*dst++ = *src++) == '\0')
          break;
     }
    if (n < dst_sz)
    return n;
    if (n > 0)
    *(dst - 1) = '\0';
    return n + strlen (src);
}

/*常规链接mysql*/
int sinit_mysql() { // 初始化连接
   sg_conn = mysql_init(&mysql);
  /* connect the database */
  if(!mysql_real_connect(sg_conn, sg_host_name, sg_user_name, sg_password, sg_db_name, sg_db_port, NULL, 0)) // 如果失败
        return -1;
  // 是否连接已经可用
  if (sexecutesql("set names utf8")) // 如果失败
      return -1;
  printf("init_mysql--is--success1111111111111111!!!!\n");
    return 0; // 返回成功
}
/*执行SQL语句*/
int sexecutesql(const char * sql) {
    /*query the database according the sql*/
    if (mysql_real_query(sg_conn, sql, strlen(sql))) // 如果失败
   // if (mysql_real_query(g_conn, sql, 80)) // 如果失败
     return -1; // 表示失败
   printf("executesql--is--success2222222222222222222!!!!\n");
    return 0; // 成功执行
}

/*创建基于本机IP的表单*/
char *creat_log_t_sql(char *tableName){
	char sql[255]="CREATE TABLE IF NOT EXISTS ";
	strcat(sql,tableName);
	char sql1[] =
			" (`id` int(11) NOT NULL AUTO_INCREMENT,`user_id` int(11) DEFAULT NULL,`type` varchar(1) DEFAULT NULL,`url` varchar(255) DEFAULT NULL,`rsp_time` varchar(15) DEFAULT NULL,`data_size` bigint(20) DEFAULT NULL,`time` int(50) DEFAULT NULL, PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1";
	strcat(sql,sql1);
//	printf("%s\n",sql);
	return sql;
}







