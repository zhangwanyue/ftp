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
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

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
void send_fd(int,int);
void recv_fd(int,int*);
void make_child(pchild,int);
void child_handle(int);
void myls(char*,int);
void mygets(char*,int);
void send_n(int,char*,int);
void myputs(char*,int);
