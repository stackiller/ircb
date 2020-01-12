/*
  Moderate Channel,
  functions

  &Bnick, &Bjoin, &Bnick,
*/

/* Bnick - set nick */
void
b_nick(char *dst, char *msg) {
  char *_bNick = g_nArg(msg, 1);
  if(_bNick != NULL) {
    set_nick(_bNick);
    printf("[%s%s@%s] NICK: %s\n", blink, fgYellow, resetCl, _bNick);
    free(_bNick);
  }
}

/* Bjoin - join into channel */
void
b_join(char *dst, char *msg) {
  char *_bJoin = g_nArg(msg, 1);
  if(_bJoin != NULL) {
    set_join(_bJoin);
    printf("[%s%s@%s] JOIN: %s\n", blink, fgCyan, resetCl, _bJoin);
    free(_bJoin);
  }
}

/* Bkick - change nick */
void
b_kick(char *dst, char *msg) {}
