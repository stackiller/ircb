/*

  Defines to ircbot,
  and data for connection.

*/

/* IRC struct data types */
typedef struct {
  int sockfd; // socket file descriptor
  SSL_CTX *ctx; // SSL context pointer
  SSL *ssl; // ssl Descriptor
  char *host, *port; // host and port
  char *nick, *pass; // nick and pass
  char *chans; // channels
  char *buffer; // buffer for messages
} irc_d;

/* attribute irc data type to irc */
irc_d irc;

/* priv messages */ 
void privMsg(char *msg, char *dst);

// get array size
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

// Sizes
#define MSG_LEN 3084

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
