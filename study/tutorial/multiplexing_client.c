/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplexing_client.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/05 05:27:20 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/05 05:48:43 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int sock;
	char msg[30];
	int len, recv_len, recv_num;

	struct sockaddr_in server_address;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <PORT>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(argv[1]);
	server_address.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		error_handling("connect() error");

	while (1)
	{
		fputs("전송할 메세지를 입력하세요 (q or quit) : ", stdout);
		fgets(msg, sizeof(msg), stdin);

		if (!strcmp(msg, "q\n")) break ;
		len = write(sock, msg, strlen(msg));

		for (recv_len = 0; recv_len < len; )
		{
			recv_num = read(sock, &msg[recv_len], len - recv_len);
			if (recv_num == -1)
				error_handling("read() error");
			recv_len += recv_num;
		}
		msg[len] = 0;
		printf("서버로부터 전송된 메세지: %s \n", msg);
	}
	close(sock);
	return (EXIT_SUCCESS);
}