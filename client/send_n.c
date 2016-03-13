#include "head.h"
void send_n(int fd,char* buf,int len)
{
	int total=0;
	int size=0;
	while(total<len)
	{
		size=send(fd,buf+total,len-total,0);
		total=total+size;
	}
}
