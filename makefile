
CC=g++ -std=c++11 -Wall -W

LIB=-g -L/usr/local/lib -lGL -lGLU -lglut
INCLUDE=-g -I/usr/local/include

all: GL_PSO

GL_PSO: main.o food.o world.o particles.o
	$(CC) -pthread  main.o food.o world.o particles.o -o GL_PSO $(LIB)

main.o: main.cpp
	$(CC) $(INCLUDE) -c main.cpp -o main.o

food.o: food.cpp
	$(CC) $(INCLUDE) -c food.cpp -o food.o

world.o: world.cpp
	$(CC) $(INCLUDE) -c world.cpp -o world.o

particles.o: particles.cpp
	$(CC) $(INCLUDE) -c particles.cpp -o particles.o

clean:
	rm *.o
	rm GL_PSO
