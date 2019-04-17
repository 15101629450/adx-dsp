
CC = g++
CFLAGS = -Wall -g
LIBS= -pthread -ljemalloc -lhiredis
SRCPATH=src/
OBJPATH=obj/
APP_NAME=auto_cost_server

OBJECTS+=$(OBJPATH)adx_list.o
OBJECTS+=$(OBJPATH)adx_rbtree.o
OBJECTS+=$(OBJPATH)adx_string.o
OBJECTS+=$(OBJPATH)adx_alloc.o
OBJECTS+=$(OBJPATH)adx_queue.o
OBJECTS+=$(OBJPATH)adx_cache.o

OBJECTS+=$(OBJPATH)adx_conf_file.o
OBJECTS+=$(OBJPATH)adx_time.o
OBJECTS+=$(OBJPATH)adx_log.o

OBJECTS+=$(OBJPATH)adx_util.o
OBJECTS+=$(OBJPATH)adx_redis.o
OBJECTS+=$(OBJPATH)adx_json.o
OBJECTS+=$(OBJPATH)json.o

OBJECTS+=$(OBJPATH)adx_auto_cost.o
OBJECTS+=$(OBJPATH)main.o

all : $(OBJECTS)
	$(CC) -o $(APP_NAME) $(OBJECTS) $(LIBS) $(CFLAGS)

install:
	rm -f /usr/bin/$(APP_NAME)
	cp $(APP_NAME) /usr/bin/$(APP_NAME)

$(OBJPATH)%.o : $(SRCPATH)%.c
	gcc $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cpp
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)
clean :
	@rm -f $(OBJECTS)
	@rm -f $(APP_NAME)
	@rm -f *.pyc

	

