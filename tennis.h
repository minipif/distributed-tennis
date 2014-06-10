#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/socket.h>
//#include <sys/types.h>
#include <unistd.h>

#define TCP_PORT 3000

#define COURT_WIDTH 29
#define COURT_HEIGHT 19

#define LEFT_END 0
#define MIDDLE_PART 1
#define RIGHT_END 2

typedef unsigned char Type;

typedef struct {
	char x, y, sx, sy;
} Ball;

