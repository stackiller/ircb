all:
	cc ircb.c -o ircb -Wall -lssl -lcrypto -g
clean:
	rm ircb
