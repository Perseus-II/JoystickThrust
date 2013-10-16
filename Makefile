TMS = joystick_thrust.cpp
TMN = joystick_thrust 
CC = g++
CFLAGS = `sdl-config --cflags`
LDFLAGS = `sdl-config --libs`
RM = rm -f

all: 
	$(CC) $(TMS) -o $(TMN) $(CFLAGS) $(LDFLAGS) 

clean: clean-swap
	$(RM) $(TMN)
	$(RM) *.o	

clean-swap:
	$(RM) .*.swp
	$(RM) .*.swo
	
re: clean all
