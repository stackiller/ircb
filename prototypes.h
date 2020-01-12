/*

  Defines to ircbot,
  and data for connection.

*/

/* irc Logo */
char irc_h[5][90] = {
  "▪  ▄▄▄   ▄▄· ▄▄▄▄·       ▄▄▄▄▄\0",
  "██ ▀▄ █·▐█ ▌▪▐█ ▀█▪▪     •██  \0",
  "▐█·▐▀▀▄ ██ ▄▄▐█▀▀█▄ ▄█▀▄  ▐█.▪\0",
  "▐█▌▐█•█▌▐███▌██▄▪▐█▐█▌.▐▌ ▐█▌·\0",
  "▀▀▀.▀  ▀·▀▀▀ ·▀▀▀▀  ▀█▄▀▪ ▀▀▀ \0"
};

/* Bot adm nick */
#define BOT_ADM "stackiller"

/* IRC struct data types */
typedef struct {
  SSL_CTX *ctx; // SSL context pointer
  SSL *ssl; // ssl Descriptor
  int sockfd; // socket file descriptor
  char *host, *port; // host and port
  char *nick, *pass; // nick and pass
  char *chans, *buffer; // channels && buffer
} irc_d;

/* Prototypes of functions */

int new_con(const char*, int); // create socket


int str_cmp(char*, char*); // compare two strings.
char* g_nArg(char*, int); // get arguments of message.
char* str_Chr(char*, char); // split token, using ch.

void Bexec(char*, char*, char*); // controller bot commands.

char* g_Nick(char*); // get user nick of message.
char* g_Chan(char*); // get channel of message.
char* g_Msg(char*); // get message of buffer.
char* g_Host(char*, char, char); // get host of message.

void set_nick(char*);
void set_creds(char*, char*);
void set_join(char*);
void priv_msg(char*, char*);

int set_pong(SSL*, char*);
char *read_buff(void);

void show_msg(char**);

void b_header(void);
void show_Certs(SSL*);
SSL_CTX* init_Ctx(void);

// get array size
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

// Sizes
#define MSG_LEN 2024

// Error defines
#define FAIL -1
#define FAIL_SOCK "[!] Failed to create socket. :("
#define FAIL_CONN "[!] Failed to connect. :("

// IRC Colors
#define ircBlack 0x031
#define ircRed 0x34
#define ircGreen 0x33
#define ircYellow 0x38
#define ircTeal 0x310
#define ircBlue 0x312
#define ircMagenta 0x313
#define ircCyan 0x311
#define ircWhite 0x30

// Foreground Colors
#define fgBlack "\e[30m"
#define fgRed "\e[91m"
#define fgGreen "\e[92m"
#define fgYellow "\e[93m"
#define fgBlue "\e[94m"
#define fgMagenta "\e[95m"
#define fgCyan "\e[96m"
#define fgWhite "\e[97m"

#define resetCl "\e[0m"
#define blink "\e[5m"

// Background Colors
#define bgBlack "\e[49m"
#define bgRed "\e[41m"
#define bgGreen "\e[42m"
#define bgYellow "\e[43m"
#define bgBlue "\e[44m"
#define bgMagenta "\e[45m"
#define bgCyan "\e[46m"
#define bgWhite "\e[47m"