/*

  ircb Main,
  xD

*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#include <pthread.h>

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
  signal(SIGPIPE, SIG_IGN);

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
  irc.pong = 0;
  irc.reconnect = 0;

  // header art.
  bot_Header();

  // init connection state.
  init_conn(&irc);

  // receive messages and check connection threads.
  pthread_t recv_thread;
  pthread_t check_con;

  threads_create:
  pthread_create(&recv_thread, NULL, &thread_recv, NULL);
  pthread_create(&check_con, NULL, &check_Timeout, NULL);
  goto recv_msg;

  reconnect:
  pthread_cancel(recv_thread);
  pthread_cancel(check_con);
  pthread_join(check_con, NULL);
  recon_conn(&irc);
  goto threads_create;

  recv_msg:
  do {
    if(irc.reconnect) {
      goto reconnect;
    } sleep(1);
  } while(true);

  // end connection state.
  end_conn(&irc);

  return(0);
}

/* Thread_recv cleanup function. */
void
thread_recv_cleanup(void *unsed)
{
  puts("Inside thread_recv_cleanup");
  if(irc.buffer_matrix_size > 0) {
    matrix_Destroy(irc.buffer_matrix, irc.buffer_matrix_size);
  }
}

/* Message receiving thread. */
void*
thread_recv(void *none)
{
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  pthread_cleanup_push(thread_recv_cleanup, NULL);
  pthread_testcancel();
  do
  {
    irc.buffer_matrix = r_Buffer(&irc.buffer_matrix_size);

    if(irc.buffer_matrix_size > 0) {
      read_matrix_Buffer(irc.buffer_matrix, irc.buffer_matrix_size);
      matrix_Destroy(irc.buffer_matrix, irc.buffer_matrix_size);
      irc.buffer_matrix_size = 0;
    }
  } while(true);
  pthread_cleanup_pop(0);
}