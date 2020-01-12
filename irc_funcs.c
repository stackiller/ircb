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

/* str_chr - split str from char */
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

/* get_nick - get nick */
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

  for(int i=0; i<strlen(_mToken); i++) {
    _mData[1][i] = _mToken[i+1];
  }
  
  free(_mToken); free(_mData[0]);
  return _mData[1];
}

/* get_chan - get channel */
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

/* get_msg - get message */
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

/* get_host - get user host */
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

/* get_nArg - get n-argument of message */ 
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

/* print_msg - print message */
void
s_Msg(char **msg_data) {
  printf("[%s] %s: %s\n", msg_data[1], msg_data[2], msg_data[3]);
}

/* str_cmp - compare strings */
int
str_cmp(char *d1, char *d2)
{
  printf("Comparando %s - %s\n", d1, d2);
  if(strncmp(d2, d1, strlen(d1)) == 0)
    return(1);
  else
    return(0);
}
 
/* b_exc - get command from message */
void
Bexec(char *uHost, char *uChan, char *uMsg)
{
  char *_mCopy;
  typedef void (*funcs)(char[], char[]);

  _mCopy = (char*) calloc(strlen(uMsg), 1);
  strncpy(_mCopy, uMsg, strlen(uMsg));

  // moderator functions key.
  char *_modKeys[] = {
    "bjoin", "bnick", "bkick"
  };
 
  // moderator functions.
  funcs mod_funcs[ARRAY_SIZE(_modKeys)] = {
    &b_join, &b_nick, &b_kick
  };

  // compare userhost for execute commands.
  if(str_cmp(uHost, BOT_ADM)) {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++) {
      if(str_cmp(_modKeys[i], _mCopy)) {
        mod_funcs[i](uChan, _mCopy);
      }
    }
  }
  
  free(_mCopy);
}

/* set_nick - identify user */
void
set_nick(char *nick)
{
  int nickLen = (strlen(nick) + 7);
  int userLen = (strlen(nick) * 14 + 10);

  char *set_nick = (char*) calloc(nickLen, 1);
  char *set_user = (char*) calloc(userLen, 1);

  // format buffers
  snprintf(set_nick, nickLen, "NICK %s\r\n", nick);
  snprintf(set_user, userLen, "USER %s %s %s %s\r\n", nick, nick, nick, nick);
  
  // replace null byte to new line
  set_nick[nickLen-1] = '\n';
  set_user[userLen-1] = '\n';

  // send user
  printf("[%s%s*%s] %s", fgGreen, blink, resetCl, set_nick); // show nick
  printf("[%s%s*%s] %s", fgGreen, blink, resetCl, set_user); // show ident
  SSL_write(irc.ssl, set_nick, nickLen); // send nick
  SSL_write(irc.ssl, set_user, userLen); // send ident

  free(set_nick); free(set_user);
}

/* set_creds - send credentials */
void
set_creds(char *nick, char *pass)
{
  char *set_creds; // credentials buffer
  int credsLen; // credentials len
  
  // allocates creds
  credsLen = (strlen(nick) + strlen(pass) + 12);
  set_creds = (char*) calloc(credsLen, 1);

  // format buffers
  snprintf(set_creds, credsLen, "IDENTIFY %s %s\r\n", nick, pass);
  set_creds[credsLen-1] = '\n'; // replace null byte with lf

  // send user
  printf("[%s%s*%s] %s", fgGreen, blink, resetCl, set_creds); // show nick
  SSL_write(irc.ssl, set_creds, credsLen); // send nick

  free(set_creds);
}

/* set_join - join channel */
void
set_join(char *chans)
{
  char *set_join; // join buffer
  int join_len; // size of join command

  join_len = (strlen(chans) + 7);
  set_join = (char*) calloc(join_len, sizeof(char)); // alocattes join

  snprintf(set_join, join_len, "JOIN %s\r\n", chans); // format join buffer
  set_join[join_len-1] = '\n'; // replace null byte with lf

  printf("[%s*%s] %s", fgCyan, resetCl, set_join); // show join command
  SSL_write(irc.ssl, set_join, join_len); // send join command

  free(set_join);
}

