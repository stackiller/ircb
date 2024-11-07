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
  if((b_Pong(buffer)) == 0) {
    return buffer;
  }

  // fragmenta os dados da mensagem numa matriz
  char *msg_Data[3] = {
    (char*)(get_Src(buffer)),
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

/* get_Dst - obtém o destinatário da mensagem */
char*
get_Dst(char *msg)
{
  char *Messages[] = {
    (char*) calloc(strlen(msg), 1), // cópia do mensagem não formatada
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

/* get_Msg - get message */
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

/* get_Src - get host */
char*
get_Src(char *mCopy)
{
  char ch_intervals[] = {'!', '@'};
  int i = 0, j=0;
  char *u_Host = (char*) calloc(20,1);
  char *split;

  char ignored_characters[] = {'!', '~', '+', '@', '%'};

  split = strchr(mCopy, ch_intervals[0]);

  if(split == NULL) {
    free(u_Host);
    return NULL;
  }

  for(int c=0; c < 4; c++) {
    if(split[i] == ignored_characters[c]) {
      i++;
    }
  }

  do {
    u_Host[j] = split[i];
    i++; j++;
  } while(split[i] != ch_intervals[1] && i < 20);

  return u_Host;
}

/* get_nArg - get n arg */ 
char*
get_nArg(char *mCopy, int n_arg)
{
  int i = 0, j = 0, k = 0;
  char *n_String = (char*) calloc(1024, 1);

  for(i=0; i<strlen(mCopy); i++)
  {
    if(k == n_arg)
    {
      while((mCopy[i] != 32) && (mCopy[i] != '\0'))
      {
        n_String[j] = mCopy[i];
        i++; j++;
      }
      return n_String;
    }
    else if(mCopy[i] == 32) k++;
  }
  
  free(n_String);
  return NULL;
}

char*
g_Args(char *msg)
{
  int i = 0, j = 0, k = 0;
  char *string = (char*) calloc(strlen(msg), 1);

  for(; i<strlen(msg); i++)
  {
    if(k == 1)
    {
      while((msg[i] != '\0')) {
        string[j] = msg[i];
        i++; j++;
      }
      printf("@@@ STRING: %s\n", string);
      return string;
    }
    else if(msg[i] == 32) k++;
  }
  
  free(string);
  return NULL;
}

/* str_Cmp - compare strings */
int
str_Cmp(char *d1, char *d2)
{
  if(strcmp(d2, d1) == 0) {
    return(1);
  }
  else {
    return(0);
  }
}
 
/* bot_Exec - execute bot commands */
int
bot_Exec(char *src, char *dst, char *msg)
{
  typedef void (*mod_f)(char[], char[]);
  char *_mCopy = (char*) calloc(strlen(msg), 1);
  char *_cKey; // command key in message

  strncpy(_mCopy, msg, strlen(msg));
  _cKey = get_nArg(_mCopy, 0);

  // moderator functions key.
  char *_modKeys[] = {
    "bjoin\0", "bnick\0", "bkick\0", "bpart\0", "bsh\0"
  };
 
  // moderator functions.
  mod_f mod_funcs[ARRAY_SIZE(_modKeys)] = {
    &m_Join, &m_Nick, &m_Kick, &m_Part, &m_Sh
  };

  // compare userhost for execute commands.
  if(!strcmp(src, BOT_ADM)) {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++) {
      if(str_Cmp(_modKeys[i], _cKey)) {
        if(dst[0] == '#') {
          mod_funcs[i](dst, _mCopy);
        } else {
          mod_funcs[i](src, _mCopy);
        }
      }
    }
  }

  free(_mCopy);
  return 0;
}

/* b_Nick - set nick */
void
b_Nick(char *uNick)
{
  char *msg_Data[] = {
    (char*) calloc(B_LEN, 1), // nick
    (char*) calloc(B_LEN, 1) // user
  };

  // format buffers
  snprintf(msg_Data[0], B_LEN, "NICK %s\r\n", uNick);
  snprintf(msg_Data[1], B_LEN, "USER %s %s %s %s\r\n", uNick, uNick, uNick, uNick);

  m_Send(msg_Data[0]); // send nick
  m_Send(msg_Data[1]); // send ident

  matrix_Destroy(msg_Data, 2);
}

/* b_Creds - set credentials */
void
b_Creds(char *uNick, char *uPass)
{  
  char *_uCreds = (char*) calloc(B_LEN, 1); // allocates user creds

  // format buffers
  snprintf(_uCreds, B_LEN, "IDENTIFY %s %s", uNick, uPass);

  bot_Priv(_uCreds, "NickServ");

  free(_uCreds);
}


/* b_Join - join to channel */
void
b_Join(char *uChans)
{
  char *_uJoin = (char*) calloc(B_LEN*10, 1); // alocattes join

  snprintf(_uJoin, B_LEN, "JOIN %s\r\n", uChans); // format join buffer

  printf("b_Join: %s\n", _uJoin);

  m_Send(_uJoin); // send join command
  
  free(_uJoin);
}


/* b_Pong - pong */
int
b_Pong(char *msg)
{
  if((strlen(msg) > 30)
  && ((strstr(msg, "PING :") == NULL)
  || (strstr(msg, ":PING :")))) {
    return 1;
  }

  char *_uPing;
  char *msg_Data[] = {
    (char*) calloc(strlen(msg), 1), // copy message
    (char*) calloc(B_LEN, 1) // pong
  };

  strncpy(msg_Data[0], msg, strlen(msg));
  
  _uPing = strstr(msg_Data[0], " :");
  snprintf(msg_Data[1], B_LEN, "PONG%s", _uPing);

  printf("\n[%s%s*%s] %s\n", tBlue, tBlink, tRs, msg_Data[1]);
  m_Send(msg_Data[1]);
  
  matrix_Destroy(msg_Data, 2);
  return 0;
}

/* bot_Priv - send privmsg */
void
bot_Priv(char *mArg, char *mDest) {
  char *_pMsg = (char*) calloc(B_LEN*2, 1); // allocates msg_tmp
  char *privmsg_models[] = {
    "PRIVMSG %s :%s\r\n",
    "PRIVMSG %s %s\r\n"
  };

  char *line = strtok(mArg, "\n");
  while (line != NULL) {
    mDest[0] == '#' ?
      snprintf(_pMsg, B_LEN*2, privmsg_models[1], mDest, line) :
      snprintf(_pMsg, B_LEN*2, privmsg_models[0], mDest, line);
    
    m_Send(_pMsg); // send message
    memset(_pMsg, 0x0, B_LEN*2);
    line = strtok(NULL, "\n");
  }

  free(_pMsg);
}

/* bot_Part - part of channel */
void
bot_Part(char *chans) {
  char *_pMsg = (char*) calloc(B_LEN, 1);

  snprintf(_pMsg, B_LEN, "PART %s :%s\r\n", chans, "afk");

  printf("bot_Part: %s\n", _pMsg);

  m_Send(_pMsg);
  free(_pMsg);
}

/* bot_Shell - part of channel */
void
bot_Shell(char *dst, char *sh_command) {
  FILE *stream_out = NULL;

  if ( (stream_out = popen(sh_command, "r")) == NULL ) {
    bot_Priv("Falha na execução do comando.", dst);
  }
   
  int ch, size = 0;
  char *output = string("\0");

  while((ch = fgetc(stream_out)) != EOF) {
    output[size] = ch;
    size++;
    output = realoca(output, size + 1);
  }

  // snprintf(_pMsg, B_LEN, "PART %s :%s\r\n", chans, "afk");

  // printf("bot_Part: %s\n", _pMsg);

  bot_Priv(output, dst);
  free(output);
  pclose(stream_out);
}

/* new_Conn - create new socket connection */
int
new_Conn(const char *hostname, int port) {
  static struct sockaddr_in server; // struct sockaddr_in
  struct hostent *host; // struct hostent
  int sockfd; // socket file descriptor

  // get addresss from hostname
  if((host = gethostbyname(hostname)) == NULL) {
    printf("[@] wrong host: %s\n", hostname);
    exit(1);
  }

  // create socket TCP/IP
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    close(sockfd);
    perror(FAIL_SOCK);
  }
 
  // assign data to fields
  server.sin_family = AF_INET; 
  server.sin_port = htons(port);
  server.sin_addr.s_addr = *(long*)(host->h_addr);
  memset(&server.sin_zero, 0x0, 8);

  // connect to irc server
  if((connect(sockfd, (struct sockaddr*) &server, sizeof(server))) != 0) {
    close(sockfd);
    perror(FAIL_CONN);
  }

  // attach SSL context to socket
  SSL_set_fd(irc.ssl, sockfd);

  // connect with ssl
  if(SSL_connect(irc.ssl) == FAIL) {
    ERR_print_errors_fp(stderr);
  }
  else {
    printf(
      "[%s%s*%s] Connected with %s encryption\n",
      tGreen, tBlink, tRs, SSL_get_cipher(irc.ssl));
  }
  show_Certs(irc.ssl); // show certificates

  return sockfd;
}

/* b_Header - header to bot */
void
b_Header() {
  printf("%s %s %s\n", tRed, b_Brand, tRs);
}

/* m_Send - send SSL Message */ 
void
m_Send(char *msg) {
  SSL_write(irc.ssl, msg, strlen(msg));
}

/* usage - helper function of program */
void
usage(char *c_name) {
  b_Header();
  printf(
    "Use:\n%s <server_addr> <port> <nick> \'<password>\' \'#channel1,#channel2,#channel3..\' <optional flag: --nossl>\n", c_name);
  exit(0);
}
