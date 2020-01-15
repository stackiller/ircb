/*

  IRC Funtions,
  funtions necessarys to use irc protocol(server / client).

*/

/* initialize new irc data structure */
irc_d irc;

/* m_Destroy */
void
m_Destroy(char **m, int mSize) {
  for(int i=0; i < mSize; i++)
    free(m[i]);
}

/* s_Msg */
void
s_Msg(char **msg_data) {
  printf("[%s] %s: %s\n", msg_data[1], msg_data[2], msg_data[3]);
}

/* r_Buffer */
char*
r_Buffer()
{
  int _mBytes, _mtBytes = 0;

  char *_mRecv = (char*) calloc(M_LEN, 1);
  char *_mLines = (char*) calloc(M_LEN, 1);

  // read data over ssl connection, if any
  if(irc.isSSL) {
    do {
      memset(_mLines, 0x0, M_LEN);
      _mBytes = SSL_read(irc.ssl, _mLines, M_LEN);
      
      if(_mBytes <= 0)
        break;
      _mtBytes += _mBytes;
      
      strncat(_mRecv, _mLines, _mBytes);
    } while(SSL_pending(irc.ssl));
  }
  else {
    recv(irc.sockfd, _mRecv, M_LEN, 0);
  }

  // respose pong message
  if((b_Pong(irc.ssl, _mRecv)) == 0)
    return _mRecv;

  // parse data from message
  char *_mData[4] = {
    g_Host(_mRecv, '!', '@'), g_Nick(_mRecv),
    g_Chan(_mRecv), g_Msg(_mRecv)
  };

  for(int i=0; i < 4; i++) {
    if(_mData[i] != NULL) continue;
    else {
      m_Destroy(_mData, 4);
      return _mRecv;
    }
  } 
  
  b_Exec(_mData[0], _mData[2], _mData[3]); // execute bot commands

  m_Destroy(_mData, 4); // release
  return _mRecv;
}

/* str_Chr */
char*
str_Chr(char *str, char ch)
{
  int i=0;
  char *str_spl = (char*) calloc(strlen(str), 1);

  while(str[i] != ch)
  {
    if(i == strlen(str)) {
      free(str_spl);
      return NULL;
    }

    str_spl[i] = str[i];
    i++;
  }

  return str_spl;
}

/* g_Nick */
char*
g_Nick(char *msg)
{
  char *_mToken;  
  char *_mData[] = {
    (char*) calloc(strlen(msg), 1), // msg
    (char*) calloc(512, 1) // nick
  };

  strncpy(_mData[0], msg, strlen(msg));

  _mToken = str_Chr(_mData[0], '!');
  if(_mToken == NULL) {
     free(_mToken); m_Destroy(_mData, 2);
    return NULL;
  }

  for(int i=0; i<strlen(_mToken); i++)
    _mData[1][i] = _mToken[i+1];
  
  free(_mToken); free(_mData[0]);
  return _mData[1];
}

/* g_Chan */
char*
g_Chan(char *msg)
{
  int i = 1;

  char *_mSpl; // splited message
  char *_mData[] = {
    (char*) calloc(strlen(msg), 1), // copy message
    (char*) calloc(512, 1) // channel
  };

  strncpy(_mData[0], msg, strlen(msg)); // copy message
  _mSpl = strstr(_mData[0], " #"); // split message

  if(_mSpl == NULL) {
    m_Destroy(_mData, 2);
    return NULL;
  }

  while(
    (_mSpl[i] != '\r') && (_mSpl[i] != 32) ) {
    _mData[1][i-1] = _mSpl[i]; i++;
  }

  free(_mData[0]);
  return _mData[1];
}

/* g_Msg */
char*
g_Msg(char *msg)
{
  int i = 2;

  char *_mSpl;
  char *_mData[] = {
    (char*) calloc(strlen(msg), 1), // copy msg
    (char*) calloc(strlen(msg), 1) // received msg
  };

  strncpy(_mData[0], msg, strlen(msg));
  _mSpl = strstr(_mData[0], " :");

  if(_mSpl == NULL) {
    m_Destroy(_mData, 2);
    return NULL;
  }

  while(_mSpl[i] != '\r') {
    _mData[1][i-2] = _mSpl[i]; i++;
  }
  
  free(_mData[0]);
  return _mData[1];
}

/* g_Host */
char*
g_Host(char *mCopy, char ch1, char ch2)
{
  int i, j = 0;
  char *u_Host = (char*) calloc(512,1);

  for(i=1; i < strlen(mCopy); i++)
  {
    if(mCopy[i-1] == ch1)
    {
      while(mCopy[i] != ch2)
      {
        u_Host[j] = mCopy[i];
        i++; j++;
      }
      return u_Host;
    }
  }

  free(u_Host);
  return NULL;
}

