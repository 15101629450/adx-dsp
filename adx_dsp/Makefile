
CC = g++
CFLAGS = -Wall -g
LIBS+= -pthread
LIBS+= -lfcgi
# LIBS+= -ljemalloc
# LIBS+= -lhiredis
# LIBS+= -lrdkafka
# LIBS+= -lprotobuf
LIBS+= -lm

SRCPATH=src/
OBJPATH=obj/
APP_NAME=adx_dsp

OBJECTS+=$(OBJPATH)adx_list.o
OBJECTS+=$(OBJPATH)adx_rbtree.o
OBJECTS+=$(OBJPATH)adx_alloc.o
OBJECTS+=$(OBJPATH)adx_queue.o
OBJECTS+=$(OBJPATH)adx_string.o
OBJECTS+=$(OBJPATH)adx_conf_file.o
OBJECTS+=$(OBJPATH)adx_cache.o
OBJECTS+=$(OBJPATH)adx_log.o

OBJECTS+=$(OBJPATH)adx_json.o
OBJECTS+=$(OBJPATH)adx_redis.o
OBJECTS+=$(OBJPATH)adx_kafka.o
OBJECTS+=$(OBJPATH)adx_region.o

OBJECTS+=$(OBJPATH)adx_util.o
OBJECTS+=$(OBJPATH)adx_dump.o
OBJECTS+=$(OBJPATH)adx_element.o
OBJECTS+=$(OBJPATH)adx_request.o
OBJECTS+=$(OBJPATH)adx_select.o
OBJECTS+=$(OBJPATH)adx_response.o
OBJECTS+=$(OBJPATH)adx_com.pb.o
OBJECTS+=$(OBJPATH)main.o

OBJECTS+=$(OBJPATH)adx_template.o

# all : clean $(OBJECTS)
all : $(OBJECTS)
	$(CC) -o $(APP_NAME) $(OBJECTS) $(LIBS) $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.c
	gcc $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cpp
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

$(OBJPATH)%.o : $(SRCPATH)%.cc
	$(CC) $(INCLUDE) -c $< -o $@ $(CFLAGS)

install:
	@./sh/install.sh

clean :
	@rm -f $(OBJECTS)
	@rm -f $(APP_NAME)
xxx :
	echo "xxxx"
	

