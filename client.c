/*
Jonathan Lepage
November 2010
*/
#include "client.h"
#include "tennis.h"

int sock;
Type type;

int connect_to_server(char* server_name) {
	socklen_t addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in server_addr;
    
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	
	memset(&server_addr, 0, addrlen);
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
	if ((server_addr.sin_addr.s_addr = inet_addr(server_name)) == INADDR_NONE) return -1;
	
	if (connect(sock, (struct sockaddr*)(&server_addr), addrlen) < 0) {
		perror("connect");
		return -1;
	}
	
	return 0;
}

int get_client_type(void) {
	if (recv(sock, &type, sizeof(Type), 0) != sizeof(Type)) {
		perror("recv");
		return -1;
	}
	return 0;
}

void draw_tennis_court(void) {
	int x, y;
	printf("\e[2J\e[1;1H"); // clear screen
	printf("\e[1;1H%s", type==LEFT_END?"\e[44m//\e[0m":" \e[44m/\e[0m");
	printf("\e[%d;1H%s", COURT_HEIGHT+2, type==LEFT_END?"\e[44m\\\\\e[0m":" \e[44m\\\e[0m");
	for (x=2; x<COURT_WIDTH+3; x++) {
		printf("\e[1;%dH\e[44m-\e[0m", x+1);
		printf("\e[%d;%dH\e[44m-\e[0m", COURT_HEIGHT+2, x+1);
	}
	printf("\e[1;%dH%s", COURT_WIDTH+4, type==RIGHT_END?"\e[44m\\\\\e[0m":"\e[44m\\\e[0m ");
	printf("\e[%d;%dH%s", COURT_HEIGHT+2, COURT_WIDTH+4, type==RIGHT_END?"\e[44m//\e[0m":"\e[44m/\e[0m ");
	for (y=1; y<COURT_HEIGHT+1; y++) {
		printf("\e[%d;1H%s", y+1, type==LEFT_END?"\e[44m||\e[0m":" \e[42m.\e[0m");
		printf("\e[%d;%dH%s", y+1, COURT_WIDTH+4, type==RIGHT_END?"\e[44m||\e[0m":"\e[42m.\e[0m ");
	}
	printf("\e[%d;1H", COURT_HEIGHT+3); // puts back cursor on the bottom
	fflush(stdout);
}

void move_ball_around(Ball* ball) {
	while (1) {
		// draw ball
		printf("\e[%d;%dH\e[31;1;7m[]\e[0m\e[%d;1H", ball->y+2, ball->x+3, COURT_HEIGHT+3);
		fflush(stdout);
		usleep(50000);
		printf("\e[%d;%dH  \e[%d;1H", ball->y+2, ball->x+3, COURT_HEIGHT+3); // erase ball
		fflush(stdout);
		// move ball
		ball->x += ball->sx;
		ball->y += ball->sy;
		// check ball position
		if (ball->y < 0) { // reached the top
			ball->y = 1;
			ball->sy = -(ball->sy);
		} else if (ball->y == COURT_HEIGHT) { // reached the bottom
			ball->y = COURT_HEIGHT-2;
			ball->sy = -(ball->sy);
		}
		if (ball->x < 0) { // reached the left side
			if (type == LEFT_END) { // bounce
				ball->x = 1;
				ball->sx = -(ball->sx);
			} else { // out of the range;
				ball->x = COURT_WIDTH-1;
				//printf("ball passed to the left\n");
				return;
			}
		}
		if (ball->x == COURT_WIDTH) { // reached the right side
			if (type == RIGHT_END) { // bounce
				ball->x = COURT_WIDTH-2;
				ball->sx = -(ball->sx);
			} else { // out of the range;
				ball->x = 0;
				//printf("ball passed to the right\n");
				return;
			}
		}
	}
}

int tennis_client(char* server_name) {
	Ball ball;
	if (connect_to_server(server_name) < 0) {
		return -1;
	}
	printf("waiting for tennis to begin...\n");
	if (get_client_type() < 0) return -1;
	draw_tennis_court();
	
	while (1) {
		// receiving ball from server
		if (recv(sock, &ball, sizeof(Ball), 0) != sizeof(Ball)) {
			printf("\e[2J\e[1;1Hgame ended\n");
			return 0;
		}
		move_ball_around(&ball);
		// send the ball to the server
		if (send(sock, &ball, sizeof(Ball), 0) != sizeof(Ball)) {
			perror("send");
			return -1;
		}
	}
	return 0;
}

int main(int argc, const char* argv[]) {
	char* server_name;
	if (argc > 1) {
		server_name = (char*)(argv[1]);
	} else {
		server_name = DEFAULT_SERVER;
	}
	if (tennis_client(server_name) < 0) {
		printf("oops, something went wrong!\n");
		return -1;
	}
	return 0;
}

