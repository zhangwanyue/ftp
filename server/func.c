#include "head.h"
void myprint(struct stat str,char name[],int fd)
{
	char file_mode[11];
	int i=0;
	char week[5];
	char mon[5];
	char day[5];
	char time[10];
	char year[5];
	char time1[10];
	bzero(week,5);
	bzero(mon,5);
	bzero(day,5);
	bzero(time,10);
	bzero(year,5);
	bzero(time1,10);

	bzero(file_mode,sizeof(file_mode));
	//printf("%u\n",str.st_mode);
	//控制文件类型和文件权限的输出
	if(S_ISREG(str.st_mode))
		file_mode[0]='-';
	else if(S_ISDIR(str.st_mode))
		file_mode[0]='d';
	else if(S_ISCHR(str.st_mode))
		file_mode[0]='c';
	else if(S_ISBLK(str.st_mode))
		file_mode[0]='b';
	else if(S_ISFIFO(str.st_mode))
		file_mode[0]='p';
	else if(S_ISLNK(str.st_mode))
		file_mode[0]='l';
	else if(S_ISSOCK(str.st_mode))
		file_mode[0]='s';	
	for(i=0;i<9;i++)
	{
		if(str.st_mode%2==1)
		{
			if(i%3==0)
			{
				file_mode[9-i]='x';
			}else if(i%3==1)
			{
				file_mode[9-i]='w';
			}else
			{
				file_mode[9-i]='r';
			}
		}
		else
		{
			file_mode[9-i]='-';
		}
		str.st_mode=str.st_mode/2;
	}

	//控制年月日
	sscanf(ctime(&str.st_mtime),"%s %s %s %s %s",week,mon,day,time,year);
	for(i=0;i<5;i++)
		time1[i]=time[i];
	char buf[100];
	bzero(buf,sizeof(buf));
	sprintf(buf,"%s %2lu %s %s %4ld %s %s %s %s",file_mode,str.st_nlink,getpwuid(str.st_uid)->pw_name,getgrgid(str.st_gid)->gr_name,str.st_size,mon,day,time1,name);
	printf("ls buf is:%s\n",buf);
	send(fd,buf,strlen(buf),0);//发送回客户端

}

void myls(char argv[],int fd)
{
	struct stat str;
	struct dirent* p;
	DIR* dir;
	char buf[128];
	
	stat(argv,&str);
	//如果是目录文件，打印目录文件下的文件
	if(S_ISDIR(str.st_mode))
	{
		dir=opendir(argv);
		while(p=readdir(dir))
		{
			sprintf(buf,"%s%s%s",argv,"/",p->d_name);
			stat(buf,&str);
			myprint(str,p->d_name,fd);
		}
	}
	//如果不是目录文件，直接打印
	else
	{
		myprint(str,argv,fd);
	}	
}

void mygets(char argv[],int fd)//下载（在服务器端发送给客户端）
{
	data buf;
	buf.len=strlen(argv);//argv中存储的是文件名
	strcpy(buf.buf,argv);
	send(fd,&buf,buf.len+4,0);//发送文件名（文件名长度和文件名一起发过去）
	int ffd=open(argv,O_RDONLY);
	while(bzero(&buf,sizeof(buf)),(buf.len=read(ffd,buf.buf,sizeof(buf.buf)))>0)
	{
		send_n(fd,(char*)&buf,buf.len+4);//发送文件内容（文件内容长度和文件内容一起发过去）
	}
	close(ffd);
	bzero(&buf,sizeof(buf));
	int flag=0;
	buf.len=sizeof(int);
	memcpy(buf.buf,&flag,4);
	send(fd,&buf,buf.len+4,0);//发送4字节的0表示结束
}

void send_n(int fd,char* buf,int len)
{
	int total=0;
	int size=0;
	while(total<len)
	{
		size=send(fd,buf+total,len-total,0);//size是真实send过去的数值
		total=total+size;//total是一共send过去的数值
	}
}

void myputs(char argv[],int fd)//上传（在服务器端下载）
{
	data buf;
	int flag=0;
	bzero(&buf,sizeof(buf));
	recv(fd,&buf.len,4,0);//接收文件名
	recv(fd,buf.buf,buf.len,0);
	printf("buf.buf is:%s\n",buf.buf);
	int ffd=open(buf.buf,O_RDWR|O_CREAT,0666);//打开或新建文件
	while(1)
	{
		bzero(&buf,sizeof(buf));
		recv(fd,&buf.len,4,0);//接收文件内容
		recv(fd,buf.buf,buf.len,0);
		printf("2 buf.buf is:%s\n",buf.buf);
		if(4==buf.len&&!memcmp(buf.buf,&flag,4))//如果是4字节的0则是结束标志
			break;
		write(ffd,buf.buf,buf.len);//写入新建的文件之中
	}
}

void mycd(char argv[],int fd)//cd，返回cd后的当前路径
{
	chdir(argv);
	char buf[50];
	bzero(buf,sizeof(buf));
	getcwd(buf,sizeof(buf));
	printf("pwd is:%s\n",buf);
	send(fd,&buf,strlen(buf),0);//send给客户端当前路径
}

void myremove(char argv[],int fd)//remove，返回success
{
	remove(argv);
	char buf[50];
	bzero(buf,sizeof(buf));
	strcpy(buf,"remove success\n");
	send(fd,&buf,strlen(buf),0);//send给客户端success
}

void mypwd(char argv[],int fd)
{
	char buf[50];
	bzero(buf,sizeof(buf));
	getcwd(buf,sizeof(buf));
	send(fd,&buf,strlen(buf),0);
}
	
			





