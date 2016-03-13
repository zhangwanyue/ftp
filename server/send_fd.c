#include "head.h"

void send_fd(int sfd,int fd)//将文件描述符fd发送给端口sfd
{
	struct msghdr msg;
	bzero(&msg,sizeof(struct msghdr));
	char buf1[10]="hello";//没有什么用但是必须写，不然编译会报错
	char buf2[10]="world";
	struct iovec iov[2];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	int *fdptr;
	fdptr=(int*)CMSG_DATA(cmsg);
	*fdptr=fd;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	int ret=sendmsg(sfd,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
}


void recv_fd(int sfd,int* fd)//从sfd端口中接收文件描述符fd
{
	struct msghdr msg;
	bzero(&msg,sizeof(struct msghdr));
	char buf1[10]={0};
	char buf2[10]={0};
	struct iovec iov[2];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len = len;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control = cmsg;
	msg.msg_controllen = len;
	int ret=recvmsg(sfd,&msg,0);
	if(-1==ret)
	{
		perror("recvmsg");
		return;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
	//printf("the buf1 is %s\n",buf1);
}

