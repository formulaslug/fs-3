/*
 * Copyright (c) 2015 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file Socket.h Abstract Socket interface */
/** @addtogroup NetSocket
 * Mbed OS Socket API
 * @{ */

#ifndef SOCKET_H
#define SOCKET_H

#include "netsocket/SocketAddress.h"
#include "Callback.h"

/** Socket interface.
 *
 * This class defines the Mbed OS Socket API.
 * Socket is an abstract interface for communicating with remote endpoints.
 *
 * This API is intended for applications and libraries instead of
 * protocol-specific implementations. For example, TCPSocket
 * and UDPSocket are implementations of the Socket interface.
 * Socket API is intentionally not protocol-specific, and allows all protocol
 * to provide the same API regardless of the underlying transport mechanism.
 */
class Socket {
public:
    /** Destroy a socket.
     *
     *  Closes socket if the socket is still open.
     */
    virtual ~Socket() = default;

    /** Closes the socket.
     *
     *  Closes any open connection and deallocates any memory associated
     *  with the socket. Called from destructor if socket is not closed.
     *
     *  If this socket was allocated by an accept() call, then calling this will also delete
     *  the socket object itself.
     *
     *  @return         NSAPI_ERROR_OK on success.
     *                  Negative subclass-dependent error code on failure.
     */
    virtual nsapi_error_t close() = 0;

    /** Connects socket to a remote address.
     *
     *  Attempts to make connection on connection-mode protocol or set or reset
     *  the peer address on connectionless protocol.
     *
     *  Connectionless protocols also use the connected address to filter
     *  incoming packets for recv() and recvfrom() calls.
     *
     *  To reset the peer address, there must be zero initialized(default constructor) SocketAddress
     *  objects in the address parameter.
     *
     *  @note If connect() fails it is recommended to close the Socket and create
     *  a new one before attempting to reconnect.
     *
     *  @param address  The SocketAddress of the remote peer.
     *  @return         NSAPI_ERROR_OK on success.
     *                  Negative subclass-dependent error code on failure.
     */
    virtual nsapi_error_t connect(const SocketAddress &address) = 0;

    /** Send data on a socket
     *
     *  The socket must be connected to a remote host before send() call.
     *  Returns the number of bytes sent from the buffer.
     *  In case of connectionless socket, sends data to pre-specified remote.
     *
     *  By default, send blocks until all data is sent. If socket is set to
     *  non-blocking or times out, a partial amount can be written.
     *  NSAPI_ERROR_WOULD_BLOCK is returned if no data was written.
     *
     *  @param data     Buffer of data to send to the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         NSAPI_ERROR_OK on success.
     *                  Negative subclass-dependent error code on failure.
     */
    virtual nsapi_size_or_error_t send(const void *data, nsapi_size_t size) = 0;

    /** Receive data from a socket.
     *
     *  Receive data from connected socket, or in the case of connectionless socket,
     *  equivalent to calling recvfrom(NULL, data, size).
     *
     *  If socket is connected, only packets coming from connected peer address
     *  are accepted.
     *
     *  @note recv() is allowed write to data buffer even if an error occurs.
     *
     *  By default, recv blocks until some data is received. If socket is set to
     *  non-blocking or times out, NSAPI_ERROR_WOULD_BLOCK can be returned to
     *  indicate no data.
     *
     *  @param data     Destination buffer for data received from the host.
     *  @param size     Size of the buffer in bytes.
     *  @return         Number of received bytes on success, negative, subclass-dependent
     *                  error code on failure. If no data is available to be received
     *                  and the peer has performed an orderly shutdown,
     *                  recv() returns 0.
     */
    virtual nsapi_size_or_error_t recv(void *data, nsapi_size_t size) = 0;

    /** Send a message on a socket.
     *
     * The sendto() function sends a message through a connection-mode or connectionless-mode socket.
     * If the socket is a connectionless-mode socket, the message is sent to the address specified.
     * If the socket is a connected-mode socket, address is ignored.
     *
     * By default, sendto blocks until data is sent. If socket is set to
     * non-blocking or times out, NSAPI_ERROR_WOULD_BLOCK is returned
     * immediately.
     *
     *  @param address  Remote address
     *  @param data     Buffer of data to send to the host
     *  @param size     Size of the buffer in bytes
     *  @return         Number of sent bytes on success, negative subclass-dependent error
     *                  code on failure
     */
    virtual nsapi_size_or_error_t sendto(const SocketAddress &address,
                                         const void *data, nsapi_size_t size) = 0;

