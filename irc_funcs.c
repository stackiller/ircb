/*

  IRC Functions,
  functions required to use the irc protocol.

*/

/* initializes a new connection data structure. */
irc_d irc;

/* Reads the data buffer. */
char*
r_Buffer()
{
  int bytes, tBytes = 0;

  char *buffer = (char*) calloc(BUFFER_SIZE, 1);
  char buffer_lines[BUFFER_LINE_SIZE] = {};

  memset(buffer_lines, 0x0, BUFFER_LINE_SIZE);

  // reads the data and concatenates it into the buffer.
  do {
    memset(buffer_lines, 0x0, BUFFER_LINE_SIZE);
    bytes = SSL_read(irc.ssl, buffer_lines, BUFFER_LINE_SIZE);

    if(bytes <= 0)
      break;
    tBytes += bytes;

    strcat(buffer, buffer_lines);
  } while(SSL_pending(irc.ssl));
  

  // responds to ping.
  if((bot_Pong(buffer)) == 0) {
    return buffer;
  }

  // fragments the message data into an array.
  char *msg_Data[3] = {
    get_Src(buffer),
    get_Dst(buffer),
    get_Msg(buffer)
  };

  // checks if any of the fields is null.
  for(int c=0; c < 3; c++) {
    if (checkNull(msg_Data[c])) {
      matrix_Destroy(msg_Data, 3);
      return buffer;
    }
  }

  bot_Exec(msg_Data[0], msg_Data[1], msg_Data[2]); // executes the bot's commands.
  matrix_Destroy(msg_Data, 3); // release the matrix.

  return buffer;
}

