
CC = g++
CFLAGS = -Wall -g
# LIBS+= -ljemalloc
# LIBS+= -lprotobuf
LIBS+= -lcurl
LIBS+= -lm
APP_NAME=app

SRCPATH=src/
OBJPATH=src/

OBJECTS+=$(OBJPATH)adx_list.o
OBJECTS+=$(OBJPATH)adx_rbtree.o
OBJECTS+=$(OBJPATH)adx_alloc.o
OBJECTS+=$(OBJPATH)adx_json.o
OBJECTS+=$(OBJPATH)adx_cache.o
OBJECTS+=$(OBJPATH)main.o

all : clean $(OBJECTS)
	$(CC) -o $(APP_NAME) $(OBJECTS) $(LIBS) $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.c
	gcc $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cpp
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cc
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

clean :
	@rm -f $(OBJECTS)
	@rm -f $(APP_NAME)