    /** Receive a data from a socket
     *
     *  Receives a data and stores the source address in address if address
     *  is not NULL. Returns the number of bytes written into the buffer.
     *
     *  If socket is connected, only packets coming from connected peer address
     *  are accepted.
     *
     *  @note recvfrom() is allowed write to address and data buffers even if error occurs.
     *
     *  By default, recvfrom blocks until a datagram is received. If socket is set to
     *  non-blocking or times out with no data, NSAPI_ERROR_WOULD_BLOCK
     *  is returned.
     *
     *  @param address  Destination for the source address or NULL
     *  @param data     Destination buffer for datagram received from the host
     *  @param size     Size of the buffer in bytes
     *  @return         Number of received bytes on success, negative subclass-dependent
     *                  error code on failure
     */
    virtual nsapi_size_or_error_t recvfrom(SocketAddress *address,
                                           void *data, nsapi_size_t size) = 0;

    /** Send a message on a socket.
     *
     * The sendto_control() function sends a message through a connection-mode or connectionless-mode socket.
     * If the socket is a connectionless-mode socket, the message is sent to the address specified.
     * If the socket is a connected-mode socket, address is ignored.
     *
     * Additional control information can be passed to the stack for specific operations.
     *
     * By default, sendto blocks until data is sent. If socket is set to
     * non-blocking or times out, NSAPI_ERROR_WOULD_BLOCK is returned
     * immediately.
     *
     *  @param address  Remote address
     *  @param data     Buffer of data to send to the host
     *  @param size     Size of the buffer in bytes
     *  @param control  Control data, for instance a populated #nsapi_pktinfo structure.
     *  @param control_size Size of \c control in bytes.
     *  @return         Number of sent bytes on success, negative subclass-dependent error
     *                  code on failure
     */
    virtual nsapi_size_or_error_t sendto_control(const SocketAddress &address,
                                                 const void *data, nsapi_size_t size,
                                                 nsapi_msghdr_t *control, nsapi_size_t control_size) = 0;


    /** Receive a data from a socket
     *
     *  Receives a data and stores the source address in address if address
     *  is not NULL. Returns the number of bytes written into the buffer.
     *
     *  If socket is connected, only packets coming from connected peer address
     *  are accepted.
     *
     *  Ancillary data is stored into \c control.  The caller needs to allocate a buffer
     *  that is large enough to contain the data they want to receive, then pass the pointer in
     *  through the \c control member.  The data will be filled into \c control, beginning with a header
     *  specifying what data was received.  See #MsgHeaderIterator for how to parse this data.
     *
     *  @note recvfrom_control() is allowed write to address and data buffers even if error occurs.
     *
     *  By default, recvfrom blocks until a datagram is received. If socket is set to
     *  non-blocking or times out with no data, NSAPI_ERROR_WOULD_BLOCK
     *  is returned.
     *
     *  @param address  Destination for the source address or NULL
     *  @param data     Destination buffer for datagram received from the host
     *  @param size     Size of the buffer in bytes
     *  @param control  Caller-allocated buffer to store ancillary data.
     *  @param control_size Size of the \c control buffer that the caller has allocated.
     *  @return         Number of received bytes on success, negative subclass-dependent
     *                  error code on failure
     */
    virtual nsapi_size_or_error_t recvfrom_control(SocketAddress *address,
                                                   void *data, nsapi_size_t size,
                                                   nsapi_msghdr_t *control, nsapi_size_t control_size) = 0;

    /** Bind a specific address to a socket.
     *
     *  Binding a socket specifies the address and port on which to receive
     *  data. If the IP address is zeroed, only the port is bound.
     *
     *  @param address  Local address to bind.
     *  @return         NSAPI_ERROR_OK on success, negative subclass-dependent error
     *                  code on failure.
     */
    virtual nsapi_error_t bind(const SocketAddress &address) = 0;

    /** Set blocking or non-blocking mode of the socket.
     *
     *  Initially all sockets are in blocking mode. In non-blocking mode
     *  blocking operations such as send/recv/accept return
     *  NSAPI_ERROR_WOULD_BLOCK if they cannot continue.
     *
     *  set_blocking(false) is equivalent to set_timeout(0)
     *  set_blocking(true) is equivalent to set_timeout(-1)
     *
     *  @param blocking true for blocking mode, false for non-blocking mode.
     */
    virtual void set_blocking(bool blocking) = 0;

