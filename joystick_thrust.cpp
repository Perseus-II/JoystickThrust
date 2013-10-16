#include "SDL/SDL.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>


#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif 

#ifndef MAX
#define MAX 64
#endif 

SDL_Joystick *stick = NULL;
SDL_Event event;

int main(int argc, char **argv) {
		int surge_port;
		int surge_starboard;
		int heave_a;
		int heave_b;

		int sockfd;
		int port;
		int n;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		char buf[MAX];

		port = 7090;

		// create a new socket
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				printf("Failed to create socket\n");
				exit(1);
		}

		server = gethostbyname("192.168.10.90");

		memset(&serv_addr, 0, sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);
		memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

		// try to connect to the server	
		if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
				// connection failed
				printf("Failed to connect to server\n");
				exit(1);
		}

	
		printf("Connected to server on %d\n", sockfd);

		/* Initialize SDL */
		if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
				puts("Failed to initialize joystick");
				return FALSE;
		}

		SDL_JoystickEventState(SDL_ENABLE);

		if(SDL_NumJoysticks() < 1) {
				puts("No joystick connected");
				return FALSE;
		}

		/* Attach the joystick */
		stick = SDL_JoystickOpen(0);

		if(stick == NULL) {
				puts("Error attaching joystick!");
				return FALSE;
		}	

		printf("Joystick is attached!");

		/* get the number of axes */
		printf("This joystick has %d axes\n", SDL_JoystickNumAxes(stick));	

		int x1_move;
		int y1_move;
		int x2_move;
		int y2_move;
		int heave_up;
		int heave_down;

		signal (SIGPIPE, SIG_IGN);

		while(1) {
				while(SDL_PollEvent(&event)) {
						x1_move = SDL_JoystickGetAxis(stick, 0);				
						y1_move = SDL_JoystickGetAxis(stick, 1);				
						x2_move = SDL_JoystickGetAxis(stick, 2);				
						y2_move = SDL_JoystickGetAxis(stick, 3);				
						heave_up = SDL_JoystickGetButton(stick, 4) * 40;
						heave_down = SDL_JoystickGetButton(stick, 5) * 40 * -1;
					
						surge_starboard = -1 * (y2_move * (100.0/32768.0));
						surge_port = -1 * (y1_move * (100.0/32768.0));
						heave_a = heave_up + heave_down;
						heave_b = heave_up + heave_down;

						sprintf(buf, "/set_thrust %d,%d,%d,%d", surge_port, surge_starboard, heave_a, heave_b); 

						write(sockfd, buf, strlen(buf)+1); 
						usleep(100000);
						//printf("(%d, %d) | (%d, %d)\n", x1_move, y1_move, x2_move, y2_move); 
						printf("Surge Port = %d, Surge Starboard = %d, Heave A = %d, Heave B = %d\n", surge_port, surge_starboard, heave_a, heave_b);

							

						switch(event.type) {
								case SDL_KEYDOWN:
										break;

								case SDL_QUIT:
										break;
						}
				}
		}
}
