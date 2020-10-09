#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define PORTNUM 80
#define MAXLINE 1024
#define HEADERSIZE 1024

#define SERVER "myserver/1.0 (Linux)"

struct user_request
{
	char method[20];   // 요청 방법
	char page[255];    // 페이지 이름
	char http_ver[80]; // HTTP 프로토콜 버전 
};

char root[MAXLINE];

int webserv(int sockfd);
int protocol_parser(char *str, struct user_request *request);
int sendpage(int sockfd, char *filename, char *http_ver, char *codemsg);


int main(int argc, char **argv)
{
	int listenfd;
	int clientfd;
	socklen_t clilen;
	int pid;
	int optval = 1;
	struct sockaddr_in addr, cliaddr;

	if(argc !=2 )
	{
		printf("Usage : %s [root directory]\n", argv[0]);
		return 1;
	}
	memset(root, 0x00, MAXLINE);
	sprintf(root, "%s", argv[1]);

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		return 1;
	}

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	memset(&addr, 0x00, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORTNUM);

	if(bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		return 1;
	}
	if(listen(listenfd, 5) == -1)
	{
		return 1;
	}

	signal(SIGCHLD, SIG_IGN);
	while(1)
	{
		clilen = sizeof(clilen);
		clientfd = accept(listenfd , (struct sockaddr *)&cliaddr, &clilen);
		if(clientfd == -1)
		{
			return 1;
		}
		pid = fork();
		if(pid == 0)
		{
			webserv(clientfd);
			close(clientfd);
			exit(0);
		}
		if(pid == -1)
		{
			return 1;
		}
		close(clientfd);
	}
}

int webserv(int sockfd)
{
	char buf[MAXLINE];
	char page[MAXLINE];
	struct user_request request;

	memset(&request, 0x00, sizeof(request));
	memset(buf, 0x00, MAXLINE);

	if(read(sockfd, buf, MAXLINE) <= 0)
	{
		return -1;
	}
	protocol_parser(buf, &request);
	if(strcmp(request.method, "GET") == 0)
	{
		sprintf(page, "%s%s", root, request.page);
		if(access(page, R_OK) != 0)
		{
			sendpage(sockfd, NULL, request.http_ver, "404 Not Found");
		}
		else
		{
			sendpage(sockfd, page, request.http_ver, "200 OK");
		}
	}
	else
	{
		sendpage(sockfd, NULL, request.http_ver, "500 Internal Server Error");
	}
	return 1;
}

int sendpage(int sockfd, char *filename, char *http_ver, char *codemsg)
{
	struct tm *tm_ptr;
	time_t the_time;
	struct stat fstat;
	char header[HEADERSIZE];
	int fd; 
	int readn;
	int content_length=0;
	char buf[MAXLINE];
	char date_str[80];
	char *daytable = "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0";
	char *montable = "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0";

	memset(header, 0x00, HEADERSIZE);

	time(&the_time);
	tm_ptr = localtime(&the_time);

	sprintf(date_str, "%s, %d %s %d %02d:%02d:%02d GMT",
			daytable+(tm_ptr->tm_wday*4),
			tm_ptr->tm_mday,
			montable+((tm_ptr->tm_mon)*4),
			tm_ptr->tm_year+1900,
			tm_ptr->tm_hour,
			tm_ptr->tm_min,
			tm_ptr->tm_sec
			);

	if(filename != NULL)
	{   
		stat(filename, &fstat);
		content_length = (int)fstat.st_size;
	}   
	else	
	{   
		content_length = strlen(codemsg);
	}
	sprintf(header, "%s %s\nDate: %s\nServer: %s\nContent-Length: %d\nConnection: close\nContent-Type: text/html; charset=UTF8\n\n",
		http_ver, date_str, codemsg, SERVER, content_length);
	write(sockfd, header, strlen(header));

	if(filename != NULL)
	{
		fd = open(filename, O_RDONLY);
		memset(buf, 0x00, MAXLINE);
		while((readn = read(fd,buf,MAXLINE)) > 0)
		{
			write(sockfd, buf, readn);
		}
		close(fd);
	}
	else
	{
		write(sockfd, codemsg, strlen(codemsg));
	}

	return 1;
}

int protocol_parser(char *str, struct user_request *request)
{
	char *tr;
	char token[] = " \r\n";
	int i;
	tr = strtok(str, token);
	for (i = 0; i < 3; i++)
	{
		if(tr == NULL) break;
		if(i == 0)
			strcpy(request->method, tr);
		else if(i == 1)
			strcpy(request->page, tr);
		else if(i == 2)
			strcpy(request->http_ver, tr);
		tr = strtok(NULL, token);
	}
	return 1;
}
