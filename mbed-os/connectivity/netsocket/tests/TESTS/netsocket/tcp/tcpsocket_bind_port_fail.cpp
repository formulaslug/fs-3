/*
 * Copyright (c) 2018, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(MBED_CONF_RTOS_PRESENT)
#include "greentea-client/test_env.h"
#include "mbed.h"
#include "tcp_tests.h"
#include "TCPSocket.h"
#include "unity/unity.h"
#include "utest.h"

using namespace utest::v1;

void TCPSOCKET_BIND_PORT_FAIL()
{
    SKIP_IF_TCP_UNSUPPORTED();
    TCPSocket *sock = new TCPSocket;
    if (!sock) {
        TEST_FAIL();
        return;
    }
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, sock->open(get_network_interface()));
    nsapi_error_t bind_result = sock->bind(1024);
    if (bind_result == NSAPI_ERROR_UNSUPPORTED) {
        TEST_IGNORE_MESSAGE("bind() not supported");
        delete sock;
        return;
    } else {
        TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, bind_result);
    }

    TCPSocket *sock2 = new TCPSocket;
    if (!sock2) {
        TEST_FAIL();
    }
    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, sock2->open(get_network_interface()));
    TEST_ASSERT_EQUAL(NSAPI_ERROR_PARAMETER, sock2->bind(1024));

    delete sock;
    delete sock2;
}
#endif // defined(MBED_CONF_RTOS_PRESENT)
