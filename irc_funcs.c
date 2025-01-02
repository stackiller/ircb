/*

  Funções IRC,
  funções necessárias para usar o protocolo irc.

*/

/* inicializa uma nova estrutura de dados da conexão */
irc_d irc;

/* r_Buffer - lê o buffer de dados */
char*
r_Buffer()
{
  int bytes, tBytes = 0;

  char *buffer = (char*) calloc(BUFFER_SIZE, 1);
  char buffer_lines[BUFFER_LINE_SIZE] = {};

  memset(buffer_lines, 0x0, BUFFER_LINE_SIZE);

  // lê os dados e os concatena no buffer
  do {
    memset(buffer_lines, 0x0, BUFFER_LINE_SIZE);
    bytes = SSL_read(irc.ssl, buffer_lines, BUFFER_LINE_SIZE);

    if(bytes <= 0)
      break;
    tBytes += bytes;

    strcat(buffer, buffer_lines);
  } while(SSL_pending(irc.ssl));
  

  // responde ao ping
  if((bot_Pong(buffer)) == 0) {
    return buffer;
  }

  // fragmenta os dados da mensagem numa matriz
  char *msg_Data[3] = {
    get_Src(buffer),
    get_Dst(buffer),
    get_Msg(buffer)
  };

  // checa se algum dos campos é nulo.
  for(int c=0; c < 3; c++) {
    if (msg_Data[c] == NULL) {
      matrix_Destroy(msg_Data, 3);
      return buffer;
    }
  }

  bot_Exec(msg_Data[0], msg_Data[1], msg_Data[2]); // executa os comandos do bot
  matrix_Destroy(msg_Data, 3); // libera a matriz

  return buffer;
}

/* get_Src - obtém o remetente da mensagem */
char*
get_Src(char *msg)
{
  char ch_intervals[] = {'!', '@'};
  int i = 0, j=0;
  char *src_host = (char*) calloc(20,1);
  char *split_msg;

  char ignored_characters[] = {'!', '~', '+', '@', '%'};

  split_msg = strchr(msg, ch_intervals[0]);

  if(split_msg == NULL) {
    free(src_host);
    return NULL;
  }

  for(int c=0; c < 4; c++) {
    if(split_msg[i] == ignored_characters[c]) {
      i++;
    }
  }

  do {
    src_host[j] = split_msg[i];
    i++; j++;
  } while(split_msg[i] != ch_intervals[1] && i < 20);

  return src_host;
}

/* get_Dst - obtém o destinatário da mensagem */
char*
get_Dst(char *msg)
{
  char *Messages[] = {
    (char*) calloc(strlen(msg), 1), // cópia da mensagem não formatada
    (char*) calloc(strlen(msg), 1) // cópia da mensagem formatada
  };

  char *Privmsg_flag = get_nArg(msg, 1);

  if ( Privmsg_flag == NULL ) {
    matrix_Destroy(Messages, 2);
    return NULL;
  }

  if(!str_Cmp(Privmsg_flag, "PRIVMSG")) {
    matrix_Destroy(Messages, 2);
    return NULL;
  }

  else {
    strncpy(Messages[0], msg, strlen(msg));
    
    Messages[1] = get_nArg(Messages[0], 2);

    if(Messages[1] == NULL) {
      matrix_Destroy(Messages, 2);
      return NULL;
    }
  }
  release(Messages[0]);
  return Messages[1];
}

/* get_Msg - obtém a mensagem */
char*
get_Msg(char *msg)
{
  int i = 2;

  char *SplitMessage;
  char *Messages[] = {
    (char*) calloc(strlen(msg), 1), // cópia do mensagem não formatada
    (char*) calloc(strlen(msg), 1) // cópia da mensagem formatada
  };

  char *Privmsg_flag = get_nArg(msg, 1);

  if(!str_Cmp(Privmsg_flag, "PRIVMSG")) {
    matrix_Destroy(Messages, 2);
    return NULL;
  }

  else {
    strncpy(Messages[0], msg, strlen(msg));
    
    SplitMessage = strstr(Messages[0], " :");

    if(SplitMessage == NULL) {
      matrix_Destroy(Messages, 2);
      return NULL;
    }

    while(SplitMessage[i] != '\r') {
      Messages[1][i-2] = SplitMessage[i]; i++;
    }
  }
  release(Messages[0]);
  return Messages[1];
}

