/*

  IRC Functions,
  required functions to use the irc protocol.

*/

/* initializes a new connection data structure. */
irc_d irc;

/* Reads the data buffer. */
char**
r_Buffer(int *buffer_matrix_size)
{
  if(irc.reconnect) {
    *buffer_matrix_size = 0;
    return NULL;
  }

  int bytes, tBytes = 0;

  char **buffer_matrix;
  char *buffer = (char*) calloc(BUFFER_MAX_SIZE, 1);
  char *buffer_lines = (char*) calloc(BUFFER_MAX_LINE_SIZE, 1);

  // reads the data and concatenates it into the buffer.
  do {
    bytes = SSL_read(irc.ssl, buffer_lines, BUFFER_MAX_LINE_SIZE);

    if(bytes <= 0) {
      matrix_Destroy((char*[]){buffer, buffer_lines}, 2);
      return NULL;
    }
    tBytes += bytes;

    strcat(buffer, buffer_lines);
    memset(buffer_lines, '\0', BUFFER_MAX_LINE_SIZE);
  } while(SSL_pending(irc.ssl));

  free(buffer_lines);

  *buffer_matrix_size = tBytes;
  buffer_matrix = format_Buffer(buffer, buffer_matrix_size);

  if(checkNull(buffer_matrix)) {
    *buffer_matrix_size = 0;
    free(buffer);
    return NULL;
  }

  int index = 0;
  do
  {
    // responds to ping
    if((bot_Pong(buffer_matrix[index])) == 0) {
      return buffer_matrix;
    }

    char *lbuffer_line_command = NULL; // command code in message.
    
    // check if any error occurs in connection and reset.
    if(buffer_matrix[index][0] != ':')
    {
      lbuffer_line_command = get_nArg(buffer_matrix[index], 0);
      if(str_Cmp("ERROR", lbuffer_line_command))
      {
        free(lbuffer_line_command);
        irc.reconnect = 1;
        break;
      }
      null_safe_release(lbuffer_line_command);
    }

    int lbuffer_line_code = get_Code(buffer_matrix[index]);

    // handles messages codes
    if(lbuffer_line_code) {
      switch (lbuffer_line_code) {
        case 376: // end of motd
          bot_Creds(irc.nick, irc.pass);
          bot_Join(irc.chans);
          break;
        case 433: // nick already in use
          printf("(%s%s!%s) NICK: %s already in use.\n", fYellow, fBlink, fRs, irc.nick);
          bot_Nick(irc.nick);
          sleep(30);
      }
    }
    
    // fragments the message data into fields.
    char *msg_Data[3] = {
      get_Src(buffer_matrix[index]), // src
      get_Dst(buffer_matrix[index]), // dst
      get_Msg(buffer_matrix[index]) // msg
    };

    // checks if any of the fields is null, and execute bot commands.
    if(!matrix_haveNull(msg_Data, 3)) {
      bot_Exec(msg_Data[0], msg_Data[1], msg_Data[2]);
    }

    for(int x=0; x < 3; x++)
      null_safe_release(msg_Data[x]);

    index++;
  } while(index < *buffer_matrix_size);

  free(buffer);
  return buffer_matrix;
}

/* Format the buffer. */
char**
format_Buffer(char *buffer, int *ret_size)
{
  int i = 0;
  char **f_buffer = matrix_Alloc(1);

  if(f_buffer == NULL) {
    *ret_size = 0;
    return NULL;
  }

  char *buffer_copy = (char*) calloc(strlen(buffer) + 1, 1);
  strcpy(buffer_copy, buffer);

  char *token = strtok(buffer_copy, "\r\n");
  while(token != NULL)
  {
    strcpy(f_buffer[i], token);
    i++;
    f_buffer = matrix_Realloc(f_buffer, i+1);

    if(checkNull(f_buffer)) {
      *ret_size = 0;
      free(buffer_copy);
      return NULL;
    }
    token = strtok(NULL, "\r\n");
  }
  
  *ret_size = i;
  free(buffer_copy);
  return f_buffer;
}

/* Get the sender of the message. */
char*
get_Src(char *lbuffer)
{
  int i = 1, j=0;

  char *src;
  char *lbuffer_copy;
  char limit = '!';

  char ign_chs[] = {':', '!', '~', '+', '@', '%'};

  src = (char*) calloc(30,1);
  lbuffer_copy = (char*) calloc(strlen(lbuffer) + 1, 1);
  
  strcpy(lbuffer_copy, lbuffer);

  for(int c=0; c < sizeof(ign_chs); c++) {
    if(lbuffer_copy[i] == ign_chs[c]) {
      i++;
    }
  }

  do {
    src[j] = lbuffer_copy[i];
    i++; j++;
  } while(lbuffer_copy[i] != limit && i < 30);

  if(i == 20) {
    matrix_Destroy((char*[]){ src, lbuffer_copy}, 2);
    return NULL;
  }

  return src;
}

