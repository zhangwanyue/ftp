#include "head.h"
void make_child(pchild p,int n)
{
	int fds[2];
	int pid;
	int i;
	int ret;
	for(i=0;i<n;i++)
	{
		ret=socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
		if(-1==ret)
		{
			perror("socketpair");
			return;
		}
		pid=fork();
		if(pid==0)
		{
			close(fds[1]);//关闭子进程的一端
			child_handle(fds[0]);//另一端用来读
		}
		close(fds[0]);//父进程也关闭一端(另一端用来写)
		p[i].pid=pid;
		p[i].fds=fds[1];//父进程的写端（对应每个子进程的读端）
		p[i].busy=0;
	}
}

void child_handle(int fdr)//子进程的读端
{
	struct command com;
	int fd;
	char nbuf[20];
	recv_fd(fdr,&fd);
	while(1)
	{
		bzero(&com,sizeof(com));
		recv(fd,&com.len,4,0);//收到客户端的操作命令长度
		recv(fd,com.word,com.len,0);//收到客户端的操作命令
		printf("child recv is %s\n",com.word);
		bzero(nbuf,sizeof(nbuf));
		if(0==strncmp(com.word,"ls",2))
		{
			strcpy(nbuf,com.word+3);
			printf("new nbuf is:%s\n",nbuf);
			myls(nbuf,fd);
		}
		else if(0==strncmp(com.word,"gets",4))
		{
		
			strcpy(nbuf,com.word+5);
			printf("new nbuf is:%s\n",nbuf);
			mygets(nbuf,fd);
		}
		else if(0==strncmp(com.word,"puts",4))
		{
			strcpy(nbuf,com.word+5);
			printf("new nbuf is:%s\n",nbuf);
			myputs(nbuf,fd);
		}else if(0==strncmp(com.word,"cd",2))
		{
			strcpy(nbuf,com.word+3);
			printf("new nbuf is:%s\n",nbuf);
			mycd(nbuf,fd);
		}else if(0==strncmp(com.word,"remove",6))
		{
			strcpy(nbuf,com.word+7);
			printf("new nbuf is:%s\n",nbuf);
			myremove(nbuf,fd);
		}else if(0==strncmp(com.word,"pwd",3))
		{
			strcpy(nbuf,com.word+4);
			printf("new nbuf is:%s\n",nbuf);
			mypwd(nbuf,fd);
		
		}else if(0==(strncmp(com.word,"exit",4)))//如果客户端关闭了
		{
			int flag=1;
			write(fdr,&flag,4);
			break;		
		}else
		{
			char rbuf[20];
			bzero(rbuf,sizeof(rbuf));
			strcpy(rbuf,"command error!");//如果是其他的则返回命令错误消息
			send(fd,&rbuf,strlen(rbuf),0);
		}
	}
	close(fd);
}
	
		

