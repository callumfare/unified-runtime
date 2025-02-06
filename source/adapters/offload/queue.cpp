#include <OffloadAPI.h>
#include <ur/ur.hpp>
#include <ur_api.h>

#include "context.hpp"
#include "ur2offload.hpp"

UR_APIEXPORT ur_result_t UR_APICALL urQueueCreate(
    [[maybe_unused]] ur_context_handle_t hContext, ur_device_handle_t hDevice,
    const ur_queue_properties_t *, ur_queue_handle_t *phQueue) {

  assert(hContext->Device == hDevice);

  ol_queue_handle_t OffloadQueue;
  auto Res = olCreateQueue(reinterpret_cast<ol_device_handle_t>(hDevice),
                           &OffloadQueue);
  if (Res != OL_SUCCESS) {
    return offloadResultToUR(Res);
  }

  *phQueue = reinterpret_cast<ur_queue_handle_t>(OffloadQueue);

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urQueueRetain(ur_queue_handle_t hQueue) {
  auto OffloadQueue = reinterpret_cast<ol_queue_handle_t>(hQueue);
  return offloadResultToUR(olRetainQueue(OffloadQueue));
}

UR_APIEXPORT ur_result_t UR_APICALL urQueueRelease(ur_queue_handle_t hQueue) {
  auto OffloadQueue = reinterpret_cast<ol_queue_handle_t>(hQueue);
  return offloadResultToUR(olReleaseQueue(OffloadQueue));
}

UR_APIEXPORT ur_result_t UR_APICALL urQueueFinish(ur_queue_handle_t hQueue) {
  auto OffloadQueue = reinterpret_cast<ol_queue_handle_t>(hQueue);
  return offloadResultToUR(olFinishQueue(OffloadQueue));
}
