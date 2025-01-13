/*

  ircb Main,
  xD

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

#include "modules/core.c" // core funcs of bot.
#include "modules/sys.c" // system functions.

#include "all_funcs.c"
#include "irc_funcs.c"
#include "ssl_funcs.c"

/* Main xP */
int
main(int n_args, char *s_args[]) {
  
  // check the number of arguments.
  if(n_args < 6) {
    usage(s_args[0]);
    exit(0);
  }

  // assign the respective data to the fields.
  irc.host = s_args[1];
  irc.port = s_args[2];
  irc.nick = s_args[3];
  irc.pass = s_args[4];
  irc.chans = s_args[5];

  // header art.
  bot_Header();

  // init connection state.
  init_conn(&irc);

  bot_Nick(irc.nick); // identifies the user.
  bot_Creds(irc.nick, irc.pass); // sets credentials.

  // receive the messages
  do {
    irc.buffer = r_Buffer(); // reads the buffer.

    char *endOf = strstr(irc.buffer, ":End of message of the day.");

    if(endOf != NULL) {
      bot_Join(irc.chans); // join to channels.
    }

    fprintf(stdout, "%s", irc.buffer); // show the received messages.
    release(irc.buffer);
  } while(1);

  // end connection state.
  end_conn(&irc);

  return(0);
}