/* get_nArg */ 
char*
g_nArg(char *mCopy, int n_arg)
{
  int i = 0, j = 0, k = 0;
  char *n_String = (char*) calloc(512, 1);

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

/* str_Cmp */
int
str_Cmp(char *d1, char *d2)
{
  if(strcmp(d2, d1) == 0) {
    printf("%s - %s: iguais.\n", d1, d2);
    return(1);
  }
  else {
    printf("%s - %s: diferentes.\n", d1, d2);
    return(0);
  }
}
 
/* b_Exec */
void
b_Exec(char *uHost, char *uChan, char *uMsg)
{
  typedef void (*mod_f)(char[], char[]);
  char *_mCopy = (char*) calloc(strlen(uMsg), 1);
  char *_cKey; // command key in message

  strncpy(_mCopy, uMsg, strlen(uMsg));
  _cKey = g_nArg(_mCopy, 0);

  // moderator functions key.
  char *_modKeys[] = {
    "bjoin\0", "bnick\0", "bkick\0"
  };
 
  // moderator functions.
  mod_f mod_funcs[ARRAY_SIZE(_modKeys)] = {
    &m_Join, &m_Nick, &m_Kick
  };

  // compare userhost for execute commands.
  if(str_Cmp(uHost, BOT_ADM)) {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++) {
      if(str_Cmp(_modKeys[i], _cKey)) {
        mod_funcs[i](uChan, _mCopy);
      }
    }
  }
  free(_mCopy);
}

/* b_Nick */
void
b_Nick(char *uNick)
{
  char *_mData[] = {
    (char*) calloc(B_LEN, 1), // nick
    (char*) calloc(B_LEN, 1) // user
  };

  // format buffers
  snprintf(_mData[0], B_LEN, "NICK %s\r\n", uNick);
  snprintf(_mData[1], B_LEN, "USER %s %s %s %s\r\n", uNick, uNick, uNick, uNick);

  m_Send(_mData[0]); // send nick
  m_Send(_mData[1]); // send ident

  m_Destroy(_mData, 2);
}

/* b_Creds */
void
b_Creds(char *uNick, char *uPass)
{  
  char *_uCreds = (char*) calloc(B_LEN, 1); // allocates user creds

  // format buffers
  snprintf(_uCreds, B_LEN, "IDENTIFY %s %s\r\n", uNick, uPass);
  m_Send(_uCreds); // send nick

  free(_uCreds);
}

/* b_Join */
void
b_Join(char *uChans)
{
  char *_uJoin = (char*) calloc(B_LEN, 1); // alocattes join

  snprintf(_uJoin, B_LEN, "JOIN %s\r\n", uChans); // format join buffer
  m_Send(_uJoin); // send join command

  free(_uJoin);
}

/* b_Pong */
int
b_Pong(SSL *ssl, char *msg)
{
  if((msg[0] == ':') && (strstr(msg, "PING :") == NULL))
    return 1;

  char *_uPing;
  char *_mData[] = {
    (char*) calloc(strlen(msg), 1), // copy message
    (char*) calloc(B_LEN, 1) // pong
  };

  strncpy(_mData[0], msg, strlen(msg));
  
  _uPing = strstr(_mData[0], " :");
  snprintf(_mData[1], B_LEN, "PONG%s", _uPing);

  printf("\n[%s%s*%s] %s\n", tBlue, tBlink, tRs, _mData[1]);
  m_Send(_mData[1]);
  
  m_Destroy(_mData, 2);
  return 0;
}

/* b_Priv */
void
b_Priv(char *mArg, char *mDest)
{
  char *_pMsg = (char*) calloc(B_LEN, 1); // allocates msg_tmp
  
  snprintf(_pMsg, B_LEN, "PRIVMSG %s :%s\r\n", mDest, mArg); // format msg_tmp buffer
  m_Send(_pMsg); // send message

  free(_pMsg);
}

/* new_Conn */
int
new_Conn(const char *hostname, int port)
{
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

  if(!irc.isSSL) {
    return sockfd;
  }

  // attach SSL context to socket
  SSL_set_fd(irc.ssl, sockfd);

  // connect with ssl
  if(SSL_connect(irc.ssl) == FAIL)
    ERR_print_errors_fp(stderr);
  else {
    printf(
      "[%s%s*%s] Connected with %s encryption\n",
      tGreen, tBlink, tRs, SSL_get_cipher(irc.ssl));
  }
  show_Certs(irc.ssl); // show certificates

  return sockfd;
}

/* b_Header */
void
b_Header() {
  for(int i=0; i < ARRAY_SIZE(b_Brand); i++)
    printf("%s %s %s\n", tGreen, b_Brand[i], tRs);
}

/* m_Send - send SSL Message */ 
void
m_Send(char *msg) {
  irc.isSSL ?
  SSL_write(irc.ssl, msg, strlen(msg)) :
  send(irc.sockfd, msg, strlen(msg), 0);
}

/* usage - helper function of program */
void
usage(char *c_name) {
  b_Header();
  printf(
    "Use:\n%s <server_addr> <port> <nick> \'<password>\' \'#channel1,#channel2,#channel3..\' <optional flag: --nossl>\n", c_name);
  exit(0);
}