/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 18:24:27 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/09 18:36:46 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void* arg);
void send_data(FILE* fp, char* ct, char* file_name);
const char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(const char* message);

int main(int argc, char *argv[])
{
	int server_socket;
	struct sockaddr_in server_address;

	char buf[BUFFER_SIZE];

	fd_set read_fds;
	fd_set write_fds;
	fd_set exception_fds;
	fd_set tmp;

	int fd_max;

	struct timeval timeout;

	if (argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(stoi(std::string(argv[1])));

	if (bind(server_socket, reinterpret_cast<struct sockaddr *>(&server_address), static_cast<socklen_t>(sizeof(server_address))))
		error_handling("bind() error");
	if (listen(server_socket, 20)==-1)
		error_handling("listen() error");

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exception_fds);

	FD_SET(server_socket, &read_fds);
	FD_SET(server_socket, &write_fds);
	FD_SET(server_socket, &exception_fds);
	fd_max = server_socket;

	while(1)
	{
		int fd;
		int len;
		int client_socket;
		struct sockaddr_in client_address;
		int client_len;

		tmp = read_fds;

		timeout.tv_sec = 5;
		timeout.tv_usec = 5;

		if (select(fd_max + 1, &tmp, 0, 0, &timeout) == -1)
			std::cerr << "Select Error" << std::endl;

		for (fd = 0; fd < fd_max + 1; fd++)
		{
			if (FD_ISSET(fd, &tmp))
			{
				if (fd == server_socket)
				{
					client_len = sizeof(client_address);
					if ((client_socket = accept(server_socket, reinterpret_cast<struct sockaddr *>(&client_address), reinterpret_cast<socklen_t *>(&client_len))) == -1)
						std::cerr << "Accept Error" << std::endl;

					FD_SET(client_socket, &read_fds);
					if (fd_max < client_socket)
						fd_max = client_socket;

					std::cout << "Client Connect FD = " << client_socket << std::endl;
				}
				else
				{
					if ((len = read(fd, buf, BUFFER_SIZE)) < 0)
					{
						std::cerr<<"read error"<<std::endl;
					}
					if (len == 0)
					{
						FD_CLR(fd, &read_fds);
						close(fd);
						std::cout<<"Client Disconnect: "<<fd<<std::endl;
					}
					else
					{
						write(fd, buf, len);
					}
				}
				
			}
		}

	}
	close(server_socket);
	return 0;
}

void* request_handler(void *arg)
{
	int client_socket=*((int*)arg);
	char req_line[SMALL_BUF];
	FILE* clnt_read;
	FILE* clnt_write;
	
	char method[10];
	char ct[15];
	char file_name[30];
  
	clnt_read=fdopen(client_socket, "r");
	clnt_write=fdopen(dup(client_socket), "w");
	fgets(req_line, SMALL_BUF, clnt_read);	
	if (strstr(req_line, "HTTP/")==NULL)
	{
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return NULL;
	}
	
	strcpy(method, strtok(req_line, " /"));
	strcpy(file_name, strtok(NULL, " /"));
	strcpy(ct, content_type(file_name));
	if (strcmp(method, "GET")!=0)
	{
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return NULL;
	 }

	fclose(clnt_read);
	send_data(clnt_write, ct, file_name); 
	return NULL;
}

void send_data(FILE* fp, char* ct, char* file_name)
{
	char protocol[]="HTTP/1.0 200 OK\r\n";
	char server[]="Server:Linux Web Server \r\n";
	char cnt_len[]="Content-length:2048\r\n";
	char cnt_type[SMALL_BUF];
	char buf[BUFFER_SIZE];
	FILE* send_file;
	
	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
	send_file=fopen(file_name, "r");
	if (send_file==NULL)
	{
		send_error(fp);
		return;
	}

	/* «Ï¥ı ¡§∫∏ ¿¸º€ */
	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);

	/* ø‰√ª µ•¿Ã≈Õ ¿¸º€ */
	while(fgets(buf, BUFFER_SIZE, send_file)!=NULL) 
	{
		fputs(buf, fp);
		fflush(fp);
	}
	fflush(fp);
	fclose(fp);
}

const char* content_type(char* file)
{
	char extension[SMALL_BUF];
	char file_name[SMALL_BUF];
	strcpy(file_name, file);
	strtok(file_name, ".");
	strcpy(extension, strtok(NULL, "."));
	
	if (!strcmp(extension, "html")||!strcmp(extension, "htm")) 
		return "text/html";
	else
		return "text/plain";
}

void send_error(FILE* fp)
{	
	char protocol[]="HTTP/1.0 400 Bad Request\r\n";
	char server[]="Server:Linux Web Server \r\n";
	char cnt_len[]="Content-length:2048\r\n";
	char cnt_type[]="Content-type:text/html\r\n\r\n";
	char content[]="<html><head><title>NETWORK</title></head>"
	       "<body><font size=+5><br>ø¿∑˘ πﬂª˝! ø‰√ª ∆ƒ¿œ∏Ì π◊ ø‰√ª πÊΩƒ »Æ¿Œ!"
		   "</font></body></html>";

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
	fflush(fp);
}

void error_handling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
