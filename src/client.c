#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in serv_addr;
	char buffer[BUF_SIZE];

	if (argc != 3)
	{
		printf("Usage: %s <IP> <Port>\n", argv[0]);
		return 1;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("socket");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
		perror("inet_pton");
		close(sock);
		return 1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		perror("connect");
		close(sock);
		return 1;
	}

	printf("Connected to server. Type messages (type 'exit' to quit):\n");

	while (1)
	{
		printf("You: ");
		if (fgets(buffer, BUF_SIZE, stdin) == NULL)
			break;

		buffer[strcspn(buffer, "\n")] = 0;

		if (send(sock, buffer, strlen(buffer), 0) == -1)
		{
			perror("send");
			break;
		}

		if (strcmp(buffer, "exit") == 0)
			break;

		memset(buffer, 0, BUF_SIZE);
		ssize_t recv_len = recv(sock, buffer, BUF_SIZE - 1, 0);
		if (recv_len <= 0)
		{
			perror("recv");
			break;
		}

		buffer[recv_len] = 0;
		printf("Server: %s\n", buffer);
	}

	printf("Connection closed.\n");
	close(sock);
	return 0;
}
