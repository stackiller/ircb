/* Creates a new socket connection. */
int
new_conn(const char *hostname, const char *port)
{
  int sockfd, s;
  struct addrinfo *res, *next, hints;
  res = addri;
  next = addri_next;

  // hints for addrinfo.
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags |= AI_NUMERICSERV | AI_ADDRCONFIG;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_addrlen = 0;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  // get address of hostname.
  s = getaddrinfo(hostname, port, &hints, &res);
  if(s < 0) {
    return -1;
  }

  // create ipv4 socket.
  for(next = res; next != NULL; next = next->ai_next) {
    sockfd = socket(next->ai_family, next->ai_socktype, next->ai_protocol);
    if(sockfd > 0) {
      break;
    }
  }
  freeaddrinfo(res);

  // exit if socket fails.
  if(sockfd < 0) {
    perror("new_conn");
    return -1;
  }

  // connects to the IRC server.
  if((connect(sockfd, next->ai_addr, next->ai_addrlen)) != 0) {
    printf("(%s*%s%s) Invalid address: %s\n", fYellow, fBlink, fRs, hostname);
    close(sockfd);
    return -1;
  }

  // attaches the SSL context to the socket.
  SSL_set_fd(irc.ssl, sockfd);

  // initializes the ssl connection to the IRC server.
  if(SSL_connect(irc.ssl) == 0)
  {
    close(sockfd);
    ERR_print_errors_fp(stderr);
    return -1;
  }
  else
  {
    printf(
      "[%s%s*%s] Connected with %s.\n",
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
    printf("Server certificate:\n"); // show message before.
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
    printf("[%s%s?%s] No certificates.\n", fYellow, fBlink, fRs);
}

/* Init connection */;
int init_conn(irc_d *irc)
{
  irc->ctx = init_Ctx();
  irc->ssl = SSL_new(irc->ctx);
  
  /* keep trying to create a new connection until successul. */
  while((irc->sockfd = new_conn(irc->host, irc->port)) < 0) {
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
  global_variables_reset();
  SSL_shutdown(irc->ssl);
  SSL_free(irc->ssl);
  SSL_CTX_free(irc->ctx);
  close(irc->sockfd);
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