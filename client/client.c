#include "head.h"
int pid;
int cfd;
void sig(int sig_id)
{
	send(cfd,"4",4,0);
	send(cfd,"exit",4,0);//发送exit给服务器
//	close(cfd);
//	close(0);
	kill(pid,SIGQUIT);
}
int main(int argc,char* argv[])
{
	if(argc != 3)//两个参数：ip 端口号
	{
		printf("error args!\n");
		return -1;
	}
	//注册信号量
//	signal(SIGINT,sig);
	//pid
	pid=getpid();
	//socket函数
	cfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in client;
	bzero(&client,sizeof(client));
	client.sin_family=AF_INET;
	client.sin_port=htons(atoi(argv[2]));
	client.sin_addr.s_addr=inet_addr(argv[1]);
	//connect函数
	int ret=connect(cfd,(struct sockaddr*)&client,sizeof(struct sockaddr));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	struct command com;	
	data dbuf;
	int dfd;
	int flag=0;
	char fname[20];
	while(1)
	{	
		bzero(&com,sizeof(com));
		ret=read(0,com.word,sizeof(com.word));
//		if(ret<0)
//			break;
		printf("com.word is:%s\n",com.word);
		com.len=strlen(com.word)-1;//要发过去的命令要把最后一个换行符去掉
		printf("com.len is:%d\n",com.len);		
		send(cfd,&com,com.len+4,0);//发送命令给服务器
		bzero(&dbuf,sizeof(dbuf));
		if(0==strncmp(com.word,"gets",4))//如果命令是下载
		{
			recv(cfd,&dbuf.len,4,0);//接收下载的文件名
			printf("1 dbuf.len is:%d\n",dbuf.len);
			recv(cfd,dbuf.buf,dbuf.len,0);
			printf("1 dbuf.buf is:%s\n",dbuf.buf);
			dfd=open(dbuf.buf,O_RDWR|O_CREAT,0666);//打开或新建文件
			while(1)
			{
				bzero(&dbuf,sizeof(dbuf));
				recv(cfd,&dbuf.len,4,0);//接收下载的文件内容
				recv(cfd,dbuf.buf,dbuf.len,0);
				if(4==dbuf.len&&!memcmp(dbuf.buf,&flag,4))//如果是4字节的0则是结束标志
					break;
				write(dfd,dbuf.buf,dbuf.len);//写入新建的文件之中
			}
		}
		else if(0==strncmp(com.word,"puts",4))//如果命令是上传
		{
			strncpy(fname,com.word+5,strlen(com.word)-6);
			dbuf.len=strlen(fname);
			strcpy(dbuf.buf,fname);
			printf("file name:%s\nend\n",fname);
			send(cfd,&dbuf,dbuf.len+4,0);//发送文件名给服务器端
			int ffd=open(fname,O_RDONLY);
			while(bzero(&dbuf,sizeof(dbuf)),(dbuf.len=read(ffd,dbuf.buf,sizeof(dbuf.buf)))>0)
			{
				printf("dbuf.buf is:%s\n",dbuf.buf);
				send_n(cfd,(char*)&dbuf,dbuf.len+4);//发送文件内容给服务器端
			}
			close(ffd);
			bzero(&dbuf,sizeof(dbuf));
			int flag=0;
			dbuf.len=sizeof(int);
			memcpy(dbuf.buf,&flag,4);
			send(cfd,&dbuf,dbuf.len+4,0);//发送4字节的0表示结束
		}		
		else
		{
			char buf2[100];
			bzero(buf2,sizeof(buf2));
			recv(cfd,buf2,sizeof(buf2),0);//从服务器child进程的func函数中接收结果
			printf("%s\n",buf2);
		}
	}
	close(cfd);
}
	
