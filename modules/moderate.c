/*
  Moderate Channel,
  functions

  &Bnick, &Bjoin, &Bnick,
*/

/* m_Nick - set nick */
void
m_Nick(char *dst, char *msg) {
  char *_bNick = get_nArg(msg, 1);
  if(_bNick != NULL) {
    b_Nick(_bNick);
    printf("[%s%s@%s] NICK: %s\n", tBlink, tYellow, tRs, _bNick);
    free(_bNick);
  }
}

/* m_Join - join into channel */
void
m_Join(char *dst, char *msg) {
  char *_bJoin = get_nArg(msg, 1);
  if(_bJoin != NULL) {
    b_Join(_bJoin);
    printf("[%s%s@%s] JOIN: %s\n", tBlink, tCyan, tRs, _bJoin);
    free(_bJoin);
  }
}

/* m_Kick - kick nickname */
void
m_Kick(char *dst, char *msg) {
  char *_datas[] = {
    get_nArg(msg, 1),
    get_nArg(msg, 2)
  };

  if(matrix_haveNull(_datas, 2))
    return;

  char *s_msg = (char*) calloc(B_LEN, 1);

  snprintf(s_msg, B_LEN, "KICK %s %s\r\n", _datas[0], _datas[1]); // format join buffer

  printf("KICK %s %s\n", _datas[0], _datas[1]);

  m_Send(s_msg); // send join command

  free(s_msg);
  matrix_Destroy(_datas, 2);
}

/* m_Part - part of channel */
void
m_Part(char *dst, char *msg) {
  char *_bPart = get_nArg(msg, 1);
  if(_bPart != NULL) {
    bot_Part(_bPart);
    printf("[%s%s@%s] PART: %s\n", tBlink, tCyan, tRs, _bPart);
    free(_bPart);
  }
}