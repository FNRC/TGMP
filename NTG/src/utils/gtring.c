/*
 * gtring.c
 *
 *  Created on: Nov 28, 2014
 *      Author: tangzhihua
 */
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "gstring.h"

#define MAX(x,y) (x > y ? x : y)
#define MIN(x,y) (x > y ? y : x)

/*
 * 去除字符串前后的空格
 */
char * gstrim(char *str) {
	char *end, *sp, *ep;
	size_t len;

	sp = str;
	end = ep = str + strlen(str) - 1;
	while (sp <= end && isspace(*sp))
		sp++;
	while (ep >= sp && isspace(*ep))
		ep--;
	len = (ep < sp) ? 0 : (ep - sp) + 1;
	sp[len] = '\0';
	return sp;
}

/*
 * 去除字符串前导空格或\t
 */
char * gstrim_st(char *str) {
	char *end, *rst;

	rst = str;
	end = str + strlen(str) - 1;
	while (rst <= end && (isblank(*rst)))
		rst++;
	return rst;
}

/*
 * 数字字符串转为相应的数值
 * 注:可以去掉前导的非数字字符
 */
int gstrtonum(char *str) {
	char *p, *end;
	int rst = 0;

	p = str;
	end = str + strlen(str) - 1;
	while (p <= end && (!isdigit(*p)))
		p++;
	if (p > end)
		return -1;
	do {
		rst = rst * 10 + (*p) - '0';
		p++;
	} while (p <= end && isdigit(*p));

	return rst;
}

/*
 * 字符串前移函数
 * 注：内存域有重叠
 */
int gstrbmove(char *dest, char * start) {
	int i = 0;
	while (start[i] != '\0') {
		dest[i] = start[i];
		i++;
//		start[i++] = '\0';
	}
	dest[i] = '\0';
	return i;//放回实际大小
}
/*
 * 16进制字符串转10进制
 */
int gstrhextonum(char * s) {
	int i = 0, n, temp = 0;
//	printf("hex--------->");
	while (1) {
		if (s[i] >= 'A' && s[i] <= 'F')
			n = s[i] - 'A' + 10;
		else if (s[i] >= 'a' && s[i] <= 'f')
			n = s[i] - 'a' + 10;
		else if (s[i] >= '0' && s[i] <= '9')
			n = s[i] - '0';
		else {
//			printf("\n");
			return temp;
		}
//		printf("%c",s[i]);
		temp = temp * 16 + n;
		i++;
	}
	return temp;
}

