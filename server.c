/*
Jonathan Lepage
November 2010
*/
#include "server.h"
#include "tennis.h"

int wait_clients_connect(int client_sock[], int nb_clients) {
	int server_sock;
	int client;

	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr, client_addr;

	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	
	memset(&server_addr, 0, addrlen);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

#if 1
	const int value = 1;
	if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)
) < 0) {
		return -1;
	}
#endif
        
	if (bind(server_sock, (struct sockaddr*) &server_addr, addrlen) < 0) {
		perror("bind");
		return -1;
	}
	
	if (listen(server_sock, LISTEN_BACKLOG) < 0) {
		perror("listen");
		return -1;
	}
	
	printf("waiting for %d clients\n", nb_clients);
	
	for (client=0; client<nb_clients; client++) {
		printf("waiting for client %d...\n", client);
		if ((client_sock[client] = accept(server_sock, (struct sockaddr*) &client_addr, &addrlen)) < 0) {
			perror("accept");
			return -1;
		}
	}
	
	if (close(server_sock) < 0) return -1;
	
	return 0;
}

int set_clients_type(int client_sock[], int nb_clients) {
	int client;
	Type type;
	for (client=0; client<nb_clients; client++) {
		if (client == 0) {
			type = LEFT_END;
		} else if (client == nb_clients - 1) {
			type = RIGHT_END;
		} else {
			type = MIDDLE_PART;
		}
		if (send(client_sock[client], &type, sizeof(Type), 0) != sizeof(Type)) return -1;
	}
	return 0;
}

int play_tennis(int client_sock[], int nb_clients) {
	Ball ball;
	int curr_client;
	
	// random starting position
	srand(time(NULL));
	ball.x = rand() % COURT_WIDTH;
	ball.y = rand() % COURT_HEIGHT;
	ball.sx = (rand()&1)?1:-1;
	ball.sy = (rand()&1)?1:-1;
	curr_client = rand() % nb_clients;
	
	while (1) {
		printf("ball position: [%2d %2d]  speed: [%d %d]  passed to client %d\n", ball.x, ball.y, ball.sx, ball.sy, curr_client);
		if (send(client_sock[curr_client], &ball, sizeof(Ball), 0) != sizeof(Ball)) {
			perror("send");
			return -1;
		}
		if (recv(client_sock[curr_client], &ball, sizeof(Ball), 0) != sizeof(Ball)) {
			printf("game ended (client %d quit)\n", curr_client);
			return 0;
		}
		if (ball.sx > 0) { // ball going to the right
			curr_client++;
		} else { // ball going to the left
			curr_client--;
		}
	}
	return 0;
}

int tennis_server(int nb_clients) {
	int client_sock[nb_clients];
	if (wait_clients_connect(client_sock, nb_clients) < 0) return -1;
	if (set_clients_type(client_sock, nb_clients) < 0) return -1;
	printf("tennis starting\n");
	if (play_tennis(client_sock, nb_clients) < 0) return -1;
	
	return 0;
}

int main(int argc, const char* argv[]) {
	int nb_clients;
	if (argc > 1) {
		nb_clients = atoi(argv[1]);
		if (nb_clients < 2) {
			nb_clients = DEFAULT_NB_CLIENTS;
		}
	} else {
		nb_clients = DEFAULT_NB_CLIENTS;
	}
	if (tennis_server(nb_clients) < 0) {
		printf("oops, something went wrong!\n");
		return -1;
	}
	return 0;
}

