"""
Copyright (c) 2018 ARM Limited
SPDX-License-Identifier: Apache-2.0

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""
import time
from mbed_host_tests import BaseHostTest
from mbed_host_tests.host_tests_runner.host_test_default import DefaultTestSelector

DEFAULT_CYCLE_PERIOD = 10.0

MSG_VALUE_DUMMY = '0'

MSG_KEY_DEVICE_READY = 'crash_reporting_ready'
MSG_KEY_DEVICE_ERROR = 'crash_reporting_inject_error'
MSG_KEY_SYNC = '__sync'

class CrashReportingTest(BaseHostTest):
    """Test for the crash reporting feature.
    """

    def __init__(self):
        super(CrashReportingTest, self).__init__()
        self.reset = False
        self.test_steps_sequence = self.test_steps()
        # Advance the coroutine to it's first yield statement.
        self.test_steps_sequence.send(None)

    def setup(self):
        self.register_callback(MSG_KEY_DEVICE_READY, self.cb_device_ready)

        # Disable the default behavior of ending the test when the target experiences a fatal error.
        # In this test, we intentionally generate a fatal error!
        self.register_callback("mbed_error", lambda key, value, timestamp: None)

    def cb_device_ready(self, key, value, timestamp):
        """Acknowledge device rebooted correctly and feed the test execution
        """
        self.reset = True

        try:
            if self.test_steps_sequence.send(value):
                self.notify_complete(True)
        except (StopIteration, RuntimeError) as exc:
            self.notify_complete(False)

    def test_steps(self):
        """Reset the device and check the status
        """
        system_reset = yield
        self.reset = False

        wait_after_reset = self.get_config_item('forced_reset_timeout')
        wait_after_reset = wait_after_reset if wait_after_reset is not None else DEFAULT_CYCLE_PERIOD

        #Wait 2 seconds for system to init
        time.sleep(7.0)
        #self.send_kv(MSG_KEY_SYNC, MSG_VALUE_DUMMY)
        self.send_kv(MSG_KEY_DEVICE_ERROR, MSG_VALUE_DUMMY)
        time.sleep(5.0)
        
        system_reset = yield
        if self.reset == False:
            raise RuntimeError('Platform did not auto-reboot as expected. This is likely due to failing to auto-reboot after a reset, or failing to preserve the contents of crash data RAM across resets.')

        # The sequence is correct -- test passed.
        yield True
