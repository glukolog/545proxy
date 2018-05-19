CC = gcc
CFLAGS = -Wall -std=gnu99

# DEFS = -D PTHREAD_BARRIER_SERIAL_THREAD
CFLAGS += $(DEFS)

TARGET = afree-proxy

SRCS = main.c conf.c miner.c pool.c proxy.c sha2.c stratum.c util.c

INC = -I /usr/local/include

LIBVAR = -luv -ljansson
LIBPATH = -L/usr/local/lib/

DPATH = -Wl,-rpath=.

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LIBPATH) $(LIBVAR) $(DPATH)

clean:
	rm -rf $(TARGET) $(OBJS)

exec:clean $(TARGET)
	@echo start exec
	./$(TARGET)
	@echo end exec

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
