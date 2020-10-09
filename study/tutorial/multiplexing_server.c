/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplexing_server.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/05 05:41:12 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/05 05:51:42 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 100

void error_hadling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int server_socket;
	struct sockaddr_in server_address;

	fd_set reads, tmp;
	int fd_max;

	char msg[BUFFER_SIZE];
	int len;
	struct timeval timeout;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(atoi(argv[1]));

	if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)))
		error_hadling("bind() error");
	if (listen(server_socket, 5) == -1)
		error_hadling("listen() error");

	FD_ZERO(&reads);
	FD_SET(server_socket, &reads);
	fd_max = server_socket;

	while (1)
	{
		int fd, len;
		int client_socket, client_len;
		struct sockaddr_in client_address;

		tmp = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if (select(fd_max + 1, &tmp, 0, 0, &timeout) == -1)
			error_hadling("select() error");

		for (fd = 0; fd < fd_max + 1; fd++)
		{
			if (FD_ISSET(fd, &tmp))
			{
				if (fd == server_socket)
				{
					client_len = sizeof(client_address);
					client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
					FD_SET(client_socket, &reads);
					if (fd_max < client_socket)
						fd_max = client_socket;
					printf("Client Connect: FD = %d\n", client_socket);
				}
				else
				{
					len = read(fd, msg, BUFFER_SIZE);
					if (len == 0)
					{
						FD_CLR(fd, &reads);
						close(fd);
						printf("Client Disconnect: FD = %d\n", fd);
					}
					else
					{
						write(fd, msg, len);
					}
				}
			}
		}
	}
	return (EXIT_SUCCESS);
}