/* Checks if the pointer is null. */
int
checkNull(void *ptr) {
  return ptr == NULL ? true : false;
}

/* Compares two strings. */
int
str_Cmp(char *d1, char *d2)
{
  if(strcmp(d2, d1) == 0) {
    return(1);
  }
  else {
    return(0);
  }
}

/* Compare if ch is different from the elements of the chs array */
int
diff(char ch, char *chs) {
  for(int i=0; i < strlen(chs); i++) {
    if(ch != chs[i] && ch != '\0') {
      continue;
    } else {
      return 1;
    }
  }
  return 0;
}

/* Builds a string. */
char*
string(char *str)
{
  int i = 1;
  while(str[i - 1] != '\0') { i++; }

  char *str_result = (char*) calloc(i, 1);
  strncpy(str_result, str, 1);
  return str_result;
}

/* Destroys a two-dimensional array. */
void
matrix_Destroy(char **m, int mSize)
{
  for(int i=0; i < mSize; i++) {
    if (m[i] != NULL)
      free(m[i]);
  }
}

/* Checks if there is any null element in the array. */
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

/* Splits the message according to the range ch1 to ch2. */
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

/* Reallocates the size of the pointer. */
char *realoca(char *str, int size) {
  char *str_result = (char*) reallocarray(str, size, 1);

  if ( str_result == NULL ) {
    printf("Falha na realocação.\n");
    free(str);
    return NULL;
  }

  str_result[size-1] = '\0';
  return (char*) str_result;
}

/* Frees the pointer with null check. */
void release(void *ptr) {
  if ( ptr != NULL ) {
    free(ptr);
  }
}