/* get_nArg - obtém o n arg */ 
char*
get_nArg(char *msg, int nArg_index)
{
  int i = 0, j = 0, k = 0;
  char *nArg = string("");

  for(; i<strlen(msg); i++)
  {
    if(k == nArg_index)
    {
      while((msg[i] != 32) && (msg[i] != '\r'))
      {
        nArg[j] = msg[i];
        i++; j++;
        nArg = realoca(nArg, j+1);
        if(checkNull(nArg)) {
          return NULL;
        }
      }
      return nArg;
    }
    else if(msg[i] == 32) {
      k++;
    }
  }
  
  free(nArg);
  return NULL;
}

/* get_Args - obtém todos os argumentos seguidos após o comando
de controle do bot */
char*
get_Args(char *msg)
{
  int i = 0, j = 0, k = 0;
  char *args = string("");

  for(; i<strlen(msg); i++)
  {
    if(k == 1)
    {
      while((msg[i] != '\r'))
      {
        args[j] = msg[i];
        i++; j++;
        args = realoca(args, j+1);
        if(checkNull(args)) {
          return NULL;
        }
      }
      return args;
    }
    else if(msg[i] == 32) k++;
  }
  
  free(args);
  return NULL;
}
 
/* bot_Exec - executa os comandos do bot */
int
bot_Exec(char *src, char *dst, char *msg)
{
  typedef void (*mod_f)(char[], char[]);
  char *_msg = (char*) calloc(strlen(msg), 1);
  char *_cKey; // chave de comando na mensagem.

  strncpy(_msg, msg, strlen(msg));
  _cKey = get_nArg(_msg, 0);

  // chaves dos módulos.
  char *_modKeys[] = {
    "bjoin\0", "bnick\0", "bkick\0", "bpart\0", "bsh\0"
  };
 
  // lista de endereço de funções dos módulos.
  mod_f mod_funcs[ARRAY_SIZE(_modKeys)] = {
    &m_Join, &m_Nick, &m_Kick, &m_Part, &m_Sh
  };

  // compara o userhost, afim de executar somente se for o adm.
  if(!strcmp(src, BOT_ADM))
  {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++)
    {
      if(str_Cmp(_modKeys[i], _cKey))
      {
        if(dst[0] == '#') // checa se o destinatário é um canal.
        {
          mod_funcs[i](dst, _msg);
        }
        else // senão enviar os dados para o usuário remetente.
        {
          mod_funcs[i](src, _msg);
        }
      }
    }
  }

  free(_msg);
  return 0;
}

/* bot_Nick - define o nick do bot */
void
bot_Nick(char *nick)
{
  char *datas[] = {
    (char*) calloc(BOT_MAX_LEN, 1), // nick.
    (char*) calloc(BOT_MAX_LEN, 1) // usuário.
  };

  // formata os buffers
  snprintf(datas[0], BOT_MAX_LEN, "NICK %s\r\n", nick);
  snprintf(datas[1], BOT_MAX_LEN, "USER %s %s %s %s\r\n", nick, nick, nick, nick);

  msg_Send(datas[0]); // envia o nick.
  msg_Send(datas[1]); // envia a identificação.

  matrix_Destroy(datas, 2);
}

/* bot_Creds - define e envia as credenciais. */
void
bot_Creds(char *nick, char *pass)
{  
  char *creds = (char*) calloc(BOT_MAX_LEN, 1); // aloca as credenciais do bot.
  snprintf(creds, BOT_MAX_LEN, "IDENTIFY %s %s\r\n", nick, pass); // formata os buffers
  bot_Priv(creds, "NickServ");
  free(creds);
}


/* bot_Join - entra para o canal. */
void
bot_Join(char *chans)
{
  char *join = (char*) calloc(BOT_MAX_LEN*10, 1);
  snprintf(join, BOT_MAX_LEN, "JOIN %s\r\n", chans);
  msg_Send(join);
  free(join);
}


/* bot_Pong - pong */
int
bot_Pong(char *msg)
{
  if((strlen(msg) > 30)
  && ((strstr(msg, "PING :") == NULL)
  || (strstr(msg, ":PING") != NULL ))) {
    return 1;
  }

  char *ping;
  char *datas[] = {
    (char*) calloc(strlen(msg), 1), // cópia da mensagem original.
    (char*) calloc(BOT_MAX_LEN, 1) // mensagem de pong.
  };

  strncpy(datas[0], msg, strlen(msg));
  ping = strstr(datas[0], " :");
  snprintf(datas[1], BOT_MAX_LEN, "PONG%s", ping);
  printf("\n[%s%s*%s] %s\n", tBlue, tBlink, tRs, datas[1]);
  msg_Send(datas[1]);
  
  matrix_Destroy(datas, 2);
  return 0;
}

