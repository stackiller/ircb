/*
    Core functions.
*/

/* Bot header art. */
void
bot_Header() {
  printf("%s %s %s\n", tGreen, bot_Brand, tRs);
}

/* Helps the user. */
void
usage(char *c_name) {
  bot_Header();
  printf("Use:\n%s <server_addr> <port> <nick> \'<password>\' \'#channel1,#channel2,#channel3..\'\n", c_name);
}

/* Checks if the pointer is null. */
int checkNull(void *ptr) { return ptr == NULL ? true : false; }

/* Compares two strings. */
int
str_Cmp(char *d1, char *d2)
{
  if(strcmp(d2, d1) == 0) return(1);
  else return(0);
}

/* Compare if ch is different from the elements of the chs array. */
int
diff_from_chs(char ch, char *chs)
{
  for(int i=0; i < strlen(chs); i++) {
    if(ch == chs[i] || ch == '\0') {
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

  if (str_result == NULL) {
    printf("Fail to create string: %s\n", str);
    return NULL;
  }

  strncpy(str_result, str, i);
  return str_result;
}

/* Reallocates the size of the pointer. */
char *string_realloc(char *str, int size) {
  char *str_result = (char*) reallocarray(str, size, 1);

  if ( str_result == NULL ) {
    printf("Fail to reallocate string: %s\n", str);
    free(str);
    return NULL;
  }

  str_result[size-1] = '\0';
  return str_result;
}

/* Allocates matrix buffer. */
char**
matrix_Alloc(int size)
{
  char **matrix = (char**) calloc(size, sizeof(char*));

  if ( matrix == NULL ) {
    printf("Fail to allocate the matrix.\n");
    return NULL;
  }

  for(int i=0; i < size; i++)
    matrix[i] = (char*) calloc(BUFFER_MAX_LINE_SIZE, 1);

  return matrix;
}

/* Reallocates the matrix size. */
char**
matrix_Realloc(char **matrix, int size)
{
  char **new_matrix = (char**) reallocarray(matrix, size, sizeof(char*));

  if ( new_matrix == NULL ) {
    printf("Fail to reallocate the matrix size.\n");
    matrix_Destroy(matrix, size);
    return NULL;
  }

  new_matrix[size-1] = (char*) calloc(BUFFER_MAX_LINE_SIZE, 1);

  return new_matrix;
}

/* Matrix destroyer. */
void
matrix_Destroy(char **matrix, int size)
{
  for(int i=0; i < size; i++)
    free(matrix[i]);
}

/* Reads the matrix buffer. */
void
read_matrix_Buffer(char **matrix, int size)
{
  for(int index=0; index < size; index++)
    puts(matrix[index]);
}

/* Checks if there is any null element in the matrix. */
int
matrix_haveNull(char **matrix, int size)
{
  for(int i=0; i < size; i++) {
    if(matrix[i] == NULL) {
      return 1;
    }
  }
  return 0;
}

/* Frees the pointer with null check. */
void null_safe_release(void *ptr)
{
  if ( ptr != NULL ) {
    free(ptr);
  }
}