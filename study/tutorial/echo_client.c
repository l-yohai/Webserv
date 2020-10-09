/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_client.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/03 05:13:33 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/03 05:21:57 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 1024

int main(int argc, char **argv)
{
	struct sockaddr_in serveraddr;
	int server_sockfd;
	int client_len;
	char buf[MAXLINE];

	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("error: ");
		return (EXIT_FAILURE);
	}

	/* 연결 요청할 서버의 주소와 포트번호 프로토콜 등을 지정한다. */
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(3500);

	client_len = sizeof(serveraddr);

	/* 서버에 연결을 시도한다. */
	if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len) == -1)
	{
		perror("connect error: ");
		return (EXIT_FAILURE);
	}

	memset(buf, 0x00, MAXLINE);
	read(0, buf, MAXLINE); /* 키보드 입력을 기다린다. */
	if (write(server_sockfd, buf, MAXLINE) <= 0) /* 입력 받은 데이터를 서버로 전송한다. */
	{
		perror("write error: ");
		return (EXIT_FAILURE);
	}
	memset(buf, 0x00, MAXLINE);
	/* 서버로부터 데이터를 읽는다. */
	if (read(server_sockfd, buf, MAXLINE) <= 0)
	{
		perror("read error: ");
		return (EXIT_FAILURE);
	}
	close(server_sockfd);
	printf("read: %s\n", buf);
	return (EXIT_SUCCESS);
}