/* Get the sender of the message. */
char*
get_Src(char *msg)
{
  char ch_intervals[] = {'!', '@'};
  int i = 0, j=0;
  char *src_host = (char*) calloc(20,1);
  char *split_msg;

  char ignored_characters[] = {'!', '~', '+', '@', '%'};

  split_msg = strchr(msg, ch_intervals[0]);

  if(checkNull(split_msg)) {
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

/* Get the recipient of the message. */
char*
get_Dst(char *msg)
{
  char *Messages[] = {
    (char*) calloc(strlen(msg), 1), // copy of unformatted message.
    (char*) calloc(strlen(msg), 1) // copy of the formatted message.
  };

  char *Privmsg_flag = get_nArg(msg, 1);

  if (checkNull(Privmsg_flag)) {
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

    if(checkNull(Messages[1])) {
      matrix_Destroy(Messages, 2);
      return NULL;
    }
  }
  release(Messages[0]);
  return Messages[1];
}

/* Get the message. */
char*
get_Msg(char *msg)
{
  int i = 2;

  char *SplitMessage;
  char *Messages[] = {
    (char*) calloc(strlen(msg), 1), // copy of unformatted message.
    (char*) calloc(strlen(msg), 1) // copy of formatted message.
  };

  char *Privmsg_flag = get_nArg(msg, 1);

  if(!str_Cmp(Privmsg_flag, "PRIVMSG")) {
    matrix_Destroy(Messages, 2);
    return NULL;
  }

  else {
    strncpy(Messages[0], msg, strlen(msg));
    
    SplitMessage = strstr(Messages[0], " :");

    if(checkNull(SplitMessage)) {
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

/* Get the n arg. */ 
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

/* Gets all arguments followed after the bot control command. */
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
 
/* Execute bot commands. */
int
bot_Exec(char *src, char *dst, char *msg)
{
  typedef void (*mod_f)(char[], char[]);
  char *_msg = (char*) calloc(strlen(msg), 1);
  char *_cKey; // command key in the message.

  strncpy(_msg, msg, strlen(msg));
  _cKey = get_nArg(_msg, 0);

  // command keys.
  char *_modKeys[] = {
    "bjoin\0", "bnick\0", "bkick\0", "bpart\0", "bsh\0"
  };
 
  // module function selection list.
  mod_f mod_funcs[ARRAY_SIZE(_modKeys)] = {
    &core_Join, &core_Nick, &core_Kick, &core_Part, &sys_Sh
  };

  // compares the userhost, in order to execute only if it is the adm.
  if(!strcmp(src, BOT_ADM))
  {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++)
    {
      if(str_Cmp(_modKeys[i], _cKey))
      {
        if(dst[0] == '#') // checks if the recipient is a channel.
        {
          mod_funcs[i](dst, _msg);
        }
        else // otherwise send the data to the sending user.
        {
          mod_funcs[i](src, _msg);
        }
      }
    }
  }

  free(_msg);
  return 0;
}

/* Set the bot's nickname */
void
bot_Nick(char *nick)
{
  char *datas[] = {
    (char*) calloc(BOT_MAX_LEN, 1), // nick.
    (char*) calloc(BOT_MAX_LEN, 1) // user.
  };

  // formats the buffers.
  snprintf(datas[0], BOT_MAX_LEN, "NICK %s\r\n", nick);
  snprintf(datas[1], BOT_MAX_LEN, "USER %s %s %s %s\r\n", nick, nick, nick, nick);

  msg_Send(datas[0]); // send the nickname.
  msg_Send(datas[1]); // send the identification.

  matrix_Destroy(datas, 2);
}

/* Sets and sends credentials. */
void
bot_Creds(char *nick, char *pass)
{  
  char *creds = (char*) calloc(BOT_MAX_LEN, 1); // allocates the bot's credentials.
  snprintf(creds, BOT_MAX_LEN, "IDENTIFY %s %s\r\n", nick, pass); // formats the buffers.
  bot_Priv(creds, "NickServ");
  free(creds);
}


/* Join the channel. */
void
bot_Join(char *chans)
{
  char *join = (char*) calloc(BOT_MAX_LEN*10, 1);
  snprintf(join, BOT_MAX_LEN, "JOIN %s\r\n", chans);
  msg_Send(join);
  free(join);
}


/* Pong. */
int
bot_Pong(char *msg)
{
  if(
    (strlen(msg) > 60) &&
    ( checkNull(strstr(msg, "PING :")) || checkNull(strstr(msg, ":PING")) )
  ) {
    return 1;
  }

  char *ping;
  char *datas[] = {
    (char*) calloc(strlen(msg), 1), // copy of the original message.
    (char*) calloc(BOT_MAX_LEN, 1) // pong message.
  };

  strncpy(datas[0], msg, strlen(msg));
  ping = strstr(datas[0], " :");
  snprintf(datas[1], BOT_MAX_LEN, "PONG%s", ping);
  printf("\n[%s%s*%s] %s\n", tBlue, tBlink, tRs, datas[1]);
  msg_Send(datas[1]);
  
  matrix_Destroy(datas, 2);
  return 0;
}

/* Privmsg. */
void
bot_Priv(char *mArg, char *mDest) {
  char *pMsg = (char*) calloc(512, 1);
  char *privmsg_models[] = {
    "PRIVMSG %s :%s\r\n",
    "PRIVMSG %s %s\r\n"
  };

  // split the message into lines and send them separately.
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

/* Part of the channel. */
void
bot_Part(char *chans) {
  char *_pMsg = (char*) calloc(BOT_MAX_LEN, 1);
  snprintf(_pMsg, BOT_MAX_LEN, "PART %s :%s\r\n", chans, "afk");
  msg_Send(_pMsg);
  free(_pMsg);
}

/* Sends a message. */ 
void
msg_Send(char *msg) {
  SSL_write(irc.ssl, msg, strlen(msg));
}

/* Bot header art. */
void
bot_Header() {
  printf("%s %s %s\n", tRed, bot_Brand, tRs);
}

/* Helps the user. */
void
usage(char *c_name) {
  bot_Header();
  printf(
    "Use:\n%s <server_addr> <port> <nick> \'<password>\' \'#channel1,#channel2,#channel3..\'\n", c_name);
  exit(0);
}
