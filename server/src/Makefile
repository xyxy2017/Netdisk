OBJS:=$(wildcard *.c)
server:$(OBJS)
		gcc $^ -o $@ -I../include -lpthread -lmysqlclient
clean:
		rm server


