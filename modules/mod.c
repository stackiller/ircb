/*
  Moderate Channel,
  functions

  &Bnick, &Bjoin, &Bnick,
*/

/* Bnick - set nick */
void
m_Nick(char *dst, char *msg) {
  char *_bNick = g_nArg(msg, 1);
  if(_bNick != NULL) {
    b_Nick(_bNick);
    printf("[%s%s@%s] NICK: %s\n", tBlink, tYellow, tRs, _bNick);
    free(_bNick);
  }
}

/* Bjoin - join into channel */
void
m_Join(char *dst, char *msg) {
  char *_bJoin = g_nArg(msg, 1);
  if(_bJoin != NULL) {
    b_Join(_bJoin);
    printf("[%s%s@%s] JOIN: %s\n", tBlink, tCyan, tRs, _bJoin);
    free(_bJoin);
  }
}

/* Bkick - change nick */
void
m_Kick(char *dst, char *msg) {}
