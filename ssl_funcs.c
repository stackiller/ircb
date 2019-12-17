/* initialize the SSL engine */
SSL_CTX*
initCtx(void)
{
  // SSL metho and context
  const SSL_METHOD *method; // SSL method pointer
  SSL_CTX *ctx; // SSL context pointer
 
  // Load OpenSSL chipers and et.all - erros messages
  OpenSSL_add_all_algorithms(); // load cryptos, and et.all
  SSL_load_error_strings(); // bring and register error messages

  // create new method and context
  method = SSLv23_client_method(); // create new client-method instance
  ctx = SSL_CTX_new(method); // create new SSL context
  if(ctx == NULL)
  {
    ERR_print_errors_fp(stderr);
    abort();
  } return(ctx);  // return SSL context
}

/* show certificates function */
void
showCerts()
{
  // X509 certificate and line pointer
  X509 *cert; // X509 certificate pointer
  char *line; // line buffer

  // get the server's certificate
  cert = SSL_get_peer_certificate(irc.ssl);
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
