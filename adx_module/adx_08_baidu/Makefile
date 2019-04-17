
CC = g++
CFLAGS = -Wall -g
LIBS+= -lm
# LIBS+= -ljemalloc
LIBS+= -lprotobuf
LIBS+= -lcurl
APP_NAME=app

SRCPATH=src/
OBJPATH=src/

OBJECTS+=$(OBJPATH)adx_curl.o
OBJECTS+=$(OBJPATH)baidu.pb.o
OBJECTS+=$(OBJPATH)main.o

all : $(OBJECTS)
	$(CC) -o $(APP_NAME) $(OBJECTS) $(LIBS) $(CFLAGS)

protobuf:
	protoc baidu.proto --cpp_out=src/

$(OBJPATH)%.o : $(SRCPATH)%.c
	gcc $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cpp
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cc
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean :
	@rm -f $(OBJECTS)
	@rm -f $(APP_NAME)

	

