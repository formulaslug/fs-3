//
// Created by wangd on 10/8/2024.
//

#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif

#include "src/can_wrapper.h"
#include "src/etc_controller.h"
#include "mbed.h"

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
  uint32_t triggered_flags = global_events.wait_any(can_handle->THROTTLE_FLAG | can_handle->STATE_FLAG | can_handle->SYNC_FLAG | can_handle->RX_FLAG);

  /* Check for every event, process and then clear the corresponding flag */
  if (triggered_flags & can_handle->THROTTLE_FLAG) {
   can_handle->sendThrottle();
   global_events.clear(can_handle->THROTTLE_FLAG);
  }
  if (triggered_flags & can_handle->STATE_FLAG) {
   can_handle->sendState();
   global_events.clear(can_handle->STATE_FLAG);
  }
  if (triggered_flags & can_handle->SYNC_FLAG) {
   can_handle->sendSync();
   global_events.clear(can_handle->SYNC_FLAG);
  }
  if (triggered_flags & can_handle->RX_FLAG) {
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
int main()
{
 etc_handle = new ETCController();
 can_handle = new CANWrapper(*etc_handle, global_events);

 Thread high_priority_thread(osPriorityHigh);
 high_priority_thread.start(do_can_processing);

 while(true) {
  /* update the etc-sensor readings */
  etc_handle->updatePedalTravel();
 }

 return 0;
}

