/*
 * wrapunix.c
 *
 *  Created on: Aug 26, 2014
 *      Author: tangzhihua
 */

#include	"unp.h"

ssize_t Read(int fd, void *ptr, size_t nbytes) {
	ssize_t n;
	if ((n = read(fd, ptr, nbytes)) == -1)
		err_sys("read error");
	return (n);
}

void Write(int fd, void *ptr, size_t nbytes) {
	if (write(fd, ptr, nbytes) != nbytes)
		err_sys("write error");
}

void Close(int fd) {
	if (close(fd) == -1)
		err_sys("close error");
}

void *Malloc(size_t size) {
	void * ptr;
	ptr = malloc(size);
	if (ptr == NULL)
		err_sys("malloc error");
	return ptr;
}
void * Calloc(size_t n, size_t size) {
	void *ptr;
	ptr = calloc(n, size);
	if (ptr == NULL)
		err_sys("calloc error");
	return ptr;
}