/* Get the recipient of the message. */
char*
get_Dst(char *lbuffer)
{
  int privmsg_bool = 0;
  
  char *lbuffer_copy = (char*) calloc(strlen(lbuffer) + 1, 1);
  char *dst = (char*) calloc(50, 1);

  char *Privmsg_flag = get_nArg(lbuffer, 1);
  privmsg_bool = str_Cmp(Privmsg_flag, "PRIVMSG");
  null_safe_release(Privmsg_flag);

  if(!privmsg_bool) {
    matrix_Destroy((char*[]){ lbuffer_copy, dst }, 2);
    return NULL;
  }

  else {
    strcpy(lbuffer_copy, lbuffer);
    
    dst = get_nArg(lbuffer_copy, 2);

    if(checkNull(dst)) {
      matrix_Destroy((char*[]){ lbuffer_copy, dst }, 2);
      return NULL;
    }
  }

  free(lbuffer_copy);
  return dst;
}

/* Get the message. */
char*
get_Msg(char *lbuffer)
{
  int i = 2;
  int privmsg_bool = 0;

  char *lbuffer_copy = (char*) calloc(strlen(lbuffer) + 1, 1);
  char *msg = (char*) calloc(BUFFER_MAX_LINE_SIZE, 1);

  strcpy(lbuffer_copy, lbuffer);

  char *Privmsg_flag = get_nArg(lbuffer_copy, 1);

  privmsg_bool = str_Cmp(Privmsg_flag, "PRIVMSG");
  null_safe_release(Privmsg_flag);

  if(!privmsg_bool) {
    matrix_Destroy((char*[]){ lbuffer_copy, msg }, 2);
    return NULL;
  }
  
  else {
    char *split_message = strstr(lbuffer_copy, " :");
    
    if(checkNull(split_message)) {
      matrix_Destroy((char*[]){ lbuffer_copy, msg }, 2);
      return NULL;
    }

    while(split_message[i] != '\0') {
      msg[i-2] = split_message[i]; i++;
    }
  }

  free(lbuffer_copy);
  return msg;
}

/* Get the n arg of a message. */ 
char*
get_nArg(char *lbuffer, int n_arg_index)
{
  int i = 0, j = 0, k = 0;
  char *n_arg = string("");

  char *lbuffer_copy = (char*) calloc(strlen(lbuffer) + 1, 1);
  strcpy(lbuffer_copy, lbuffer);

  for(; i<strlen(lbuffer_copy); i++)
  {
    if(k == n_arg_index)
    {
      while(!diff_from_chs(lbuffer_copy[i], (char[]) {'\r', '\n', ' '}))
      {
        n_arg[j] = lbuffer_copy[i];
        i++; j++;
        n_arg = string_realloc(n_arg, j+1);
        if(checkNull(n_arg)) {
          free(lbuffer_copy);
          return NULL;
        }
      }
      return n_arg;
    }
    else if(lbuffer_copy[i] == 32) {
      k++;
    }
  }

  matrix_Destroy((char*[]) { lbuffer_copy, n_arg }, 2);
  return NULL;
}

/* Gets all arguments followed after the bot control command. */
char*
get_Args(char *lbuffer)
{
  int i = 0, j = 0, k = 0;
  char *args = string("");
  char *lbuffer_copy = (char*) calloc(strlen(lbuffer) + 1, 1);

  strcpy(lbuffer_copy, lbuffer);

  for(; i<strlen(lbuffer_copy); i++)
  {
    if(k == 1)
    {
      while((lbuffer_copy[i] != '\0'))
      {
        args[j] = lbuffer_copy[i];
        i++; j++;
        args = string_realloc(args, j+1);
        if(checkNull(args)) {
          free(lbuffer_copy);
          return NULL;
        }
      }
      return args;
    }
    else if(lbuffer_copy[i] == 32) k++;
  }

  matrix_Destroy((char*[]) { args, lbuffer_copy }, 2);
  return NULL;
}

/* Get code message. */
int
get_Code(char *lbuffer) {
  int rcode;
  char *lbuffer_copy = (char*) calloc(strlen(lbuffer) + 1, 1);
  strcpy(lbuffer_copy, lbuffer);

  char *code = get_nArg(lbuffer_copy, 1);
  if(checkNull(code)) {
    free(lbuffer_copy);
    return 0;
  }

  rcode = atoi(code);
  
  matrix_Destroy((char*[]) { code, lbuffer_copy }, 2);
  return rcode;
}
 
