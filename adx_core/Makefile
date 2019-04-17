
CC = g++
CFLAGS = -Wall -g
LIBS= -lfcgi -pthread -L/usr/lib64/mysql/ -lmysqlclient
SRCPATH=src/
OBJPATH=obj/
APP_NAME=odin_pv

OBJECTS+=$(OBJPATH)adx_log.o
OBJECTS+=$(OBJPATH)adx_list.o
OBJECTS+=$(OBJPATH)adx_queue.o
OBJECTS+=$(OBJPATH)adx_string.o
OBJECTS+=$(OBJPATH)adx_alloc.o
OBJECTS+=$(OBJPATH)adx_uri.o
OBJECTS+=$(OBJPATH)adx_region.o
OBJECTS+=$(OBJPATH)adx_mysql.o
OBJECTS+=$(OBJPATH)adx_conf_file.o

OBJECTS+=$(OBJPATH)main.o
OBJECTS+=$(OBJPATH)pv.o

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

	

