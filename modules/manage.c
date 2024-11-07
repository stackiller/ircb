/*
  Moderate functions,

  &Bsh
*/

/* manage_Sh - execute shell commands */
void
m_Sh(char *dst, char *msg) {
  char *_bSh = g_Args(msg);
  if(_bSh != NULL) {
    bot_Shell(dst, _bSh);
    release(_bSh);
  }
}