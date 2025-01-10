all:
	cc ircb.c -o ircb -Wall -lssl -lcrypto
clean:
	rm ircb
