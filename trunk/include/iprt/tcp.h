/** @file
 * IPRT - TCP/IP.
 */

/*
 * Copyright (C) 2006-2010 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 */

#ifndef ___iprt_tcp_h
#define ___iprt_tcp_h

#include <iprt/cdefs.h>
#include <iprt/types.h>
#include <iprt/thread.h>
#include <iprt/net.h>
#include <iprt/sg.h>

#ifdef IN_RING0
# error "There are no RTFile APIs available Ring-0 Host Context!"
#endif


RT_C_DECLS_BEGIN

/** @defgroup grp_rt_tcp    RTTcp - TCP/IP
 * @ingroup grp_rt
 * @{
 */


/**
 * Serve a TCP Server connection.
 *
 * @returns iprt status code.
 * @returns VERR_TCP_SERVER_STOP to terminate the server loop forcing
 *          the RTTcpCreateServer() call to return.
 * @param   Sock        The socket which the client is connected to.
 *                      The call will close this socket.
 * @param   pvUser      User argument.
 */
typedef DECLCALLBACK(int) FNRTTCPSERVE(RTSOCKET Sock, void *pvUser);
/** Pointer to a RTTCPSERVE(). */
typedef FNRTTCPSERVE *PFNRTTCPSERVE;

/** Pointer to a RTTCPSERVER handle. */
typedef struct RTTCPSERVER *PRTTCPSERVER;
/** Pointer to a RTTCPSERVER handle. */
typedef PRTTCPSERVER *PPRTTCPSERVER;

/**
 * Create single connection at a time TCP Server in a separate thread.
 *
 * The thread will loop accepting connections and call pfnServe for
 * each of the incoming connections in turn. The pfnServe function can
 * return VERR_TCP_SERVER_STOP too terminate this loop. RTTcpServerDestroy()
 * should be used to terminate the server.
 *
 * @returns iprt status code.
 * @param   pszAddress      The address for creating a listening socket.
 *                          If NULL or empty string the server is bound to all interfaces.
 * @param   uPort           The port for creating a listening socket.
 * @param   enmType         The thread type.
 * @param   pszThrdName     The name of the worker thread.
 * @param   pfnServe        The function which will serve a new client connection.
 * @param   pvUser          User argument passed to pfnServe.
 * @param   ppServer        Where to store the serverhandle.
 */
RTR3DECL(int)  RTTcpServerCreate(const char *pszAddress, unsigned uPort, RTTHREADTYPE enmType, const char *pszThrdName,
                                 PFNRTTCPSERVE pfnServe, void *pvUser, PPRTTCPSERVER ppServer);

/**
 * Create single connection at a time TCP Server.
 * The caller must call RTTcpServerListen() to actually start the server.
 *
 * @returns iprt status code.
 * @param   pszAddress      The address for creating a listening socket.
 *                          If NULL the server is bound to all interfaces.
 * @param   uPort           The port for creating a listening socket.
 * @param   ppServer        Where to store the serverhandle.
 */
RTR3DECL(int) RTTcpServerCreateEx(const char *pszAddress, uint32_t uPort, PPRTTCPSERVER ppServer);

/**
 * Closes down and frees a TCP Server.
 * This will also terminate any open connections to the server.
 *
 * @returns iprt status code.
 * @param   pServer         Handle to the server.
 */
RTR3DECL(int) RTTcpServerDestroy(PRTTCPSERVER pServer);

/**
 * Listen for incoming connections.
 *
 * The function will loop accepting connections and call pfnServe for
 * each of the incoming connections in turn. The pfnServe function can
 * return VERR_TCP_SERVER_STOP too terminate this loop. A stopped server
 * can only be destroyed.
 *
 * @returns iprt status code.
 * @param   pServer         The server handle as returned from RTTcpServerCreateEx().
 * @param   pfnServe        The function which will serve a new client connection.
 * @param   pvUser          User argument passed to pfnServe.
 */
RTR3DECL(int) RTTcpServerListen(PRTTCPSERVER pServer, PFNRTTCPSERVE pfnServe, void *pvUser);

/**
 * Listen and accept one incomming connection.
 *
 * This is an alternative to RTTcpServerListen for the use the callbacks are not
 * possible.
 *
 * @returns IPRT status code.
 * @retval  VERR_TCP_SERVER_SHUTDOWN if shut down by RTTcpServerShutdown.
 * @retval  VERR_INTERRUPTED if the listening was interrupted.
 *
 * @param   pServer         The server handle as returned from RTTcpServerCreateEx().
 * @param   phClientSocket  Where to return the socket handle to the client
 *                          connection (on success only).  This must be closed
 *                          by calling RTTcpServerDisconnectClient2().
 */
RTR3DECL(int) RTTcpServerListen2(PRTTCPSERVER pServer, PRTSOCKET phClientSocket);

/**
 * Terminate the open connection to the server.
 *
 * @returns iprt status code.
 * @param   pServer         Handle to the server.
 */
RTR3DECL(int) RTTcpServerDisconnectClient(PRTTCPSERVER pServer);

