/*

  Defines to ircbot,
  and data for connection.

*/

/* Bot art logo */
char b_Brand[5][90] = {
  ",--.          ,----.   ",
  "`--',--.  ,--.'.-.  |  ",
  ",--. \\  `'  /   .' <  ",
  "|  | /  /.  \\ /'-'  | ",
  "`--''--'  '--'`----'   ",                    
};

/* Bot Adm Nick */
#define BOT_ADM "stackiller"

/* get size of pointer array */ 
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

/* IRC Struct data types */
typedef struct {
  SSL_CTX *ctx; // SSL context pointer
  SSL *ssl; // ssl Descriptor
  int sockfd; // socket file descriptor
  bool isSSL;
  char *host, *port; // host and port
  char *nick, *pass; // nick and pass
  char *chans, *buffer; // channels && buffer
} irc_d;

/* Prototypes of Functions */
SSL_CTX* init_Ctx(void);

int new_Conn(const char*, int); // create socket
int b_Pong(SSL*, char*);
int str_Cmp(char*, char*); // compare two strings.

char* r_Buffer(void);
char* str_Chr(char*, char); // split token, using ch.
char* g_Nick(char*); // get user nick of message.
char* g_Chan(char*); // get channel of message.
char* g_Msg(char*); // get message of buffer.
char* g_Host(char*, char, char); // get host of message.
char* g_nArg(char*, int); // get arguments of message.

void m_Send(char*);
void show_Certs(SSL*);
void b_Nick(char*);
void b_Creds(char*, char*);
void b_Join(char*);
void p_Msg(char*, char*);
void b_Exec(char*, char*, char*); // controller bot commands.
void s_Msg(char**);
void b_Header(void);
void usage(char*);

// Sizes
#define M_LEN 2024
#define B_LEN 30

// Error defines
#define FAIL -1
#define FAIL_SOCK "[!] Failed to create socket. :("
#define FAIL_CONN "[!] Failed to connect. :("

// Foreground Colors
#define tRed "\e[91m"
#define tGreen "\e[92m"
#define tYellow "\e[93m"
#define tBlue "\e[94m"
#define tCyan "\e[96m"

#define tRs "\e[0m"
#define tBlink "\e[5m"