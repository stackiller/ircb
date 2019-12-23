/*

  IRC Funtions,
  funtions necessarys to use irc protocol(server / client).

*/

// attribute irc data type to irc
irc_d irc;

/* str_char - split str from char */
char*
str_chr(char str[], char ch)
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

/* get_nick - get nick */
char*
get_nick(char msg[])
{
  char m_copy[strlen(msg)];
  char *m_token, *m_nick = (char*) calloc(512,1);
  int i = 0;

  strncpy(m_copy, msg, strlen(msg)); // copy message
  m_token = str_chr(m_copy, '!'); // split message

  if(m_token!=NULL) {
    for(; i < (strlen(m_token)-1); i++)
      m_nick[i] = m_token[i+1];
    m_nick[i+1] = '\0';
  }
  else
    return NULL;
  
  return(m_nick);
}

/* get_chan - get channel */
char*
get_chan(char msg[])
{
  char m_copy[strlen(msg)];
  char *m_spl, *m_chan = (char*) calloc(512,1);
  int i = 0;

  strncpy(m_copy, msg, strlen(msg)); // copy message
  m_spl = strstr(m_copy, " #"); // split message

  if(m_spl!=NULL) {
    while(m_spl[i+1] != 32) {
      m_chan[i] = m_spl[i+1];
      i++;
    } m_chan[i+1] = '\0';
  }
  else
    return NULL;

  return(m_chan); // return nullbyte
}

/* get_msg - get message */
char*
get_msg(char msg[])
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

/* comp_str - compare Strings */
int
comp_str(char *key, char *msg)
{
  if(strncmp(msg, key, strlen(key)) == 0)
    return(1);
  else
    return(0);
}
 
/* bot_exec - get command from message */
void
bot_exec(char *msg, char *chan)
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
    if(comp_str(keys[i], m_copy)) {
      funcs[i](chan);
    }
  }
}

/* setUser - identify user */
void
set_nick()
{
  // user and ident - buffer & len
  char *set_nick, *setUser;
  int nickLen, userLen;
  
  // allocates (nick)
  nickLen = (strlen(irc.nick) + 7);
  set_nick = calloc(nickLen, sizeof(char));
  // allocates (user)
  userLen = (strlen(irc.nick) * 14 + 10);
  setUser = calloc(userLen, sizeof(char));

  // format buffers
  snprintf(set_nick, nickLen, "NICK %s\r\n", irc.nick);
  snprintf(setUser, userLen, "USER %s %s %s %s\r\n", irc.nick, irc.nick, irc.nick, irc.nick);
  // replace null byte to new line
  set_nick[nickLen-1] = '\n';
  setUser[userLen-1] = '\n';

  // send user
  printf("[%s%s*%s] %s", fgGreen, blink, resetCl, set_nick); // show nick
  printf("[%s%s*%s] %s", fgGreen, blink, resetCl, setUser); // show ident
  SSL_write(irc.ssl, set_nick, nickLen); // send nick
  SSL_write(irc.ssl, setUser, userLen); // send ident
}

/* set_creds - set credentials of user */
void
set_creds()
{
  char *set_creds; // credentials buffer
  int credsLen; // credentials len
  
  // allocates creds
  credsLen = (strlen(irc.nick) + strlen(irc.pass) + 12);
  set_creds = calloc(credsLen, sizeof(char));

  // format buffers
  snprintf(set_creds, credsLen, "IDENTIFY %s %s\r\n", irc.nick, irc.pass);
  // replace null byte to new line
  set_creds[credsLen-1] = '\n';

  // send user
  printf("[%s%s*%s] %s", fgGreen, blink, resetCl, set_creds); // show nick
  SSL_write(irc.ssl, set_creds, credsLen); // send nick
}

/* set_join - join channel */
void
set_join()
{
  char *set_join; // join buffer
  int join_len; // size of join command

  // alocattes tmp
  join_len = (strlen(irc.chans) + 7);
  set_join = calloc(join_len, sizeof(char)); // alocattes join
  snprintf(set_join, join_len, "JOIN %s\r\n", irc.chans); // format join buffer
  set_join[join_len-1] = '\n';

  // send join command
  printf("[%s*%s] %s", fgCyan, resetCl, set_join); // show join command
  SSL_write(irc.ssl, set_join, join_len); // send join command
}

/* set_pong - send Pong response */
int
set_pong(SSL *ssl, char *msg)
{
  char *ping, *pong;
  char m_copy[strlen(msg)];

  strncpy(m_copy, msg, strlen(m_copy));
  ping = strstr(m_copy, " :");
  pong = (char*) calloc(strlen(ping) + 7, 1);
  snprintf(pong, strlen(ping) + 7, "PONG%s\r\n", ping);

  printf("[%s%s*%s] %s", fgBlue, blink, resetCl, pong);
  SSL_write(ssl, pong, strlen(pong));
  return 0;
}

// send messages
void
priv_msg(char *msg, char *dst)
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
*read_buf(char *irc_buff)
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

  // check for ping or end of motd
  if( msg_recv[0] != ':' && strstr(msg_recv, "PING ") != NULL ) {
    set_pong(irc.ssl, msg_recv);
    return msg_recv;
  }

  /* parse message { nick, chan, msg } */
  char **msg_data = (char**) calloc(3, sizeof(char*));
  msg_data[0] = get_nick(msg_recv);
  msg_data[1] = get_chan(msg_recv);
  msg_data[2] = get_msg(msg_recv);

  /* check if all exist in message */
  if((
    msg_data[0] != NULL && msg_data[1] != NULL && msg_data[2] != NULL
    ))
  {
    printf("[%s] %s: %s\n", msg_data[0], msg_data[1], msg_data[2]);
    bot_exec(msg_data[2], msg_data[1]); // check for bot commands
    for(int i=0; i < 3; i++)
      free(msg_data[i]);
  }
  
  return msg_recv;
}

/* newCon - open && create socket connection */
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
  } return(sockfd); // return socket
}

/* irc_header - show intro */
void
irc_header()
{
  for(int i=0; i < ARRAY_SIZE(irc_h); i++)
    printf("%s %s %s\n", fgGreen, irc_h[i], resetCl);
}