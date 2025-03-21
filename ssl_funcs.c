/* Creates a new socket connection. */
int
new_conn(const char *hostname, int port)
{
  static struct sockaddr_in server; // struct sockaddr_in.
  struct hostent *host; // struct hostent.
  int sockfd; // socket descriptor.

  // get ipv4 address by hostname.
  if((host = gethostbyname(hostname)) == NULL) {
    printf("(%s*%s%s) Invalid address: %s\n", fYellow, fBlink, fRs, hostname);
    return -1;
  }

  // creates a TCP/IP socket.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    free(host);
    perror("new_conn");
    return -1;
  }
 
  // assigns the connection data to the fields.
  server.sin_family = AF_INET; 
  server.sin_port = htons(port);
  server.sin_addr.s_addr = *(long*)(host->h_addr);
  memset(&server.sin_zero, 0x0, 8);

  // connects to the IRC server.
  if((connect(sockfd, (struct sockaddr*) &server, sizeof(server))) != 0) {
    free(host);
    close(sockfd);
    perror("new_conn");
    return -1;
  }

  // attaches the SSL context to the socket.
  SSL_set_fd(irc.ssl, sockfd);

  // initializes the ssl connection to the IRC server.
  if(SSL_connect(irc.ssl) == 0)
  {
    free(host);
    close(sockfd);
    ERR_print_errors_fp(stderr);
    return -1;
  }
  else
  {
    printf(
      "[%s%s*%s] Conectado com %s.\n",
      fGreen, fBlink, fRs, SSL_get_cipher(irc.ssl));
  }
  show_Certs(irc.ssl); // show certificates.
  return sockfd;
}

/* Initialize the SSL engine */
SSL_CTX*
init_Ctx(void)
{
  const SSL_METHOD *method; // SSL method pointer
  SSL_CTX *ctx; // SSL context pointer
 
  // create new method and context
  method = TLS_method(); // create new client-method instance
  ctx = SSL_CTX_new(method); // create new SSL context
  if(ctx == NULL)
  {
    ERR_print_errors_fp(stderr);
    exit(1);
  } return(ctx);  // return SSL context
}

/* Show certificates */
void
show_Certs(SSL *ssl)
{
  // X509 certificate and line pointer
  X509 *cert; // X509 certificate pointer
  char *line; // line buffer

  // get the server's certificate
  cert = SSL_get_peer_certificate(ssl);
  if(cert != NULL)
  {
    printf("Server certificates:\n"); // show message before.
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0); // get subject.

    printf("|_ Subject: %s\n", line); // show subject.
    free(line); // free malloc'ed string

    // get issuer name
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0); // get the issuer.
    printf("|_ Issuer: %s\n", line); // show issuer certificate.
    
    free(line); // free the malloc'ed string
    X509_free(cert); // free the malloc'ed certificate copy
   }
   else
    printf("[?] no certificates.\n");
}

/* Init connection */;
int init_conn(irc_d *irc)
{
  irc->ctx = init_Ctx();
  irc->ssl = SSL_new(irc->ctx);
  
  /* keep trying to create a new connection until successul. */
  while((irc->sockfd = new_conn(irc->host, atoi(irc->port))) < 0) {
    sleep(10);
  }

  for(int i=0; i < 3; i++)
    bot_Nick(irc->nick); // identifies the user.

  return 0;
}

/* End connection */
int
end_conn(irc_d *irc)
{
  SSL_shutdown(irc->ssl);
  SSL_free(irc->ssl);
  close(irc->sockfd);
  SSL_CTX_free(irc->ctx);
  return 0;
}

/* End connection */
int
recon_conn(irc_d *irc)
{
  end_conn(irc);
  init_conn(irc);
  irc->reconnect = 0;
  return 0;
}