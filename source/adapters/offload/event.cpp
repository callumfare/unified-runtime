#include <OffloadAPI.h>
#include <ur_api.h>

#include "ur2offload.hpp"

UR_APIEXPORT ur_result_t UR_APICALL
urEventWait(uint32_t numEvents, const ur_event_handle_t *phEventWaitList) {
  // TODO: Check for errors
  for (uint32_t i = 0; i < numEvents; i++) {
    olWaitEvent(reinterpret_cast<ol_event_handle_t>(phEventWaitList[i]));
  }
  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urEventRetain(ur_event_handle_t hEvent) {
  auto OffloadEvent = reinterpret_cast<ol_event_handle_t>(hEvent);
  return offloadResultToUR(olRetainEvent(OffloadEvent));
}

UR_APIEXPORT ur_result_t UR_APICALL urEventRelease(ur_event_handle_t hEvent) {
  auto OffloadEvent = reinterpret_cast<ol_event_handle_t>(hEvent);
  return offloadResultToUR(olReleaseEvent(OffloadEvent));
}
