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

#include "modules/manage.c" // gerenciamento
#include "modules/moderate.c" // moderação do canal

#include "all_funcs.c"
#include "irc_funcs.c"
#include "ssl_funcs.c"

/* main - sem muito o que dizer xP */
int
main(int n_args, char *s_args[]) {
  
  // checa o número de argumentos.
  if(n_args < 6) {
    usage(s_args[0]);
    exit(0);
  }

  // Atribui os respectivos dados aos campos.
  irc.host = s_args[1];
  irc.port = s_args[2];
  irc.nick = s_args[3];
  irc.pass = s_args[4];
  irc.chans = s_args[5];

  // Arte do cabeçalho.
  bot_Header();

  irc.ctx = init_Ctx();
  irc.ssl = SSL_new(irc.ctx);
  irc.sockfd = new_Conn(irc.host, atoi(irc.port));

  bot_Nick(irc.nick); // identifica o usuário.
  bot_Creds(irc.nick, irc.pass); // define as credenciais.

  // recebe as mensagens
  do {
    irc.buffer = r_Buffer(); // lê o buffer.

    char *endOf = strstr(irc.buffer, ":End of message of the day.");

    if(endOf != NULL) {
      // bot_Nick(irc.nick); // identifica o usuário.
      bot_Creds(irc.nick, irc.pass); // define as credenciais.
      bot_Join(irc.chans); // entra para o canal.
    }

    fprintf(stdout, "%s", irc.buffer); // exibe a entrada do buffer.
    
    if(irc.buffer != NULL) {
      free(irc.buffer);
    }
  } while(1);

  // libera o estado da conexão.
  SSL_free(irc.ssl);
  SSL_CTX_free(irc.ctx);
  close(irc.sockfd);
  
  return(0);
}
