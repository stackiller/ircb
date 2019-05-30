/*

  b0t Main,
  IRC Functions.

  Author: aSynKronus

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "prototypes.h"

#include "modules/moderate.c"
#include "modules/tricks.c"

#include "irc.c"
#include "ssl.c"

/* main - create SSL context and connect */
int main(int count, char *args[])
{
  // check arguments number
  if(count != 6)
  {
    printf("usage: %s <server_addr> <port> <nick> <password> \"#channel1,#channel2,#channel3,#channel4,#channel5\"\n", args[0]);
    exit(0);
  }

  // assign data to fields
  irc.host = args[1];
  irc.port = args[2];
  irc.nick = args[3];
  irc.pass = args[4];
  irc.chans = args[5]; // copy into irc.chans
  irc.ctx = initCtx(); // initialize SSL context
  irc.ssl = SSL_new(irc.ctx); // create new SSL connection state
  irc.sockfd = newCon(irc.host, atoi(irc.port)); // create connection socket

  // attach socket descriptor
  SSL_set_fd(irc.ssl, irc.sockfd);
  if(SSL_connect(irc.ssl) == FAIL)
    ERR_print_errors_fp(stderr);
  else
  // show SSL chiper - certificates
  printf(
    "[%s%s*%s] Connected with %s encryption\n",
    fgGreen, blink, resetCl, SSL_get_cipher(irc.ssl)
  ); showCerts(irc.ssl);

  setNick(irc.nick); // identify user
  setCreds(irc.pass); // set credentials

  // receive messages
  do
  {
    irc.buffer = NULL; // point pointer to null
    irc.buffer = readMsg(irc.buffer); // store the read message
    printf("%s", irc.buffer); // print message
    free(irc.buffer); // release buffer
  }
  while(1);

  SSL_free(irc.ssl); // release connection state
  close(irc.sockfd); // close sockfd
  SSL_CTX_free(irc.ctx); // release context
  return(0);
}
