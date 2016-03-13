#include "head.h"

int main(int argc,char* argv[])
{	
	FILE *config;
	config=fopen("conf.txt","r+");
	char addr[20],port[20];
	int num;
	int ret;
	fscanf(config,"%s%s%d",addr,port,&num);//读取配置文件
//	printf("%s\n%s\n%d\n",addr,port,num);
	pchild p;
	p=(pchild)calloc(num,sizeof(child));
	//make_child
	make_child(p,num);

	//socket函数
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in seradd;
	bzero(&seradd,sizeof(seradd));
	seradd.sin_family=AF_INET;
	seradd.sin_port=htons(atoi(port));
	seradd.sin_addr.s_addr=inet_addr(addr);
	//bind函数
	bind(sfd,(struct sockaddr*)&seradd,sizeof(struct sockaddr));
	//listen函数
	listen(sfd,num);//最大链接数为num
	//epoll_create
	int epfd=epoll_create(1);
	struct epoll_event ev,*evs;
	//epoll_ctl注册sfd
	ev.events=EPOLLIN;
	ev.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&ev);
	int i;
	for(i=0;i<num;i++)
	{
		//epoll_ctl注册每个子进程对应的端口
		ev.events=EPOLLIN;
		ev.data.fd=p[i].fds;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].fds,&ev);
	}
	int new_fd;
	int size;
	int j;
	char buf[20];
	int cfd;
	while(1)
	{
		ret=epoll_wait(epfd,evs,num+1,-1);//永久等待(返回值是准备好被监听的文件描述符数量）
		if(ret>0)
		{
			for(i=0;i<ret;i++)
			{
				if(evs[i].data.fd==sfd && evs[i].events==EPOLLIN)//如果有客户端发消息过来
				{
					new_fd=accept(sfd,NULL,NULL);
					printf("one client connect,new_fd is %d\n",new_fd);
					for(j=0;j<num;j++)
					{
						if(p[j].busy==0)
							break;
					}
					if(j!=num)
					{
						p[j].busy=1;
						printf("child %d is busy\n",j);
						send_fd(p[j].fds,new_fd);//向child发送与客户端的链接
						
					//	p[j].busy=0;
					//	printf("child %d is notbusy\n",j);
					}									
				}
				for(j=0;j<num;j++)
				{
//					printf("evs:%d p[j]:%d\n",evs[i].data.fd,p[j].fds);
					if(evs[i].data.fd==p[j].fds && evs[i].events==EPOLLIN)
				//	if(evs[i].data.fd==p[j].fds)
					{
	//					printf("i am here\n");
						size=read(p[j].fds,buf,4);//直接从子进程读取消息
						printf("server read buf is:%s\n",buf);
						p[j].busy=0;
						printf("child %d is notbusy\n",j);
					}
				}
			}
		}
	}
	return 0;
}
						
		

	





