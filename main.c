/*

  b0t Main,
  IRC Functions.

  Author: stackiller

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "prototypes.h"

#include "modules/mod.c"
#include "modules/tricks.c"

#include "irc_funcs.c"
#include "ssl_funcs.c"

/* main - create SSL context and connect */
int
main(int n_args, char *s_args[]) {
  
  // check arguments number.
  if(n_args != 6)
  {
    printf("usage: %s <server_addr> <port> <nick> <password> \"#channel1,#channel2,#channel3,#channel4,#channel5\"\n", s_args[0]);
    exit(0);
  }

  b_Header(); // bot header

  // assign data to fields.
  irc.host = s_args[1];
  irc.port = s_args[2];
  irc.nick = s_args[3];
  irc.pass = s_args[4];
  irc.chans = s_args[5];
  irc.ctx = init_Ctx(); // init the SSL context.
  irc.ssl = SSL_new(irc.ctx); // create new SSL context.
  irc.sockfd = new_Conn(irc.host, atoi(irc.port)); // initialize SSL connection.

  b_Nick(irc.nick); // identify user
  b_Creds(irc.nick, irc.pass); // set credentials
  b_Join(irc.chans); // join to channel

  // receive messages
  do {
    irc.buffer = r_Buffer(); // store the read message
    fprintf(stdout, "%s", irc.buffer); // uncomment this if you want see the buffer output
    free(irc.buffer); // release buffer
  } while(1);

  SSL_free(irc.ssl); // release connection state
  close(irc.sockfd); // close sockfd
  SSL_CTX_free(irc.ctx); // release context
  return(0);
}