/* Execute bot commands. */
int
bot_Exec(char *src, char *dst, char *msg)
{
  typedef void (*mod_f)(char[], char[]);
  char *msg_copy = (char*) calloc(strlen(msg) + 1, 1);
  char *_cKey; // command key in the message.

  strcpy(msg_copy, msg);
  _cKey = get_nArg(msg_copy, 0);

  // command keys.
  char *_modKeys[] = {
    ".reconnect", ".join", ".nick", ".kick", ".part", ".identify", ".sh",
  };
 
  // module function selection list.
  mod_f mod_funcs[ARRAY_SIZE(_modKeys)] = {
    &core_Reconnect, &core_Join, &core_Nick, &core_Kick, &core_Part, &core_Identify, &sys_Sh
  };

  // compares the userhost, in order to execute only if it is the adm.
  if(str_Cmp(src, BOT_ADM))
  {
    for(int i=0; i<ARRAY_SIZE(_modKeys); i++)
    {
      if(str_Cmp(_modKeys[i], _cKey))
      {
        if(dst[0] == '#') // checks if the recipient is a channel.
        {
          mod_funcs[i](dst, msg_copy);
        }
        else // otherwise send the data to the sending user.
        {
          mod_funcs[i](src, msg_copy);
        }
      }
    }
  }

  matrix_Destroy((char*[]){ msg_copy, _cKey }, 2);
  return 0;
}

/* Set the bot's nickname */
void
bot_Nick(char *nick)
{
  char *datas[] = {
    (char*) calloc(BOT_MAX_LEN, 1),
    (char*) calloc(BOT_MAX_LEN, 1)
  };

  // formats the buffers.
  snprintf(datas[0], BOT_MAX_LEN, "NICK %s\r\n", nick);
  snprintf(datas[1], BOT_MAX_LEN, "USER %s %s %s %s\r\n", nick, nick, nick, nick);

  msg_Send(datas[0]);
  
  if(!user_flag) {
    msg_Send(datas[1]);
    user_flag = 1;
  }

  matrix_Destroy(datas, 2);
}

/* Sets and sends credentials. */
void
bot_Creds(char *nick, char *pass)
{  
  char *creds = (char*) calloc(BOT_MAX_LEN*2, 1);
  snprintf(creds, BOT_MAX_LEN*2, "IDENTIFY %s %s", nick, pass);
  bot_Priv(creds, "NickServ");
  free(creds);
}


/* Join the channel. */
void
bot_Join(char *chans)
{
  char *join = (char*) calloc(BOT_MAX_LEN*2, 1);
  snprintf(join, BOT_MAX_LEN*2, "JOIN %s\r\n", chans);
  msg_Send(join);
  free(join);
}

/* Pong. */
int
bot_Pong(char *msg)
{
  if((strncmp("PING :", msg, 6)) != 0) {
    return 1;
  }

  char *ping;
  char *msg_copy = (char*) calloc(strlen(msg) + 1, 1);
  char *pong = (char*) calloc(BOT_MAX_LEN, 1);

  strcpy(msg_copy, msg);
  ping = strstr(msg_copy, " :");
  snprintf(pong, BOT_MAX_LEN, "PONG%s\r\n", ping);
  printf("\n[%s%s*%s] %s\n", fBlue, fBlink, fRs, pong);
  msg_Send(pong);

  irc.pong = 1; // set pong flag.

  matrix_Destroy((char*[]) { msg_copy, pong }, 2);
  return 0;
}

/* Privmsg. */
void
bot_Priv(char *mArg, char *mDest) {
  char *privmsg = (char*) calloc(BUFFER_MAX_LINE_SIZE, 1);
  char *privmsg_models[] = {
    "PRIVMSG %s :%s\r\n",
    "PRIVMSG %s %s\r\n"
  };

  // split the message into lines and send them separately.
  char *line = strtok(mArg, "\n");

  while (line != NULL) {
    mDest[0] == '#' ?
      snprintf(privmsg, BUFFER_MAX_LINE_SIZE, privmsg_models[1], mDest, line) :
      snprintf(privmsg, BUFFER_MAX_LINE_SIZE, privmsg_models[0], mDest, line);
    
    msg_Send(privmsg);
    memset(privmsg, '\0', BUFFER_MAX_LINE_SIZE);
    line = strtok(NULL, "\n");
  }

  free(privmsg);
}

/* Part of the channel. */
void
bot_Part(char *chans) {
  char *privmsg = (char*) calloc(BOT_MAX_LEN*2, 1);
  snprintf(privmsg, BOT_MAX_LEN*2, "PART %s :%s\r\n", chans, PART_MESSAGE);
  msg_Send(privmsg);
  free(privmsg);
}

/* Quit from server */
void
bot_Quit(void) {
  char *privmsg = (char*) calloc(BOT_MAX_LEN, 1);
  snprintf(privmsg, BOT_MAX_LEN, "QUIT :%s\r\n", QUIT_MESSAGE);
  msg_Send(privmsg);
  free(privmsg);
}

/* Sends a message. */ 
void
msg_Send(char *msg) {
  SSL_write(irc.ssl, msg, strlen(msg));
}

/* Check connection timeout. */
void*
check_Timeout(void *unsed)
{
  do {
    sleep(60 * 5);
    if(irc.pong == 0) {
      irc.reconnect = 1;
    } else {
      irc.pong = 0;
    }
  } while(true);
  return (void*)0;
}