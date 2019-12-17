/*

  IRC Funtions,
  funtions necessarys to use irc protocol(server / client).

*/

// attribute irc data type to irc
irc_d irc;

// split str from char
char*
strchar(char str[], char ch)
{
  char *str_spl = (char*) calloc(strlen(str),1);
  int i=0;

  while(str[i] != ch) {
    if(i >= strlen(str))
      return NULL;
    str_spl[i] = str[i];
    i++;
  } return (char*)str_spl;
}

// get nick
char*
getNick(char msg[])
{
  char m_copy[strlen(msg)];
  char *m_token, *m_nick = (char*) calloc(512,1);
  int i = 0;

  strncpy(m_copy, msg, strlen(msg)); // copy message
  m_token = strchar(m_copy, '!'); // split message

  if(m_token!=NULL) {
    for(; i < (strlen(m_token)-1); i++)
      m_nick[i] = m_token[i+1];
    m_nick[i+1] = '\0';
  }
  else
    return NULL;
  
  return(m_nick);
}

// get channel
char*
getChan(char msg[])
{
  char m_copy[strlen(msg)];
  char *m_spl, *m_chan = (char*) calloc(512,1);
  int i = 0;

  strncpy(m_copy, msg, strlen(msg)); // copy message
  m_spl = strchr(m_copy, '#'); // split message

  if(m_spl!=NULL) {
    while(m_spl[i] != 32) {
      m_chan[i] = m_spl[i];
      i++;
    } m_chan[i+1] = '\0';
  }
  else
    return NULL;

  return(m_chan); // return nullbyte
}

// get message
char*
getMsg(char msg[])
{
  char m_copy[strlen(msg)];
  char *m_spl, *m_recv = (char*) calloc(2024,1);
  int i = 0;

  memset(m_copy, 0x0, strlen(msg));
  strncpy(m_copy, msg, strlen(msg));
  m_spl = strstr(m_copy, " :");

  if(m_spl!=NULL) {
    while(m_spl[i+2] != '\r') {
      m_recv[i] = m_spl[i+2];
      i++;
    } m_recv[i+1] = '\0';
  } else { return NULL; }
  return(m_recv);
}

/* commpareStr - compare Strings */
int
compareStr(char *key, char *msg)
{
  if(strncmp(msg, key, strlen(key)) == 0)
    return(1);
  else
    return(0);
}
 
/* exec - get command from message */
void
exec(char *msg, char *chan)
{
  typedef void (*funcs_c)(char[]);
  char m_copy[(strlen(msg))];
  int i;
  
  // keywords for functions
  char *keys[] = {
    "brules", "bregister", "bidentify",
    "babout", "bsay", "bhelp"
  };
  
  // functions of keywords
  funcs_c funcs[ARRAY_SIZE(keys)] = {
    &Brules, &Bregister, &Bidentify,
    &Babout, &Bsay, &Bhelp
  }; strncpy(m_copy, msg, (strlen(msg)));
    
  // keywords -> functions
  for(i=0; i < ARRAY_SIZE(keys); i++) {
    if(compareStr(keys[i], m_copy)) {
      funcs[i](chan);
    }
  }
}

/* setUser - identify user */
void
setNick()
{
  // user and ident - buffer & len
  char *setNick, *setUser;
  int nickLen, userLen;
  
  // allocates (nick)
  nickLen = (strlen(irc.nick) + 7);
  setNick = calloc(nickLen, sizeof(char));
  // allocates (user)
  userLen = (strlen(irc.nick) * 14 + 10);
  setUser = calloc(userLen, sizeof(char));

  // format buffers
  snprintf(setNick, nickLen, "NICK %s\r\n", irc.nick);
  snprintf(setUser, userLen, "USER %s %s %s %s\r\n", irc.nick, irc.nick, irc.nick, irc.nick);
  // replace null byte to new line
  setNick[nickLen-1] = '\n';
  setUser[userLen-1] = '\n';

  // send user
  printf("[%s*%s] %s", fgGreen, resetCl, setNick); // show nick
  printf("[%s*%s] %s", fgGreen, resetCl, setUser); // show ident
  SSL_write(irc.ssl, setNick, nickLen); // send nick
  SSL_write(irc.ssl, setUser, userLen); // send ident
}

