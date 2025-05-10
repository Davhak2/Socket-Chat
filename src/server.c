#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void process_string(char *str)
{
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == 'z' || str[i] == 'Z')
			str[i] -= 25;
		else if ((str[i] >= 'a' && str[i] < 'z') || (str[i] >= 'A' && str[i] < 'Z'))
			str[i]++;
	}
}

int main(int argc, char **argv)
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;
	char buffer[BUF_SIZE];

	if (argc != 2)
	{
		printf("Usage: %s <Port>\n", argv[0]);
		return 1;
	}
	else
		printf("Waiting for connection...\n");

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
	{
		perror("socket");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("bind");
		close(serv_sock);
		return 1;
	}

	if (listen(serv_sock, 5) == -1)
	{
		perror("listen");
		close(serv_sock);
		return 1;
	}

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
	if (clnt_sock == -1)
	{
		perror("accept");
		close(serv_sock);
		return 1;
	}

	printf("Client connected. Chat started...\n");

	while (1)
	{
		memset(buffer, 0, BUF_SIZE);
		ssize_t recv_len = recv(clnt_sock, buffer, BUF_SIZE - 1, 0);
		if (recv_len <= 0)
			break;

		buffer[recv_len] = 0;

		printf("Client: %s\n", buffer);

		if (strcmp(buffer, "exit") == 0)
		{
			printf("Client exited. Closing server.\n");
			break;
		}

		process_string(buffer);
		if (send(clnt_sock, buffer, strlen(buffer), 0) == -1)
		{
			perror("send");
			break;
		}
	}

	close(clnt_sock);
	close(serv_sock);
	return 0;
}
