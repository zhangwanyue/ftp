#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>


typedef struct childs{
	pid_t pid;
	int fds;
	int busy;
}child,*pchild;

typedef struct fdata{
	int len;
	char buf[10000];
}data,*pdata;

struct command{
	int len;
	char word[50];
};

void send_n(int,char*,int);