/**
 * Terminates an open client connect when using RTTcpListen2
 *
 * @returns IPRT status code.
 * @param   hClientSocket   The client socket handle.  This will be invalid upon
 *                          return, whether successful or not.  NIL is quietly
 *                          ignored (VINF_SUCCESS).
 */
RTR3DECL(int) RTTcpServerDisconnectClient2(RTSOCKET hClientSocket);

/**
 * Shuts down the server, leaving client connections open.
 *
 * @returns IPRT status code.
 * @param   pServer         Handle to the server.
 */
RTR3DECL(int) RTTcpServerShutdown(PRTTCPSERVER pServer);

/**
 * Connect (as a client) to a TCP Server.
 *
 * @returns iprt status code.
 * @param   pszAddress      The address to connect to.
 * @param   uPort           The port to connect to.
 * @param   pSock           Where to store the handle to the established connection.
 */
RTR3DECL(int) RTTcpClientConnect(const char *pszAddress, uint32_t uPort, PRTSOCKET pSock);

/**
 * Close a socket returned by RTTcpClientConnect().
 *
 * @returns iprt status code.
 * @param   Sock        Socket descriptor.
 */
RTR3DECL(int) RTTcpClientClose(RTSOCKET Sock);

/**
 * Receive data from a socket.
 *
 * @returns iprt status code.
 * @param   Sock        Socket descriptor.
 * @param   pvBuffer    Where to put the data we read.
 * @param   cbBuffer    Read buffer size.
 * @param   pcbRead     Number of bytes read.
 *                      If NULL the entire buffer will be filled upon successful return.
 *                      If not NULL a partial read can be done successfully.
 */
RTR3DECL(int)  RTTcpRead(RTSOCKET Sock, void *pvBuffer, size_t cbBuffer, size_t *pcbRead);

/**
 * Send data to a socket.
 *
 * @returns iprt status code.
 * @retval  VERR_INTERRUPTED if interrupted before anything was written.
 *
 * @param   Sock        Socket descriptor.
 * @param   pvBuffer    Buffer to write data to socket.
 * @param   cbBuffer    How much to write.
 */
RTR3DECL(int)  RTTcpWrite(RTSOCKET Sock, const void *pvBuffer, size_t cbBuffer);

/**
 * Flush socket write buffers.
 *
 * @returns iprt status code.
 * @param   Sock        Socket descriptor.
 */
RTR3DECL(int)  RTTcpFlush(RTSOCKET Sock);

/**
 * Enables or disables delaying sends to coalesce packets.
 *
 * The TCP/IP stack usually uses the Nagle algorithm (RFC 896) to implement the
 * coalescing.
 *
 * @returns iprt status code.
 * @param   Sock        Socket descriptor.
 * @param   fEnable     When set to true enables coalescing.
 */
RTR3DECL(int)  RTTcpSetSendCoalescing(RTSOCKET Sock, bool fEnable);

/**
 * Socket I/O multiplexing.
 * Checks if the socket is ready for reading.
 *
 * @returns iprt status code.
 * @param   Sock        Socket descriptor.
 * @param   cMillies    Number of milliseconds to wait for the socket.
 *                      Use RT_INDEFINITE_WAIT to wait for ever.
 */
RTR3DECL(int)  RTTcpSelectOne(RTSOCKET Sock, RTMSINTERVAL cMillies);


#if 0 /* skipping these for now - RTTcpServer* handles this. */
/**
 * Listen for connection on a socket.
 *
 * @returns iprt status code.
 * @param   Sock        Socket descriptor.
 * @param   cBackLog    The maximum length the queue of pending connections
 *                      may grow to.
 */
RTR3DECL(int)  RTTcpListen(RTSOCKET Sock, int cBackLog);

/**
 * Accept a connection on a socket.
 *
 * @returns iprt status code.
 * @param   Sock            Socket descriptor.
 * @param   uPort           The port for accepting connection.
 * @param   pSockAccepted   Where to store the handle to the accepted connection.
 */
RTR3DECL(int)  RTTcpAccept(RTSOCKET Sock, unsigned uPort, PRTSOCKET pSockAccepted);

#endif

/**
 * Gets the address of the local side.
 *
 * @returns IPRT status code.
 * @param   Sock            Socket descriptor.
 * @param   pAddr           Where to store the local address on success.
 */
RTR3DECL(int) RTTcpGetLocalAddress(RTSOCKET Sock, PRTNETADDR pAddr);

/**
 * Gets the address of the other party.
 *
 * @returns IPRT status code.
 * @param   Sock            Socket descriptor.
 * @param   pAddr           Where to store the peer address on success.
 */
RTR3DECL(int) RTTcpGetPeerAddress(RTSOCKET Sock, PRTNETADDR pAddr);

/**
 * Send data from a scatter/gather buffer to a socket.
 *
 * @returns iprt status code.
 * @retval  VERR_INTERRUPTED if interrupted before anything was written.
 *
 * @param   Sock        Socket descriptor.
 * @param   pSgBuf      Scatter/gather buffer to write data to socket.
 */
RTR3DECL(int)  RTTcpSgWrite(RTSOCKET Sock, PCRTSGBUF pSgBuf);

/** @} */
RT_C_DECLS_END

#endif

