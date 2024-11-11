/* checkNull - checa se o ponteiro é núlo */
int
checkNull(void *ptr) {
  return ptr == NULL ? true : false;
}

/* string - constroi uma string */
char*
string(char *str)
{
  int i = 1;
  while(str[i - 1] != '\0') { i++; }

  char *str_result = (char*) calloc(i, 1);
  strncpy(str_result, str, 1);
  return str_result;
}

/* matrix_Destroy - destroi uma matriz bidmensional */
void
matrix_Destroy(char **m, int mSize)
{
  for(int i=0; i < mSize; i++) {
    if (m[i] != NULL)
      free(m[i]);
  }
}

/* m_haveNull - checa se há algum elemento nulo na matriz */
int 
matrix_haveNull(char **m, int mSize)
{
  for(int i=0; i < mSize; i++) {
    if(m[i] == NULL) {
      return 1;
    }
  }
  return 0;
}

/* Split - divide a mensagem de acordo com o intervalo ch1 ao ch2 */
char*
split(char *str, char ch1, char ch2)
{
  int i, j = 0;
  char *splited = (char*) calloc(1024,1);

  for(i=1; i < strlen(str); i++)
  {
    if(str[i-1] == ch1)
    {
      while(str[i] != ch2)
      {
        splited[j] = str[i];
        i++; j++;
      }
      return splited;
    }
  }

  free(splited);
  return NULL;
}

/* realoca - Realoca o tamanho do ponteiro */
char *realoca(char *str, int size) {
  char *str_result = (char*) reallocarray(str, size, 1);

  if ( str_result == NULL ) {
    printf("Falha na realocação.\n");
    free(str);
    return NULL;
  }

  return (char*) str_result;
}

/* libera o ponteiro */
void release(void *ptr) {
  if ( ptr != NULL ) {
    free(ptr);
  }
}