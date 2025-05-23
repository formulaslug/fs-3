/*
 * Copyright (c) 2020, Arm Limited and affiliates.
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

#include "TELIT_ME310.h"
#include "TELIT_ME310_CellularContext.h"
#include "TELIT_ME310_CellularStack.h"
#include "TELIT_ME310_CellularNetwork.h"
#include "AT_CellularNetwork.h"
#include "PinNames.h"
#include "rtos/ThisThread.h"

using namespace std::chrono_literals;
using namespace mbed;
using namespace rtos;
using namespace events;

#if !defined(MBED_CONF_TELIT_ME310_PWR)
#define MBED_CONF_TELIT_ME310_PWR    NC
#endif

#if !defined(MBED_CONF_TELIT_ME310_TX)
#define MBED_CONF_TELIT_ME310_TX    NC
#endif

#if !defined(MBED_CONF_TELIT_ME310_RX)
#define MBED_CONF_TELIT_ME310_RX    NC
#endif

#if !defined(MBED_CONF_TELIT_ME310_POLARITY)
#define MBED_CONF_TELIT_ME310_POLARITY    1 // active high
#endif

static const intptr_t cellular_properties[AT_CellularDevice::PROPERTY_MAX] = {
    AT_CellularNetwork::RegistrationModeLAC,    // C_EREG
    AT_CellularNetwork::RegistrationModeLAC,    // C_GREG
    AT_CellularNetwork::RegistrationModeLAC,    // C_REG
    0,  // AT_CGSN_WITH_TYPE
    0,  // AT_CGDATA
    1,  // AT_CGAUTH
    1,  // AT_CNMI
    1,  // AT_CSMP
    1,  // AT_CMGF
    1,  // AT_CSDH
    1,  // PROPERTY_IPV4_STACK
    1,  // PROPERTY_IPV6_STACK
    1,  // PROPERTY_IPV4V6_STACK
    0,  // PROPERTY_NON_IP_PDP_TYPE
    1,  // PROPERTY_AT_CGEREP,
    1,  // PROPERTY_AT_COPS_FALLBACK_AUTO
    6,  // PROPERTY_SOCKET_COUNT
    1,  // PROPERTY_IP_TCP
    1,  // PROPERTY_IP_UDP
    20, // PROPERTY_AT_SEND_DELAY
};

TELIT_ME310::TELIT_ME310(FileHandle *fh, PinName pwr, bool active_high)
    : AT_CellularDevice(fh),
      _active_high(active_high),
      _pwr_key(pwr, !_active_high)
{
    set_cellular_properties(cellular_properties);
}

AT_CellularContext *TELIT_ME310::create_context_impl(ATHandler &at, const char *apn, bool cp_req, bool nonip_req)
{
    return new TELIT_ME310_CellularContext(at, this, apn, cp_req, nonip_req);
}

nsapi_error_t TELIT_ME310::init()
{
    nsapi_error_t err = AT_CellularDevice::init();
    if (err != NSAPI_ERROR_OK) {
        return err;
    }
    _at.lock();
#if defined (MBED_CONF_TELIT_ME310_RTS) && defined (MBED_CONF_TELIT_ME310_CTS)
    _at.at_cmd_discard("&K3;&C1;&D0", "");
#else
    _at.at_cmd_discard("&K0;&C1;&D0", "");
#endif

    // AT#QSS=1
    // Enable the Query SIM Status unsolicited indication in the ME. The format of the
    // unsolicited indication is the following:
    // #QSS: <status>
    // The ME informs at
    // every SIM status change through the basic unsolicited indication where <status> range is 0...1
    // <status> values:
    // - 0: SIM not inserted
    // - 1: SIM inserted
    _at.at_cmd_discard("#QSS", "=1");

    // AT#PSNT=1
    // Set command enables unsolicited result code for packet service network type (PSNT)
    // having the following format:
    // #PSNT:<nt>
    // <nt> values:
    // - 0: GPRS network
    // - 4: LTE network
    // - 5: unknown or not registered
    _at.at_cmd_discard("#PSNT", "=1");

    // AT+CMER=2
    // Set command enables sending of unsolicited result codes from TA to TE in the case of
    // indicator state changes.
    // Current setting: buffer +CIEV Unsolicited Result Codes in the TA when TA-TE link is
    // reserved (e.g. on-line data mode) and flush them to the TE after
    // reservation; otherwise forward them directly to the TE
    _at.at_cmd_discard("+CMER", "=2");

    // AT+CMEE=2
    // Set command disables the use of result code +CME ERROR: <err> as an indication of an
    // error relating to the +Cxxx command issued. When enabled, device related errors cause the +CME
    // ERROR: <err> final result code instead of the default ERROR final result code. ERROR is returned
    // normally when the error message is related to syntax, invalid parameters or DTE functionality.
    // Current setting: enable and use verbose <err> values
    _at.at_cmd_discard("+CMEE", "=2");

    // AT&W&P
    // - AT&W: Execution command stores on profile <n> the complete configuration of the device. If
    //         parameter is omitted, the command has the same behavior of AT&W0.
    // - AT&P: Execution command defines which full profile will be loaded at startup. If parameter
    //         is omitted, the command has the same behavior as AT&P0
    _at.at_cmd_discard("&W&P", "");

    return _at.unlock_return_error();
}

#if MBED_CONF_TELIT_ME310_PROVIDE_DEFAULT
#include "drivers/BufferedSerial.h"
CellularDevice *CellularDevice::get_default_instance()
{
    static BufferedSerial serial(MBED_CONF_TELIT_ME310_TX, MBED_CONF_TELIT_ME310_RX, MBED_CONF_TELIT_ME310_BAUDRATE);
#if defined (MBED_CONF_TELIT_ME310_RTS) && defined (MBED_CONF_TELIT_ME310_CTS)
    serial.set_flow_control(SerialBase::RTSCTS, MBED_CONF_TELIT_ME310_RTS, MBED_CONF_TELIT_ME310_CTS);
#endif
    static TELIT_ME310 device(&serial,
                              MBED_CONF_TELIT_ME310_PWR,
                              MBED_CONF_TELIT_ME310_POLARITY);
    return &device;
}

/*
 * With e.g. GCC linker option "--undefined=<LINK_FOO>", pull in this
 * object file anyway for being able to override weak symbol successfully
 * even though from static library. See:
 * https://stackoverflow.com/questions/42588983/what-does-the-gnu-ld-undefined-option-do
 *
 * NOTE: For C++ name mangling, 'extern "C"' is necessary to match the
 *       <LINK_FOO> symbol correctly.
 */
extern "C"
void LINK_TELIT_ME310_CPP(void)
{
}
#endif

nsapi_error_t TELIT_ME310::hard_power_on()
{
    soft_power_on();

    return NSAPI_ERROR_OK;
}

nsapi_error_t TELIT_ME310::soft_power_on()
{
    _pwr_key = _active_high;
    ThisThread::sleep_for(500ms);
    _pwr_key = !_active_high;
    ThisThread::sleep_for(5s);
    _pwr_key = _active_high;
    ThisThread::sleep_for(5s);

    return NSAPI_ERROR_OK;
}

nsapi_error_t TELIT_ME310::hard_power_off()
{
    _pwr_key = !_active_high;
    ThisThread::sleep_for(10s);

    return NSAPI_ERROR_OK;
}

nsapi_error_t TELIT_ME310::soft_power_off()
{
    return AT_CellularDevice::soft_power_off();
}

AT_CellularNetwork *TELIT_ME310::open_network_impl(ATHandler &at)
{
    return new TELIT_ME310_CellularNetwork(at, *this);
}
