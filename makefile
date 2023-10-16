CC := gcc
SRC := src
OBJ := obj

# foo.o: $(SRC)/registration.c
#     $(CC) -I$(SRC) -c $< -o $@
	
# SOURCES := $(wildcard $(SRC)/*.c)
# OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# all: $(OBJECTS)
#     $(CC) $^ -o $@

# $(OBJ)/%.o: $(SRC)/%.c
#     $(CC) -I$(SRC) -c $< -o $@
server: $(SRC)/server.c login.o faculty.o student.o config.o course.o registration.o read_line.o client
	gcc $(OBJ)/login.o $(OBJ)/faculty.o $(OBJ)/student.o $(OBJ)/config.o $(OBJ)/course.o $(OBJ)/registration.o $(OBJ)/read_line.o $(SRC)/server.c -o server
	
client: $(SRC)/client.c
	gcc $(OBJ)/read_line.o $(SRC)/client.c -o client

login.o: $(SRC)/login.c read_line.o
	gcc -c $(SRC)/login.c -o $(OBJ)/login.o

faculty.o: $(SRC)/faculty.c
	gcc -c $(SRC)/faculty.c -o $(OBJ)/faculty.o

student.o: $(SRC)/student.c
	gcc -c $(SRC)/student.c -o $(OBJ)/student.o

config.o: $(SRC)/config.c
	gcc -c $(SRC)/config.c -o $(OBJ)/config.o
	
course.o: $(SRC)/course.c
	gcc -c $(SRC)/course.c -o $(OBJ)/course.o

registration.o: $(SRC)/registration.c
	gcc -c $(SRC)/registration.c -o $(OBJ)/registration.o

read_line.o: $(SRC)/read_line.c
	gcc -c $(SRC)/read_line.c -o $(OBJ)/read_line.o


clean:
	rm -rf *.o