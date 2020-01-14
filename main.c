/*

  b0t Main,
  IRC Functions.

  Author: stackiller

*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
  
  // check arguments number
  if(n_args < 6)
    usage(s_args[0]);

  b_Header(); // bot header

  // assign data to fields
  irc.host = s_args[1];
  irc.port = s_args[2];
  irc.nick = s_args[3];
  irc.pass = s_args[4];
  irc.chans = s_args[5];
  irc.isSSL = true; // default to use SSL

  if(n_args == 7) {
    if(str_Cmp("--nossl", s_args[6]))
      irc.isSSL = false;
    else
      usage(s_args[0]);
  }

  irc.ctx = irc.isSSL ? init_Ctx() : NULL;
  irc.ssl = irc.isSSL ? SSL_new(irc.ctx) : NULL;
  irc.sockfd = new_Conn(irc.host, atoi(irc.port));

  b_Nick(irc.nick); // identify user
  b_Creds(irc.nick, irc.pass); // set credentials
  b_Join(irc.chans); // join to channel

  // receive messages
  do {
    irc.buffer = r_Buffer(); // read buffer
    fprintf(stdout, "%s", irc.buffer); // uncomment this if you want see the buffer output
    free(irc.buffer); // release buffer
  } while(1);

  // release connection state
  if(irc.isSSL) {
    SSL_free(irc.ssl);
    SSL_CTX_free(irc.ctx);
  }
  close(irc.sockfd);
  
  return(0);
}