/* bot_Priv - função privmsg */
void
bot_Priv(char *mArg, char *mDest) {
  char *pMsg = (char*) calloc(512, 1);
  char *privmsg_models[] = {
    "PRIVMSG %s :%s\r\n",
    "PRIVMSG %s %s\r\n"
  };

  // divide a mensagem em linhas e os envia separadamente.
  char *line = strtok(mArg, "\n");
  while (line != NULL) {
    mDest[0] == '#' ?
      snprintf(pMsg, BOT_MAX_LEN*2, privmsg_models[1], mDest, line) :
      snprintf(pMsg, BOT_MAX_LEN*2, privmsg_models[0], mDest, line);
    
    msg_Send(pMsg);
    memset(pMsg, 0x0, BOT_MAX_LEN*2);
    line = strtok(NULL, "\n");
  }

  free(pMsg);
}

/* bot_Part - parte do canal */
void
bot_Part(char *chans) {
  char *_pMsg = (char*) calloc(BOT_MAX_LEN, 1);
  snprintf(_pMsg, BOT_MAX_LEN, "PART %s :%s\r\n", chans, "afk");
  msg_Send(_pMsg);
  free(_pMsg);
}

/* bot_Shell - executa comandos do sistema */
void
bot_Shell(char *dst, char *sh_command) {
  FILE *stream = NULL;

  // executa o comando fornecido e armazena a saída.
  if ( (stream = popen(sh_command, "r")) == NULL ) {
    bot_Priv("Falha na execução do comando.", dst);
  }
   
  int ch = 0, size = 0;
  char *out = string("");
 
  while ( (ch = fgetc(stream)) != EOF ) {
    out[size] = ch;
    size++;
    out = realoca(out, size+1);
    if(checkNull(out)) {
      bot_Priv("Comando com retorno nulo.", dst);
    }
  }

  bot_Priv(out, dst); // envia a saída do comando.
  memset(out, 0x0, strlen(out));
  free(out);
  pclose(stream);
}

/* new_Conn - cria uma nova conexão de socket */
int
new_Conn(const char *hostname, int port) {
  static struct sockaddr_in server; // estrutura sockaddr_in
  struct hostent *host; // estrutura hostent
  int sockfd; // descritor do socket.

  // obtém o endereço ipv4 pelo nome de host.
  if((host = gethostbyname(hostname)) == NULL) {
    printf("[@] Endereço inválido: %s\n", hostname);
    exit(1);
  }

  // cria um socket TCP/IP.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    close(sockfd);
    perror(FAIL_SOCK);
  }
 
  // atribui os dados de conexão aos campos.
  server.sin_family = AF_INET; 
  server.sin_port = htons(port);
  server.sin_addr.s_addr = *(long*)(host->h_addr);
  memset(&server.sin_zero, 0x0, 8);

  // conecta ao servidor IRC.
  if((connect(sockfd, (struct sockaddr*) &server, sizeof(server))) != 0) {
    close(sockfd);
    perror(FAIL_CONN);
  }

  // anexa o contexto SSL ao socket.
  SSL_set_fd(irc.ssl, sockfd);

  // inicializa a conexão ssl com o servidor IRC.
  if(SSL_connect(irc.ssl) == FAIL) {
    ERR_print_errors_fp(stderr);
  }
  else {
    printf(
      "[%s%s*%s] Conectado com %s.\n",
      tGreen, tBlink, tRs, SSL_get_cipher(irc.ssl));
  }
  show_Certs(irc.ssl); // mostra os certificados.

  return sockfd;
}

/* bot_Header - cabeçalho do bot. */
void
bot_Header() {
  printf("%s %s %s\n", tRed, bot_Brand, tRs);
}

/* msg_Send - envia uma mensagem. */ 
void
msg_Send(char *msg) {
  SSL_write(irc.ssl, msg, strlen(msg));
}

/* usage - função de ajuda, modo de uso. */
void
usage(char *c_name) {
  bot_Header();
  printf(
    "Use:\n%s <server_addr> <port> <nick> \'<password>\' \'#channel1,#channel2,#channel3..\'\n", c_name);
  exit(0);
}
