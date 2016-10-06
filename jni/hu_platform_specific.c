/*
 * Platform specific and conditional definitions of functions for HU
 * Moved these here as they are not used, but might have a use
 * Haven't reviewed any of this yet
 * Used mainly to reduce the size of hu_uti
 */
// TODO:Review the need for this

#ifdef GENERIC_SERVER
#ifndef GENERIC_SERVER_INCLUDED
#define GENERIC_SERVER_INCLUDED

int gen_server_exiting = 0;

int gen_server_loop(int net_port,
                    int poll_ms) { // Run until gen_server_exiting != 0, passing
                                   // incoming commands to
                                   // gen_server_loop_func() and responding with
                                   // the results
  int sockfd = -1, newsockfd = -1, cmd_len = 0, ctr = 0;
  socklen_t cli_len = 0, srv_len = 0;
#ifdef CS_AF_UNIX
  struct sockaddr_un cli_addr = {0}, srv_addr = {0};
  srv_len = strlen(srv_addr.sun_path) + sizeof(srv_addr.sun_family);
#else
  struct sockaddr_in cli_addr = {0}, srv_addr = {0};
// struct hostent *hp;
#endif
  unsigned char cmd_buf[DEF_BUF] = {0};

#ifdef CS_AF_UNIX
  strlcpy(api_srvsock, DEF_API_SRVSOCK, sizeof(api_srvsock));
  char itoa_ret[MAX_ITOA_SIZE] = {0};
  strlcat(api_srvsock, itoa(net_port, itoa_ret, 10), sizeof(api_srvsock));
  unlink(api_srvsock);
#endif
  errno = 0;
  if ((sockfd = socket(CS_FAM, CS_SOCK_TYPE, 0)) < 0) { // Create socket
    loge("gen_server_loop socket  errno: %d (%s)", errno, strerror(errno));
    return (-1);
  }

  sock_reuse_set(sockfd);

  if (poll_ms != 0)
    sock_tmo_set(sockfd, poll_ms); // If polling mode, set socket timeout for
                                   // polling every poll_ms milliseconds

  memset((char *)&srv_addr, sizeof(srv_addr), 0);
#ifdef CS_AF_UNIX
  srv_addr.sun_family = AF_UNIX;
  strlcpy(srv_addr.sun_path, api_srvsock, sizeof(srv_addr.sun_path));
  srv_len = strlen(srv_addr.sun_path) + sizeof(srv_addr.sun_family);
#else
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK); // Will bind to loopback instead of common
                              // INADDR_ANY. Packets should only be received by
                              // loopback and never Internet.
  // For 2nd line of defence see: loge ("Unexpected suspicious packet from
  // host");
  // errno = 0;
  // hp = gethostbyname ("localhost");
  // if (hp == 0) {
  //  loge ("Error gethostbyname  errno: %d (%s)", errno, strerror (errno));
  //  return (-2);
  //}
  // bcopy ((char *) hp->h_addr, (char *) & srv_addr.sin_addr, hp->h_length);
  srv_addr.sin_port = htons(net_port);
  srv_len = sizeof(struct sockaddr_in);
#endif

#ifdef CS_AF_UNIX
  logd("srv_len: %d  fam: %d  path: %s", srv_len, srv_addr.sun_family,
       srv_addr.sun_path);
#else
  logd("srv_len: %d  fam: %d  addr: 0x%x  port: %d", srv_len,
       srv_addr.sin_family, ntohl(srv_addr.sin_addr.s_addr),
       ntohs(srv_addr.sin_port));
#endif
  errno = 0;
  if (bind(sockfd, (struct sockaddr *)&srv_addr, srv_len) <
      0) { // Bind socket to server address
    loge("Error bind  errno: %d (%s)", errno, strerror(errno));
#ifdef CS_AF_UNIX
    return (-3);
#endif
#ifdef CS_DGRAM
    return (-3);
#endif
    loge("Inet stream continuing despite bind error"); // OK to continue w/
                                                       // Internet Stream
  }

