OBJS:=$(wildcard *.c)
server:$(OBJS)
		gcc $^ -o $@ -I ../include -lpthread -lmysqlclient -lcrypto
clean:
		rm server


