/*
 * Copyright (c) 2017, ARM Limited, All Rights Reserved
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
#if !defined(MBED_CONF_RTOS_PRESENT)
#error [NOT_SUPPORTED] EMAC test cases require a RTOS to run
#else

#define ETHERNET 1
#define WIFI 2

#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"

#include "emac_tests.h"
#include "emac_util.h"
#include "OnboardNetworkStack.h"
#include "EmacTestNetworkStack.h"

using namespace utest::v1;

// Test setup
utest::v1::status_t test_setup(const size_t number_of_cases)
{
#ifdef MBED_GREENTEA_TEST_EMAC_TIMEOUT_S
    GREENTEA_SETUP(MBED_GREENTEA_TEST_EMAC_TIMEOUT_S, "default_auto");
#else
    GREENTEA_SETUP(1400, "default_auto");
#endif // #ifdef MBED_GREENTEA_TEST_EMAC_TIMEOUT_S

    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("EMAC initialize", test_emac_initialize),
    Case("EMAC broadcast", test_emac_broadcast), // note: this test case has the side effect of finding the CTP server MAC address and saving it
    Case("EMAC unicast", test_emac_unicast),
    Case("EMAC unicast frame length", test_emac_unicast_frame_len),
    Case("EMAC unicast burst", test_emac_unicast_burst),
    Case("EMAC unicast long", test_emac_unicast_long),
#if !((MBED_CONF_NETWORK_EMAC_NO_SUPPORT_FOR_MULTICAST_FILTER == 1) && \
    (MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE == WIFI))
    Case("EMAC multicast filter", test_emac_multicast_filter),
#endif // !(MBED_CONF_NETWORK_EMAC_NO_SUPPORT_FOR_MULTICAST_FILTER == 1)
    Case("EMAC memory", test_emac_memory),
    Case("EMAC power down and power up", test_emac_power_down_and_power_up),
    Case("EMAC unicast again after power cycle", test_emac_unicast),
};

Specification specification(test_setup, cases);

int main()
{
    return !Harness::run(specification);
}

// Override network stack selection to return the EmacTest network stack
OnboardNetworkStack &OnboardNetworkStack::get_default_instance()
{
    return EmacTestNetworkStack::get_instance();
}

#endif // !defined(MBED_CONF_RTOS_PRESENT)
