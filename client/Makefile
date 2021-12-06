OBJS:=$(wildcard *.c)
client:$(OBJS)
		gcc $^ -o $@ -I ../include -lpthread -lmysqlclient -lcrypto -lcrypt
clean:
		rm client