// Done after socket() and before bind() so don't repeat it here ?
// if (poll_ms != 0)
//  sock_tmo_set (sockfd, poll_ms);                                   // If
//  polling mode, set socket timeout for polling every poll_ms milliseconds

// Get command from client
#ifndef CS_DGRAM
  errno = 0;
  if (listen(sockfd, 5)) { // Backlog= 5; likely don't need this
    loge("Error listen  errno: %d (%s)", errno, strerror(errno));
    return (-4);
  }
#endif

  logd("gen_server_loop Ready");

  while (!gen_server_exiting) {
    memset((char *)&cli_addr, sizeof(cli_addr), 0); // ?? Don't need this ?
    // cli_addr.sun_family = CS_FAM;                                   // ""
    cli_len = sizeof(cli_addr);

// logd ("ms_get: %d",ms_get ());
#ifdef CS_DGRAM
    errno = 0;
    cmd_len = recvfrom(sockfd, cmd_buf, sizeof(cmd_buf), 0,
                       (struct sockaddr *)&cli_addr, &cli_len);
    if (cmd_len <= 0) {
      if (errno == EAGAIN) {
        if (poll_ms != 0)                // If timeout polling is enabled...
          gen_server_poll_func(poll_ms); // Do the polling work
        else
          loge("gen_server_loop EAGAIN !!!"); // Else EGAIN is an unexpected
                                              // error for blocking mode
      } else { // Else if some other error, sleep it off for 100 ms
        if (errno == EINTR)
          logw("Error recvfrom errno: %d (%s)", errno, strerror(errno));
        else
          loge("Error recvfrom errno: %d (%s)", errno, strerror(errno));
        quiet_ms_sleep(101);
      }
      continue;
    }
#ifndef CS_AF_UNIX
    // !!
    if (cli_addr.sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {
      // loge ("Unexpected suspicious packet from host %s", inet_ntop
      // (cli_addr.sin_addr.s_addr));
      loge("Unexpected suspicious packet from host"); // %s", inet_ntoa
      // (cli_addr.sin_addr.s_addr));
    }
#endif
#else
    errno = 0;
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
    if (newsockfd < 0) {
      loge("Error accept  errno: %d (%s)", errno, strerror(errno));
      ms_sleep(101); // Sleep 0.1 second to try to clear errors
      continue;
    }
#ifndef CS_AF_UNIX
    // !!
    if (cli_addr.sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {
      // loge ("Unexpected suspicious packet from host %s", inet_ntop
      // (cli_addr.sin_addr.s_addr));
      loge("Unexpected suspicious packet from host"); // %s", inet_ntoa
      // (cli_addr.sin_addr.s_addr));
    }
#endif
    errno = 0;
    cmd_len = read(newsockfd, cmd_buf, sizeof(cmd_buf));
    if (cmd_len <= 0) {
      loge("Error read  errno: %d (%s)", errno, strerror(errno));
      ms_sleep(101); // Sleep 0.1 second to try to clear errors
      close(newsockfd);
      ms_sleep(101); // Sleep 0.1 second to try to clear errors
      continue;
    }
#endif

#ifdef CS_AF_UNIX
// logd ("cli_len: %d  fam: %d  path:
// %s",cli_len,cli_addr.sun_family,cli_addr.sun_path);
#else
// logd ("cli_len: %d  fam: %d  addr: 0x%x  port:
// %d",cli_len,cli_addr.sin_family, ntohl (cli_addr.sin_addr.s_addr), ntohs
// (cli_addr.sin_port));
#endif
    // hex_dump ("", 32, cmd_buf, n);

    unsigned char res_buf[RES_DATA_MAX] = {0};
    int res_len = 0;

    cmd_buf[cmd_len] = 0; // Null terminate for string usage
    // Do server command function and provide response
    res_len = gen_server_loop_func(cmd_buf, cmd_len, res_buf, sizeof(res_buf));

    if (ena_log_verb)
      logd("gen_server_loop gen_server_loop_func res_len: %d", res_len);

    if (res_len < 0) { // If error
      res_len = 2;
      res_buf[0] = 0xff; // '?';   ?? 0xff for HCI ?
      res_buf[1] = 0xff; // '\n';
      res_buf[2] = 0;
    }
// hex_dump ("", 32, res_buf, res_len);

// Send response
#ifdef CS_DGRAM
    errno = 0;
    if (sendto(sockfd, res_buf, res_len, 0, (struct sockaddr *)&cli_addr,
               cli_len) != res_len) {
      loge("Error sendto  errno: %d (%s)  res_len: %d", errno, strerror(errno),
           res_len);
      ms_sleep(101); // Sleep 0.1 second to try to clear errors
    }
#else
    errno = 0;
    if (write(newsockfd, res_buf, res_len) !=
        res_len) { // Write, if can't write full buffer...
      loge("Error write  errno: %d (%s)", errno, strerror(errno));
      ms_sleep(101); // Sleep 0.1 second to try to clear errors
    }
    close(newsockfd);
#endif
  }
  close(sockfd);
#ifdef CS_AF_UNIX
  unlink(api_srvsock);
#endif

  return (0);
}

#endif //#ifndef GENERIC_SERVER_INCLUDED
#endif //#ifdef  GENERIC_SERVER

#ifdef GENERIC_CLIENT
#ifndef GENERIC_CLIENT_INCLUDED
#define GENERIC_CLIENT_INCLUDED

// Generic IPC API:

int gen_client_cmd(unsigned char *cmd_buf, int cmd_len, unsigned char *res_buf,
                   int res_max, int net_port, int rx_tmo) {
  logv("net_port: %d  cmd_buf: \"%s\"  cmd_len: %d", net_port, cmd_buf,
       cmd_len);
  static int sockfd = -1;
  int res_len = 0, written = 0, ctr = 0;
  static socklen_t srv_len = 0;
#ifdef CS_AF_UNIX
  static struct sockaddr_un srv_addr;
#ifdef CS_DGRAM
#define CS_DGRAM_UNIX
  struct sockaddr_un
      cli_addr; // Unix datagram sockets must be bound; no ephemeral sockets.
  socklen_t cli_len = 0;
#endif
#else
  // struct hostent *hp;
  struct sockaddr_in srv_addr, cli_addr;
  socklen_t cli_len = 0;
#endif

  if (sockfd < 0) {
    errno = 0;
    if ((sockfd = socket(CS_FAM, CS_SOCK_TYPE, 0)) <
        0) { // Get an ephemeral, unbound socket
      loge("gen_client_cmd: socket errno: %d (%s)", errno, strerror(errno));
      return (0);
    }
#ifdef CS_DGRAM_UNIX // Unix datagram sockets must be bound; no ephemeral
                     // sockets.
    strlcpy(api_clisock, DEF_API_CLISOCK, sizeof(api_clisock));
    char itoa_ret[MAX_ITOA_SIZE] = {0};
    strlcat(api_clisock, itoa(net_port, itoa_ret, 10), sizeof(api_clisock));
    unlink(api_clisock); // Remove any lingering client socket
    memset((char *)&cli_addr, sizeof(cli_addr), 0);
    cli_addr.sun_family = AF_UNIX;
    strlcpy(cli_addr.sun_path, api_clisock, sizeof(cli_addr.sun_path));
    cli_len = strlen(cli_addr.sun_path) + sizeof(cli_addr.sun_family);

    errno = 0;
    if (bind(sockfd, (struct sockaddr *)&cli_addr, cli_len) < 0) {
      loge("gen_client_cmd: bind errno: %d (%s)", errno, strerror(errno));
      close(sockfd);
      sockfd = -1;
      return (0); // OK to continue w/ Internet Stream but since this is Unix
                  // Datagram and we ran unlink (), let's fail
    }
#endif
  }
  //!! Can move inside above
  // Setup server address
  memset((char *)&srv_addr, sizeof(srv_addr), 0);
#ifdef CS_AF_UNIX
  strlcpy(api_srvsock, DEF_API_SRVSOCK, sizeof(api_srvsock));
  char itoa_ret[MAX_ITOA_SIZE] = {0};
  strlcat(api_srvsock, itoa(net_port, itoa_ret, 10), sizeof(api_srvsock));
  srv_addr.sun_family = AF_UNIX;
  strlcpy(srv_addr.sun_path, api_srvsock, sizeof(srv_addr.sun_path));
  srv_len = strlen(srv_addr.sun_path) + sizeof(srv_addr.sun_family);
#else
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  // errno = 0;
  // hp = gethostbyname ("localhost");
  // if (hp == 0) {
  //  loge ("gen_client_cmd: Error gethostbyname  errno: %d (%s)", errno,
  //  strerror (errno));
  //  return (0);
  //}
  // bcopy ((char *) hp->h_addr, (char *) & srv_addr.sin_addr, hp->h_length);
  srv_addr.sin_port = htons(net_port);
  srv_len = sizeof(struct sockaddr_in);
#endif

// Send cmd_buf and get res_buf
#ifdef CS_DGRAM
  errno = 0;
  written = sendto(sockfd, cmd_buf, cmd_len, 0,
                   (const struct sockaddr *)&srv_addr, srv_len);
  if (written != cmd_len) { // Dgram buffers should not be segmented
    loge("gen_client_cmd: sendto errno: %d (%s)", errno, strerror(errno));
#ifdef CS_DGRAM_UNIX
    unlink(api_clisock);
#endif
    close(sockfd);
    sockfd = -1;
    return (0);
  }

  sock_tmo_set(sockfd, rx_tmo);

  res_len = -1;
  ctr = 0;
  while (res_len < 0 && ctr < 2) {
    errno = 0;
    res_len = recvfrom(sockfd, res_buf, res_max, 0,
                       (struct sockaddr *)&srv_addr, &srv_len);
    ctr++;
    if (res_len < 0 && ctr < 2) {
      if (errno == EAGAIN)
        logw("gen_client_cmd: recvfrom errno: %d (%s)", errno,
             strerror(errno)); // Occasionally get EAGAIN here
      else
        loge("gen_client_cmd: recvfrom errno: %d (%s)", errno, strerror(errno));
    }
  }
  if (res_len <= 0) {
    loge("gen_client_cmd: recvfrom errno: %d (%s)", errno, strerror(errno));
#ifdef CS_DGRAM_UNIX
    unlink(api_clisock);
#endif
    close(sockfd);
    sockfd = -1;
    return (0);
  }
#ifndef CS_AF_UNIX
  // !!   ?? Don't need this ?? If srv_addr still set from sendto, should
  // restrict recvfrom to localhost anyway ?
  if (srv_addr.sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {
    loge("gen_client_cmd: Unexpected suspicious packet from host"); // %s",
    // inet_ntop(srv_addr.sin_addr.s_addr));
    // //inet_ntoa(srv_addr.sin_addr.s_addr));
  }
#endif
#else
  errno = 0;
  if (connect(sockfd, (struct sockaddr *)&srv_addr, srv_len) < 0) {
    loge("gen_client_cmd: connect errno: %d (%s)", errno, strerror(errno));
    close(sockfd);
    sockfd = -1;
    return (0);
  }
  errno = 0;
  written = write(sockfd, cmd_buf, cmd_len); // Write the command packet
  if (written !=
      cmd_len) { // Small buffers under 256 bytes should not be segmented ?
    loge("gen_client_cmd: write errno: %d (%s)", errno, strerror(errno));
    close(sockfd);
    sockfd = -1;
    return (0);
  }

  sock_tmo_set(sockfd, rx_tmo);

  errno = 0;
    res_len = read (sockfd, res_buf, res_max)); // Read response
    if (res_len <= 0) {
      loge("gen_client_cmd: read errno: %d (%s)", errno, strerror(errno));
      close(sockfd);
      sockfd = -1;
      return (0);
    }
#endif
// hex_dump ("", 32, res_buf, n);
#ifdef CS_DGRAM_UNIX
  unlink(api_clisock);
#endif
  // close (sockfd);
  return (res_len);
}

#endif //#ifndef GENERIC_CLIENT_INCLUDED
#endif //#ifdef  GENERIC_CLIENT
