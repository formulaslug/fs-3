//
// Created by wangd on 10/8/2024.
//

#if !DEVICE_CAN
#error[NOT_SUPPORTED] CAN not supported for this target
#endif

// Comment this out (or don't -D it) in production builds
#define ENABLE_DEBUG_PRINTF

#ifdef ENABLE_DEBUG_PRINTF
  #define DPRINT(...)   do { printf(__VA_ARGS__); } while(0)
#else
  #define DPRINT(...)   do { } while(0)
#endif

#include "mbed.h"
#include "src/can_wrapper.h"
#include "src/etc_controller.h"

EventFlags global_events;
ETCController* etc_handle;
CANWrapper* can_handle;

/**
 * Thread function that waits on global_events
 * Processes CAN Events
 * while(there's stuff in EventFlags):
 * Do the corresponding thing (defined in can wrapper class)
 * @return 1 if error
 */
void do_can_processing() {
    while (true) {
        // Wait for any event flag to be set (defined in the can wrapper class)
        uint32_t triggered_flags =
            global_events.wait_any(can_handle->THROTTLE_FLAG | can_handle->STATE_FLAG |
                                   can_handle->SYNC_FLAG | can_handle->RX_FLAG);

        /* Check for every event, process and then clear the corresponding flag */
        if (triggered_flags & can_handle->THROTTLE_FLAG) {
            DPRINT("Flag Trigger: Throttle\n");
            can_handle->sendThrottle();
            global_events.clear(can_handle->THROTTLE_FLAG);
        }
        if (triggered_flags & can_handle->STATE_FLAG) {
            DPRINT("Flag Trigger: State\n");
            can_handle->sendState();
            global_events.clear(can_handle->STATE_FLAG);
        }
        if (triggered_flags & can_handle->SYNC_FLAG) {
            DPRINT("Flag Trigger: Sync\n");
            can_handle->sendSync();
            global_events.clear(can_handle->SYNC_FLAG);
        }
        if (triggered_flags & can_handle->RX_FLAG) {
            DPRINT("Flag: CAN RX\n");
            can_handle->processCANRx();
            global_events.clear(can_handle->RX_FLAG);
        }
    }
}

/**
 * Initialize ETC and CAN Wrapper
 * while (true):
 * update state
 * process received messages
 * @return 1 if error
 */
int main() {
    DPRINT("ETC Main Function start!\n");
    etc_handle = new ETCController();
    can_handle = new CANWrapper(*etc_handle, global_events);

    Thread high_priority_thread(osPriorityHigh);
    high_priority_thread.start(do_can_processing);

    while (true) {
        DPRINT("ETC State Updated\n");
        /* update the etc-sensor readings */
        etc_handle->updateState();
    }
    return 0;
}
