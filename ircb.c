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

#include "modules/core.c" // core bot functions.
#include "modules/sys.c" // bot system functions.

#include "core_funcs.c"
#include "irc_funcs.c"
#include "ssl_funcs.c"

/* Main :D */
int
main(int n_args, char *s_args[])
{
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

  // receive the messages.
  do {
    irc.buffer_matrix = r_Buffer(&irc.buffer_matrix_size);

    if(irc.buffer_matrix_size > 0) {
      read_matrix_Buffer(irc.buffer_matrix, irc.buffer_matrix_size);
      matrix_Destroy(irc.buffer_matrix, irc.buffer_matrix_size);
    }
  } while(1);

  // end connection state.
  end_conn(&irc);

  return(0);
}