/*
  Core functions of bot,
*/

/* Sets a new nick. */
void
core_Nick(char *dst, char *msg) {
  char *_bNick = get_nArg(msg, 1);
  if(_bNick != NULL) {
    bot_Nick(_bNick);
    printf("[%s%s@%s] NICK: %s\n", tBlink, tYellow, tRs, _bNick);
    free(_bNick);
  }
}

/* Join to a channel. */
void
core_Join(char *dst, char *msg) {
  char *_bJoin = get_nArg(msg, 1);
  if(_bJoin != NULL) {
    bot_Join(_bJoin);
    printf("[%s%s@%s] JOIN: %s\n", tBlink, tCyan, tRs, _bJoin);
    free(_bJoin);
  }
}

/* Kicks a nickname. */
void
core_Kick(char *dst, char *msg) {
  char *_datas[] = {
    get_nArg(msg, 1),
    get_nArg(msg, 2)
  };

  if(matrix_haveNull(_datas, 2))
    return;

  char *s_msg = (char*) calloc(BOT_MAX_LEN, 1);
  snprintf(s_msg, BOT_MAX_LEN, "KICK %s %s\r\n", _datas[0], _datas[1]);
  printf("KICK %s %s\n", _datas[0], _datas[1]);
  msg_Send(s_msg);

  free(s_msg);
  matrix_Destroy(_datas, 2);
}

/* Part of a channel. */
void
core_Part(char *dst, char *msg) {
  char *_bPart = get_nArg(msg, 1);
  if(_bPart != NULL) {
    bot_Part(_bPart);
    printf("[%s%s@%s] PART: %s\n", tBlink, tCyan, tRs, _bPart);
    free(_bPart);
  }
}