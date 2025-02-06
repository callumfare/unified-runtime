#include "kernel.hpp"
#include "ur2offload.hpp"
#include <OffloadAPI.h>
#include <ur/ur.hpp>
#include <ur_api.h>

UR_APIEXPORT ur_result_t UR_APICALL
urKernelCreate(ur_program_handle_t hProgram, const char *pKernelName,
               ur_kernel_handle_t *phKernel) {
  ol_kernel_handle_t OffloadKernel;

  auto Res = olCreateKernel(reinterpret_cast<ol_program_handle_t>(hProgram),
                            pKernelName, &OffloadKernel);

  if (Res != OL_SUCCESS) {
    return offloadResultToUR(Res);
  }

  *phKernel = reinterpret_cast<ur_kernel_handle_t>(OffloadKernel);

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urKernelRetain(ur_kernel_handle_t hKernel) {
  return offloadResultToUR(
      olRetainKernel(reinterpret_cast<ol_kernel_handle_t>(hKernel)));
}

UR_APIEXPORT ur_result_t UR_APICALL
urKernelRelease(ur_kernel_handle_t hKernel) {
  return offloadResultToUR(
      olReleaseKernel(reinterpret_cast<ol_kernel_handle_t>(hKernel)));
}

UR_APIEXPORT ur_result_t UR_APICALL
urKernelSetExecInfo(ur_kernel_handle_t, ur_kernel_exec_info_t, size_t,
                    const ur_kernel_exec_info_properties_t *, const void *) {
  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urKernelSetArgPointer(
    ur_kernel_handle_t hKernel, uint32_t argIndex,
    const ur_kernel_arg_pointer_properties_t *, const void *pArgValue) {
  // setKernelArg is expecting a pointer to our argument
  return offloadResultToUR(
      olSetKernelArgValue(reinterpret_cast<ol_kernel_handle_t>(hKernel),
                          argIndex, sizeof(pArgValue), &pArgValue));
}

UR_APIEXPORT ur_result_t UR_APICALL urKernelSetArgValue(
    ur_kernel_handle_t hKernel, uint32_t argIndex, size_t argSize,
    const ur_kernel_arg_value_properties_t *, const void *pArgValue) {
  return offloadResultToUR(
      olSetKernelArgValue(reinterpret_cast<ol_kernel_handle_t>(hKernel),
                          argIndex, argSize, (void *)pArgValue));
}

UR_APIEXPORT ur_result_t UR_APICALL
urKernelGetGroupInfo(ur_kernel_handle_t hKernel, ur_device_handle_t hDevice,
                     ur_kernel_group_info_t propName, size_t propSize,
                     void *pPropValue, size_t *pPropSizeRet) {
  UrReturnHelper ReturnValue(propSize, pPropValue, pPropSizeRet);

  if (propName == UR_KERNEL_GROUP_INFO_COMPILE_WORK_GROUP_SIZE) {
    size_t GroupSize[3] = {0, 0, 0};
    return ReturnValue(GroupSize, 3);
  }
  return UR_RESULT_ERROR_UNSUPPORTED_ENUMERATION;
}
