/*

  IRC Funtions,
  funtions necessarys to use irc protocol(server / client).

*/

/* initialize new irc data structure */
irc_d irc;

/* m_Destroy - destroy multidimensional array */
void
m_Destroy(char **m, int mSize) {
  for(int i=0; i < mSize; i++) {
    free(m[i]);
  }
}

/* Split - split message from ch1 to ch2 */
char*
split(char *str, char ch1, char ch2)
{
  int i, j = 0;
  char *splited = (char*) calloc(1024,1);

  for(i=1; i < strlen(str); i++)
  {
    if(str[i-1] == ch1)
    {
      while(str[i] != ch2)
      {
        splited[j] = str[i];
        i++; j++;
      }
      return splited;
    }
  }

  free(splited);
  return NULL;
}

/* r_Buffer - read buffer messages */
char*
r_Buffer()
{
  int _mBytes, _mtBytes = 0;

  char *_mRecv = (char*) calloc(M_LEN, 1);
  char _mLines[M_LEN] = {};

  memset(_mLines, 0x0, M_LEN);

  // read data over ssl connection, if any
  if(irc.isSSL) {
    do {
      memset(_mLines, 0x0, M_LEN);
      _mBytes = SSL_read(irc.ssl, _mLines, M_LEN);
      
      if(_mBytes <= 0)
        break;
      _mtBytes += _mBytes;
      
      strcat(_mRecv, _mLines);
    } while(SSL_pending(irc.ssl));
  }
  else {
    while((recv(irc.sockfd, _mLines, M_LEN, 0)) > 0) {
      strcat(_mRecv, _mLines);
      memset(_mLines, 0x0, M_LEN);
    }
  }

  // respose pong message
  if((b_Pong(_mRecv)) == 0) {
    return _mRecv;
  }

  char *_mData[3] = {
    (char*)(g_Host(_mRecv, '!', '@')),
    g_Chan(_mRecv),
    g_Msg(_mRecv)
  };

  if(_mData[0] == NULL) {
    m_Destroy(_mData, 3);
    return _mRecv;
  }

  b_Exec(_mData[0], _mData[1], _mData[2]); // execute bot commands

  m_Destroy(_mData, 3); // release
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

/* g_mCode - get message Code */ 
char*
g_mCode(char *msg) {
  char *start = strchr(msg, 32);
  char *code;

  if(start == NULL) {
    return NULL;
  }

  code = (char*) calloc(3, 1);

  int i = 0;
  do {
    code[i] = start[i+1];
    i++;
  } while(start[i+1] != 32 && i <= 3);

  return code;
}

/* g_Nick - get nick */
char*
g_Nick(char *msg) {
  int i = 0;

  char *_mData[] = {
    (char*) calloc(strlen(msg), 1), // copy message
    (char*) calloc(9, 1) // nick
  };
  
  strncpy(_mData[0], msg, strlen(msg)); // copy message

  do {
    _mData[1][i] = _mData[0][i];
    i++;
  } while(_mData[0][i] != '!' || i < 8);

  if(_mData[1] == NULL) {
    m_Destroy(_mData, 2);
    return NULL;
  }

  free(_mData[0]);
  return _mData[1];
}

/* g_Chan - get channel */
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
    (_mSpl[i] != '\r') && (_mSpl[i] != 32) && i < 512) {
    _mData[1][i-1] = _mSpl[i]; i++;
  }

  free(_mData[0]);
  return _mData[1];
}

/* g_Msg - get message */
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

  while(_mSpl[i] != '\r' && i < strlen(msg)) {
    _mData[1][i-2] = _mSpl[i]; i++;
  }
  
  free(_mData[0]);
  return _mData[1];
}

/* g_Host - get host */
char*
g_Host(char *mCopy, char ch1, char ch2)
{
  int i = 0;
  char *u_Host = (char*) calloc(512,1);
  char *split;

  split = strchr(mCopy, ch1);

  if(split == NULL) {
    free(u_Host);
    return NULL;
  }

  do {
    u_Host[i] = split[i+2];
    i++;
  } while(split[i+2] != ch2 && i < 9);

  return u_Host;
}

/* get_nArg - get n arg */ 
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

/* str_Cmp - compare strings */
int
str_Cmp(char *d1, char *d2)
{
  if(strcmp(d2, d1) == 0) {
    // printf("%s - %s: iguais.\n", d1, d2);
    return(1);
  }
  else {
    // printf("%s - %s: diferentes.\n", d1, d2);
    return(0);
  }
}
 
/* b_Exec - execute bot commands */
int
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
  if(!strcmp(uHost, BOT_ADM)) {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++) {
      if(str_Cmp(_modKeys[i], _cKey)) {
        mod_funcs[i](uChan, _mCopy);
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

/* b_Creds - set credentials */
void
b_Creds(char *uNick, char *uPass)
{  
  char *_uCreds = (char*) calloc(B_LEN, 1); // allocates user creds

  // format buffers
  snprintf(_uCreds, B_LEN, "IDENTIFY %s %s", uNick, uPass);

  b_Priv(_uCreds, "NickServ");

  free(_uCreds);
}


/* b_Join - join to channel */
void
b_Join(char *uChans)
{
  char *_uJoin = (char*) calloc(B_LEN, 1); // alocattes join

  snprintf(_uJoin, B_LEN, "JOIN %s\r\n", uChans); // format join buffer

  m_Send(_uJoin); // send join command
  
  free(_uJoin);
}


/* b_Pong - pong */
int
b_Pong(char *msg)
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

/* b_Priv - send privmsg */
void
b_Priv(char *mArg, char *mDest) {
  char *_pMsg = (char*) calloc(B_LEN*2, 1); // allocates msg_tmp
  
  snprintf(_pMsg, B_LEN*2, "PRIVMSG %s :%s\r\n", mDest, mArg); // format msg_tmp buffer
  
  m_Send(_pMsg); // send message

  free(_pMsg);
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

  if(!irc.isSSL) {
    return sockfd;
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