/* set_pong - send Pong response */
int
set_pong(SSL *ssl, char *msg)
{
  char *ping, *pong, *m_copy;

  if((msg[0] == ':') && (strstr(msg, "PING ") == NULL))
    return 1;

  m_copy = (char*) calloc(strlen(msg), 1);
  strncpy(m_copy, msg, strlen(msg));
  
  ping = strstr(m_copy, " :");
  pong = (char*) calloc(strlen(ping) + 4, 1);
  snprintf(pong, strlen(ping) + 7, "PONG%s", ping);

  printf("[%s%s*%s] %s", fgBlue, blink, resetCl, pong);
  SSL_write(ssl, pong, strlen(pong));
  
  free(m_copy); free(pong);
  return 0;
}

/* priv_msg - send priv messages */
void
priv_msg(char *msg, char *dst)
{
  char *msg_tmp; // tmp message
  int msg_len; // size of message

  msg_len = (strlen(dst) + strlen(msg) + 13); // get total sizes
  msg_tmp = (char*) calloc(msg_len, 1); // allocates msg_tmp
  
  snprintf(msg_tmp, msg_len, "PRIVMSG %s :%s\r\n", dst, msg); // format msg_tmp buffer

  // send message
  SSL_write(irc.ssl, msg_tmp, msg_len); // send message

  free(msg_tmp);
}

/* read_bufff - read data from SSL pointer */
char*
read_buff()
{
  char msg_line[MSG_LEN], *msg_recv;
  int msg_nbyts, msg_tnbyts = 0;

  msg_recv = (char*) calloc(MSG_LEN, 1);

  // fetch data from SSL socket
  do {
    memset(msg_line, 0x0, MSG_LEN);
    msg_nbyts = SSL_read(irc.ssl, msg_line, MSG_LEN); // get line bytes
    if(msg_nbyts <= 0)
      break; // break loop if not exist data
    msg_tnbyts += msg_nbyts; // total received bytes
    strncat(msg_recv, msg_line, msg_nbyts);
  } while(SSL_pending(irc.ssl));

  // respose pong message
  if((set_pong(irc.ssl, msg_recv)) == 0)
    return msg_recv;

  // parse data from message
  char *_mData[4] = {
    g_Host(msg_recv, '!', '@'), g_Nick(msg_recv),
    g_Chan(msg_recv), g_Msg(msg_recv)
  };

  // check if all exist in message
  for(int i=0; i < 4; i++) {
    if(_mData[i] != NULL) continue;
    else {
      m_Destroy(_mData, 4);
      return msg_recv;
    }
  } 
  
  Bexec(_mData[0], _mData[2], _mData[3]);
  s_Msg(_mData);

  m_Destroy(_mData, 4);
  return msg_recv;
} 

/* new_con - open && create socket connection */
int
new_con(const char *hostname, int port)
{
  static struct sockaddr_in server; // struct sockaddr_in
  struct hostent *host; // struct hostent
  int sockfd; // socket file descriptor

  // get addresss from hostname
  if((host = gethostbyname(hostname)) == NULL) {
    perror(hostname);
    abort();
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

  // attach SSL context to socket.
  SSL_set_fd(irc.ssl, sockfd);
  if(SSL_connect(irc.ssl) == FAIL)
    ERR_print_errors_fp(stderr);
  else {
    printf(
      "[%s%s*%s] Connected with %s encryption\n",
      fgGreen, blink, resetCl, SSL_get_cipher(irc.ssl)
    );
  } show_Certs(irc.ssl);

  return(sockfd);
}

/* b_header - art of header */
void
b_header() {
  for(int i=0; i < ARRAY_SIZE(irc_h); i++)
    printf("%s %s %s\n", fgGreen, irc_h[i], resetCl);
}