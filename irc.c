/* getNick - get nick from message */
char *getNick(char msg[]) {
  char msg_clone[strlen(msg)];
  char *nick = (char*) calloc(512,1); // allocate nick
  char *splitted;
  int i;

  if(msg[0] == ':') {
    msg_clone[strlen(msg)] = '\0';  // add nullbyte
    strncpy(msg_clone, msg, strlen(msg)); // copy msg to clone
    splitted = strtok(msg_clone, "!"); // split the message
    if(splitted!=NULL) {
      for(i=0; i < (strlen(splitted)-1); i++) {
        nick[i] = splitted[i+1]; // copy nick
      } nick[i+1] = '\0'; // add nullbyte
    } else { nick[0] = '\0'; }
  } else { nick[0] = '\0'; }
  return(nick); // return nullbyte;
}

/* getChan - get channel from message */
char *getChan(char msg[]) {
  char msg_clone[strlen(msg)];
  char *channel = (char*) calloc(512,1); // allocates channel
  char *splitted;
  int i = 0; // auxiliary variable

  if(msg[0] == ':') {
    msg_clone[strlen(msg)] = '\0'; // add nullbyte
    strncpy(msg_clone, msg, strlen(msg)); // copy msg to clone
    splitted = strchr(msg_clone, '#'); // split the message
    if(splitted!=NULL) {
      while(splitted[i] != 32) {
        channel[i] = splitted[i]; // copy channel
        i++; // increment index
      } channel[i+1] = '\0'; // add nullbyte
    } else { channel[0] = '\0'; }
  } else { channel[0] = '\0'; }
  return(channel); // return nullbyte
}

/* getArgument - get argument from message */
char *getArgument(char msg[]) {
  char msg_clone[strlen(msg)];
  char *argument = (char*) calloc((strlen(msg)-2),1);
  char *splitted;
  int i;

  if(msg[0] == ':') {
    msg_clone[strlen(msg)] = '\0'; // add nullbyte
    strncpy(msg_clone, msg, strlen(msg)); // copy msg to clone
    splitted = strstr(msg_clone, " :"); // get argument in msg
    if(splitted!=NULL) {
      for(i=2; i < strlen(splitted); i++) {
        argument[i-2] = splitted[i];
      } argument[i-2] = '\0';
    } else { argument[0] = '\0'; }
  } else { argument[0] = '\0'; }
  return(argument); // return nullbyte
}

/* commpareStr - compare Strings */
int compareStr(char *keyword, char *msg) {
  int keyword_len = strlen(keyword);
  if(strncmp(msg, keyword, keyword_len) == 0) {
    return(1);
  } else { return(0); }
}
 
/* getCommand - get command from message */
void getCommand(char msg[], char channel[]) {
  typedef void (*funcs_c)(char[]); // msg[] ? como argumento
  char strip[(strlen(msg))];
  int i;
  
  // keywords for functions
  char *keys[] = {
    "Brules", "Bregister", "Bidentify",
    "Babout", "Bsay",
  };
  
  // functions of keywords
  funcs_c funcs[ARRAY_SIZE(keys)] = {
    &Brules, &Bregister, &Bidentify,
    &Babout, &Bsay,
  }; strncpy(strip, msg, (strlen(msg)));
  strip[(strlen(msg))-1] = '\0'; // add nullbyte
  
  // keywords -> functions
  for(i=0; i < ARRAY_SIZE(keys); i++) {
    if(compareStr(keys[i], strip)) {
      funcs[i](channel);
    }
  }
}

/* setUser - identify user */
void setNick() {
  // user and ident - buffer & len
  char *setNick, *setUser;
  int nickLen, userLen;
  
  // allocates (nick)
  nickLen = (strlen(irc.nick) + 7);
  setNick = calloc(nickLen, sizeof(char));
  // allocates (user)
  userLen = (strlen(irc.nick) * 4 + 10);
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
void readMsg(char *ircMsg) {
  char msg_line[MSG_LEN];
  char msg_tmp[MSG_LEN];
  int bytes;
  int totalBytes = 0;

  // zeroed buffers
  memset(ircMsg, 0x0, MSG_LEN);
  memset(msg_line, 0x0, MSG_LEN);
  memset(msg_tmp, 0x0, MSG_LEN);

  // loop for check exist data
  do {
    memset(msg_line, 0x0, MSG_LEN); // zeroed buffers
    bytes = SSL_read(irc.ssl, msg_line, MSG_LEN); // get current bytes
    if(bytes < 0) {
      break; // break if erros in read data
    } else if (bytes == 0) {
      break; // break if not exist data
    } totalBytes += bytes; // total bytes received
    strncat(msg_tmp, msg_line, bytes); // concatene lines into buffer_tmp
  } while(SSL_pending(irc.ssl));

  // add null byte
  msg_tmp[totalBytes] = '\0';

  char *pingOnMsg = strstr(msg_tmp, "PING "); // ping message
  char *endMessages = strstr(msg_tmp, "End of"); // end motd

  if(pingOnMsg != NULL) { sendPong(msg_tmp); } // call the ping function
  else if(endMessages != NULL) { setJoin(irc.ssl); } // call the join function

  // get data from message
  char *userNick = getNick(msg_tmp);
  char *channel  = getChan(msg_tmp);
  char *message  = getArgument(msg_tmp);

  // check if all exist in message
  if((userNick[0] != '\0' && channel[0] != '\0' && message[0] != '\0')) {
    printf("[%s] %s: %s", userNick, channel, message);
  } getCommand(message, channel);
  strcpy(ircMsg, msg_tmp); // copy message to ircMsg

  // release pointers
  free(userNick);
  free(channel);
  free(message);
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
