all:
	cc ircb.c -o ircb -Wall -lssl -lcrypto -g -lpthread
clean:
	rm ircb