/* setCreds - set credentials of user */
void
setCreds()
{
  char *setCreds; // credentials buffer
  int credsLen; // credentials len
  
  // allocates creds
  credsLen = (strlen(irc.nick) + strlen(irc.pass) + 12);
  setCreds = calloc(credsLen, sizeof(char));

  // format buffers
  snprintf(setCreds, credsLen, "IDENTIFY %s %s\r\n", irc.nick, irc.pass);
  // replace null byte to new line
  setCreds[credsLen-1] = '\n';

  // send user
  printf("[%s*%s] %s", fgGreen, resetCl, setCreds); // show nick
  SSL_write(irc.ssl, setCreds, credsLen); // send nick
}

/* setJoin - join channel */
void
setJoin()
{
  char *setJoin; // join buffer
  int join_len; // size of join command

  // alocattes tmp
  join_len = (strlen(irc.chans) + 7);
  setJoin = calloc(join_len, sizeof(char)); // alocattes join
  snprintf(setJoin, join_len, "JOIN %s\r\n", irc.chans); // format join buffer
  setJoin[join_len-1] = '\n';

  // send join command
  printf("[%s*%s] %s", fgCyan, resetCl, setJoin); // show join command
  SSL_write(irc.ssl, setJoin, join_len); // send join command
}

// sendPong - send Pong response
int
setPong(SSL *ssl, char *pingOnMsg) {
  int index = 5; // index of pong array
  int pong_len = 0; // pong lenght
  char *pong; // pong response message

  // calculate size of ping message
  while(pingOnMsg[index] != '\n' && pingOnMsg[index] != 32) {
    pong_len++; // increment size of pong_len
    index++; // increment index
  }

  pong_len += 7; // num pong positions
  pong = calloc(pong_len, sizeof(char)); // alocates pong
  strcpy(pong, "PONG "); // copy PONG to pong array
  
  // copy ping to pong
  index = 5;
  while(pingOnMsg[index] != '\n') {
    pong[index] = pingOnMsg[index]; // copy begin to pong
    index++;
  } // format array
  pong[pong_len-2] = '\r';
  pong[pong_len-1] = '\n';

  printf("[%s%s*%s] %s", fgBlue, blink, resetCl, pong); // print pong message
  SSL_write(irc.ssl, pong, pong_len); // send pong response
  return(0);
}

// send messages
void
privMsg(char *msg, char *dst)
{
  char *msg_tmp; // tmp message
  int msg_len; // size of message

  // allocates tmp
  msg_len = (strlen(dst) + strlen(msg) + 12); // get total sizes
  msg_tmp = (char*) calloc(msg_len, sizeof(char)); // allocates msg_tmp
  snprintf(msg_tmp, msg_len, "PRIVMSG %s :%s", dst, msg); // format msg_tmp buffer
  msg_tmp[msg_len-1] = '\n';

  // send message
  SSL_write(irc.ssl, msg_tmp, msg_len); // send message
}

/* readMsg - read data from SSL pointer */
char
*readBuf(char *irc_buff)
{
  char msg_line[MSG_LEN], *msg_recv;
  int msg_nbyts, msg_tnbyts = 0;

  msg_recv = (char*) calloc(MSG_LEN, 1);

  /* fetch data from SSL socket */
  do {
    memset(msg_line, 0x0, MSG_LEN);
    msg_nbyts = SSL_read(irc.ssl, msg_line, MSG_LEN); // get line bytes
    if(msg_nbyts <= 0)
      break; // break if not exist more data
    msg_tnbyts += msg_nbyts; // total received bytes
    strncat(msg_recv, msg_line, msg_nbyts); // concatene lines
  } while(SSL_pending(irc.ssl));

  // check ping message
  char *ping_on = strstr(msg_recv, "PING ");
  char *end_on = strstr(msg_recv, "End");

  // check for ping or end of motd
  if((msg_recv[0] != ':') && (ping_on != NULL))
    setPong(irc.ssl, msg_recv);
  else if((end_on != NULL))
    setJoin(irc.ssl);

  /* fetch data { nick, chan, msg } */
  char *nick = getNick(msg_recv);
  char *chan = getChan(msg_recv);
  char *msg  = getMsg(msg_recv);

  /* check if all exist in message */
  if((nick != NULL && chan != NULL && msg != NULL)) {
    printf("[%s] %s: %s\n", nick, chan, msg);
    exec(msg, chan);
    free(nick);
    free(chan);
    free(msg);
  }
  
  return msg_recv;
}

/* newCon - open && create socket connection */
int newCon(const char *hostname, int port) {
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
  } return(sockfd); // return socket
}