/* getNick - get nick from message */
char *getNick(char msg[])
{
  char _clone[strlen(msg)];
  char *token, *nick = (char*) calloc(2024,1);
  int i = 0;

  if(msg[0] == ':') {
    msg[strlen(msg)] = '\0';  // add nullbyte
    strncpy(_clone, msg, strlen(msg)); // copy msg to clone
    token = strtok(_clone, "!"); // split the message
    if(token!=NULL) {
      for(; i < (strlen(token)-1); i++) {
        nick[i] = token[i+1]; // copy nick
      } nick[i+1] = '\0'; // add nullbyte
    } else { nick[0] = '\0'; }
  } else { nick[0] = '\0'; }
  return(nick); // return nullbyte;
}

/* getChan - get channel from message */
char *getChan(char msg[])
{
  char _clone[strlen(msg)];
  char *sub, *channel = (char*) calloc(2024,1);
  int i = 0; // auxiliary variable

  if(msg[0] == ':') {
    msg[strlen(msg)] = '\0'; // add nullbyte
    strncpy(_clone, msg, strlen(msg)); // copy msg to clone
    sub = strchr(_clone, '#'); // split the message
    if(sub!=NULL) {
      while(sub[i] != 32) {
        channel[i] = sub[i]; // copy channel
        i++; // increment index
      } channel[i+1] = '\0'; // add nullbyte
    } else { channel[0] = '\0'; }
  } else { channel[0] = '\0'; }
  return(channel); // return nullbyte
}

/* getArgument - get argument from message */
char *getArgument(char msg[])
{
  char _clone[strlen(msg)];
  char *sub, *rtmsg = (char*) calloc(strlen(msg),1);
  int i = 2;

  memset(_clone, 0x0, strlen(msg));

  if(msg[0] == ':') {
    msg[strlen(msg)] = '\0'; // add nullbyte
    strncpy(_clone, msg, strlen(msg)); // copy msg to clone
    sub = strstr(_clone, " :"); // get argument in msg
    if(sub!=NULL) {
      while(sub[i] != '\r') {
        rtmsg[i-2] = sub[i];
        i++;
      } rtmsg[i-2] = '\0';
    } else { rtmsg[0] = '\0'; }
  } else { rtmsg[0] = '\0'; }
  return(rtmsg); // return nullbyte
}

/* commpareStr - compare Strings */
int compareStr(char *keyword, char *msg) {
  int keyword_len = strlen(keyword);
  if(strncmp(msg, keyword, keyword_len) == 0) {
    return(1);
  } else { return(0); }
}
 
/* exec - get command from message */
void
exec(char msg[], char channel[])
{
  typedef void (*funcs_c)(char[]); // msg[] ? como argumento
  char strip[(strlen(msg))];
  int i;
  
  // keywords for functions
  char *keys[] =
  {
    "Brules", "Bregister", "Bidentify",
    "Babout", "Bsay",
  };
  
  // functions of keywords
  funcs_c funcs[ARRAY_SIZE(keys)] =
  {
    &Brules, &Bregister, &Bidentify,
    &Babout, &Bsay,
  }; strncpy(strip, msg, (strlen(msg)));
    
  // keywords -> functions
  for(i=0; i < ARRAY_SIZE(keys); i++)
  {
    if(compareStr(keys[i], strip))
      funcs[i](channel);
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
void setCreds() {
  char *setCreds; // credentials buffer
  int credsLen; // credentials len
  
  // allocates creds
  credsLen = (strlen(irc.pass) + 28);
  setCreds = calloc(credsLen, sizeof(char));

  // format buffers
  snprintf(setCreds, credsLen, "PRIVMSG NICKSERV identify %s\r\n", irc.pass);
  // replace null byte to new line
  setCreds[credsLen-1] = '\n';

  // send user
  printf("[%s*%s] %s", fgGreen, resetCl, setCreds); // show nick
  SSL_write(irc.ssl, setCreds, credsLen); // send nick
}

/* setJoin - join channel */
void setJoin() {
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

/* sendPong - send Pong response */
int sendPong(char *pingOnMsg) {
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

  printf("[%s*%s] %s", fgBlue, resetCl, pong); // print pong message
  SSL_write(irc.ssl, pong, pong_len); // send pong response
  return(0);
}

/* privMsg - writer on SSL pointer */
void privMsg(char *msg, char *dst) {
  char *msg_tmp; // tmp message
  int msg_len; // size of message

  // allocates tmp
  msg_len = (strlen(irc.chans) + strlen(msg) + 11); // get total sizes
  msg_tmp = calloc(msg_len, sizeof(char)); // allocates msg_tmp
  snprintf(msg_tmp, msg_len, "PRIVMSG %s %s\r\n", dst, msg); // format msg_tmp buffer
  msg_tmp[msg_len-1] = '\n'; // replace '\0' with '\n'

  // send message
  SSL_write(irc.ssl, msg_tmp, msg_len); // send message
}

/* readMsg - read data from SSL pointer */
char
*readBf()
{
  char line[MSG_LEN], *buffer = (char*) malloc(MSG_LEN);
  int bytes, totalBytes = 0;

  /* clear buffers */
  memset(buffer, 0x0, MSG_LEN);
  memset(line, 0x0, MSG_LEN);

  /* fetch data from SSL socket */
  do
  {
    memset(line, 0x0, MSG_LEN); // reset line content
    bytes = SSL_read(irc.ssl, line, MSG_LEN); // get current bytes
    if(bytes <= 0) {
      break; // break if erros in read data
    } totalBytes += bytes; // total bytes received
    strncat(buffer, line, bytes); // concatene lines into buffer_tmp
  } while(SSL_pending(irc.ssl));

  /* indicates the final content string. */
  buffer[totalBytes] = 0x0;

  char *ping = strstr(buffer, "PING "); // ping message
  char *end = strstr(buffer, "End of "); // end motd

  if(ping != NULL)
    sendPong(ping); // call the ping function
  else if(end != NULL)
    setJoin(irc.ssl); // call the join function

  /* get data from message */
  char *userNick = getNick(buffer);
  char *channel  = getChan(buffer);
  char *message  = getArgument(buffer);

  /* check if all exist in message */
  if((userNick[0] != '\0' && channel[0] != '\0' && message[0] != '\0'))
    printf("[%s] %s: %s\n", userNick, channel, message);
  exec(message, channel);

  /* release pointers */
  free(userNick);
  free(channel);
  free(message);

  return buffer; /* return buffer */
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
