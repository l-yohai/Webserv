/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPServer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/05 11:24:17 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/05 16:13:52 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define LINE_SIZE 100

void error_hadling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

void send_error(FILE *fp)
{
	char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
	char server[] = "Server: Best Http Server \r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[] = "Content-type:text/html\r\n\r\n";
	char content[] = "<html><head><title>NETWORK</title></head>"
						"<body><font size=+5><br>오류발생! 요청파일명 및 요청방식 확인!"
						"</font></body></html>";
	
	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
	fflush(fp);
}

char *strtrim(char *str)
{
	int end = strlen(str);
	while (str[end] <= ' ' && end > 0)
		end--;
	str[end + 1] = 0;
	while (*str <= ' ' && *str != 0)
		str++;
	return str;
}

char *content_type(char *f)
{
	char extension[LINE_SIZE];
	char filename[LINE_SIZE];
	strcpy(filename, f);
	strtok(filename, ".");
	strcpy(extension, strtok(NULL, "."));

	if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
		return "text/html";
	if (!strcmp(extension, "txt") || !strcmp(extension, "c"))
		return "text/plain";

	return "text/plain";
}

void send_data(FILE *fp, char *ct, char *filename)
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char server[] = "Server: Get Pork Belly \r\n";
	char cnt_len[] = "Content-length: 2048\r\n";
	char cnt_type[LINE_SIZE];
	char buf[BUFFER_SIZE];
	FILE *send_file;
	int len;

	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);

	send_file = fopen(filename, "r");
	if (send_file == NULL)
	{
		send_error(fp);
		return ;
	}

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);

	while ((len = strlen(fgets(buf, BUFFER_SIZE, send_file))) != 0)
	{
		fputs(buf, fp);
		fflush(fp);
	}
	fflush(fp);
	fclose(fp);
}

void *client_connection(void *arg)
{
	int client_socket = *((int *)arg);
	char require_line[LINE_SIZE];
	FILE *client_read;
	FILE *client_write;

	char method[10];
	char ct[15];
	char filename[30];

	client_read = fdopen(client_socket, "r");
	client_write = fdopen(dup(client_socket), "w");

	fgets(require_line, LINE_SIZE, client_read);
	fputs(require_line, stdout);

	if (strstr(require_line, "HTTP/") == NULL)
	{
		send_error(client_write);
		fclose(client_read);
		fclose(client_write);
		return NULL;
	}
	strcpy(method, strtok(require_line, " /"));
	strcpy(filename, strtok(NULL, " /"));
	strcpy(ct, content_type(filename));

	if (strcmp(method, "GET") != 0)
	{
		send_error(client_write);
		fclose(client_read);
		fclose(client_write);
		return NULL;
	}
	
	while (1)
	{
		fgets(require_line, LINE_SIZE, client_read);
		fputs(require_line, stdout);
		if (strcmp(strtrim(require_line), "") == 0)
			break ;
	}
	fclose(client_read);
	send_data(client_write, ct, filename);

	return NULL;
}

int main(int argc, char **argv)
{
	int server_socket;
	int client_socket;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int client_address_size;
	char buf[BUFFER_SIZE];
	pthread_t thread;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
		error_hadling("socket() error");

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		error_hadling("bind() error");
	if (listen(server_socket, 20) == -1)
		error_hadling("listen() error");

	while (1)
	{
		client_address_size = sizeof(client_address);
		client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);

		printf("연결요청: %s %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
		pthread_create(&thread, NULL, client_connection, &client_socket);
	}
	return (EXIT_SUCCESS);
}

