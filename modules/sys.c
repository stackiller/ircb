/*
  Moderation functions,
*/

/* Any function used here that is outside
the scope of the file should be listed here using extern */
extern char *string(char*);
extern int checkNull(void*);

/* Gateway to bot_Shell. */
void
sys_Sh(char *dst, char *msg) {
  char *_bSh = get_Args(msg);
  if(_bSh != NULL) {
    bot_Shell(dst, _bSh);
    release(_bSh);
  }
}

/* Executes system commands */
void
bot_Shell(char *dst, char *sh_command) {
  FILE *stream = NULL;

  // execute the given command and store the output.
  if ( (stream = popen(sh_command, "r")) == NULL ) {
    bot_Priv("Falha na execução do comando.", dst);
  }
   
  int ch = 0, size = 0;
  char *out = string("");
 
  while ( (ch = fgetc(stream)) != EOF ) {
    out[size] = ch;
    size++;
    out = realoca(out, size+1);
    if(checkNull(out)) {
      bot_Priv("Comando com retorno nulo.", dst);
    }
  }

  bot_Priv(out, dst); // sends the output of the command.
  memset(out, 0x0, strlen(out));
  free(out);
  pclose(stream);
}