    /** Set timeout on blocking socket operations.
     *
     *  Initially all sockets have unbounded timeouts. NSAPI_ERROR_WOULD_BLOCK
     *  is returned if a blocking operation takes longer than the specified
     *  timeout. A timeout of 0 removes the timeout from the socket. A negative
     *  value gives the socket an unbounded timeout.
     *
     *  set_timeout(0) is equivalent to set_blocking(false)
     *  set_timeout(-1) is equivalent to set_blocking(true)
     *
     *  @param timeout  Timeout in milliseconds
     */
    virtual void set_timeout(int timeout) = 0;

    /** Register a callback on state change of the socket.
     *
     *  The specified callback is called on state changes, such as when
     *  the socket can receive/send/accept successfully and when an error
     *  occurs. The callback may also be called spuriously without reason.
     *
     *  The callback may be called in an interrupt context and should not
     *  perform expensive operations such as receive/send calls.
     *
     *  Note! This is not intended as a replacement for a poll or attach-like
     *  asynchronous API, but rather as a building block for constructing
     *  such functionality. The exact timing of the registered function
     *  is not guaranteed and susceptible to change.
     *
     *  @param func     Function to call on state change.
     */
    virtual void sigio(mbed::Callback<void()> func) = 0;

    /** Set socket options.
     *
     *  setsockopt() allows an application to pass stack-specific options
     *  to the underlying stack using stack-specific level and option names,
     *  or to request generic options using levels from nsapi_socket_level_t.
     *
     *  For unsupported options, NSAPI_ERROR_UNSUPPORTED is returned
     *  and the socket is unmodified.
     *
     *  @param level    Stack-specific protocol level or nsapi_socket_level_t.
     *  @param optname  Level-specific option name.
     *  @param optval   Option value.
     *  @param optlen   Length of the option value.
     *  @retval         NSAPI_ERROR_OK on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET if socket is not open.
     *  @retval         Negative error code on failure, see #NetworkStack::setsockopt
     */
    virtual nsapi_error_t setsockopt(int level, int optname, const void *optval, unsigned optlen) = 0;

    /** Get socket options.
     *
     *  getsockopt() allows an application to retrieve stack-specific options
     *  from the underlying stack using stack-specific level and option names,
     *  or to request generic options using levels from nsapi_socket_level_t.
     *
     *  For unsupported options, NSAPI_ERROR_UNSUPPORTED is returned
     *  and the socket is unmodified.
     *
     *  @param level    Stack-specific protocol level or nsapi_socket_level_t.
     *  @param optname  Level-specific option name.
     *  @param optval   Destination for option value.
     *  @param optlen   Length of the option value.
     *  @retval         NSAPI_ERROR_OK on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET if socket is not open.
     *  @retval         Negative error code on failure, see #NetworkStack::getsockopt
     */
    virtual nsapi_error_t getsockopt(int level, int optname, void *optval, unsigned *optlen) = 0;

    /**
     *  @brief Waits for a client to try to connect to this socket, then returns a socket for the new connection.
     *
     *  On error, returns nullptr and sets the error code parameter (if provided).
     *
     *  Note that if a socket is returned, you must eventually call close() on it to release its resources.
     *  Calling close() will also delete the socket object, no separate \c delete is required.
     *  Referencing the returned socket after a close() call is not allowed and leads to undefined behavior.
     *
     *  By default, this function blocks until an incoming connection occurs. If socket is set to
     *  non-blocking or times out, an \c NSAPI_ERROR_WOULD_BLOCK error is returned.
     *
     *  @param error      If you wish to check the error code, pass an error code pointer here.
     *  @return           Pointer to a socket, or nullptr on error
     */
    virtual Socket *accept(nsapi_error_t *error = nullptr) = 0;

    /** Listen for incoming connections.
     *
     *  Marks the socket as a passive socket that can be used to accept
     *  incoming connections.
     *
     *  @param backlog  Number of pending connections that can be queued
     *                  simultaneously, defaults to 1.
     *  @return         NSAPI_ERROR_OK on success, negative error code on failure.
     */
    virtual nsapi_error_t listen(int backlog = 1) = 0;

    /** Get the remote-end peer associated with this socket.
     *
     * Copy the remote peer address to a SocketAddress structure pointed by
     * address parameter. Socket must be connected to have a peer address
     * associated.
     *
     *  @param address  Pointer to SocketAddress structure.
     *  @retval         NSAPI_ERROR_OK on success.
     *  @retval         NSAPI_ERROR_NO_SOCKET if socket is not connected.
     *  @retval         NSAPI_ERROR_NO_CONNECTION if the remote peer was not set.
     */
    virtual nsapi_error_t getpeername(SocketAddress *address) = 0;
};


#endif

/** @}*/
