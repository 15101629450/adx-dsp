
CC = g++
CFLAGS = -Wall -g
LIBS+= -lm
LIBS+= -ljemalloc
LIBS+= -lprotobuf
APP_NAME=app

SRCPATH=src/
OBJPATH=src/

OBJECTS+=$(OBJPATH)adx_list.o
OBJECTS+=$(OBJPATH)adx_alloc.o
OBJECTS+=$(OBJPATH)adx_json.o
OBJECTS+=$(OBJPATH)pack.pb.o
OBJECTS+=$(OBJPATH)main.o

all : clean protobuf $(OBJECTS)
	$(CC) -o $(APP_NAME) $(OBJECTS) $(LIBS) $(CFLAGS)

protobuf:
	protoc pack.proto --cpp_out=src/

$(OBJPATH)%.o : $(SRCPATH)%.c
	gcc $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cpp
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cc
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean :
	@rm -f $(OBJECTS)
	@rm -f $(APP_NAME)

	

