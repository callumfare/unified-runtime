#include <OffloadAPI.h>
#include <ur/ur.hpp>
#include <ur_api.h>

#include "ur2offload.hpp"

UR_APIEXPORT ur_result_t UR_APICALL urDeviceGet(ur_platform_handle_t hPlatform,
                                                ur_device_type_t,
                                                uint32_t NumEntries,
                                                ur_device_handle_t *phDevices,
                                                uint32_t *pNumDevices) {

  if (pNumDevices) {
    if (auto Res = olGetDeviceCount(
            reinterpret_cast<ol_platform_handle_t>(hPlatform), pNumDevices)) {
      return offloadResultToUR(Res);
    }
  }

  if (phDevices) {
    if (auto Res = olGetDevice(
            reinterpret_cast<ol_platform_handle_t>(hPlatform), NumEntries,
            reinterpret_cast<ol_device_handle_t *>(phDevices))) {
      return offloadResultToUR(Res);
    }
  }

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urDeviceGetInfo(ur_device_handle_t hDevice,
                                                    ur_device_info_t propName,
                                                    size_t propSize,
                                                    void *pPropValue,
                                                    size_t *pPropSizeRet) {
  UrReturnHelper ReturnValue(propSize, pPropValue, pPropSizeRet);

  ol_device_info_t olInfo;
  switch (propName) {
  case UR_DEVICE_INFO_NAME:
    olInfo = OL_DEVICE_INFO_NAME;
    break;
  case UR_DEVICE_INFO_PARENT_DEVICE:
    return ReturnValue(nullptr);
  case UR_DEVICE_INFO_VERSION:
    return ReturnValue("");
  case UR_DEVICE_INFO_EXTENSIONS:
    return ReturnValue("");
  case UR_DEVICE_INFO_TYPE:
    olInfo = OL_DEVICE_INFO_TYPE;
    break;
  case UR_DEVICE_INFO_VENDOR:
    olInfo = OL_DEVICE_INFO_VENDOR;
    break;
  case UR_DEVICE_INFO_DRIVER_VERSION:
    olInfo = OL_DEVICE_INFO_DRIVER_VERSION;
    break;
  default:
    return UR_RESULT_ERROR_INVALID_ENUMERATION;
  }

  if (pPropSizeRet) {
    if (auto Res =
            olGetDeviceInfoSize(reinterpret_cast<ol_device_handle_t>(hDevice),
                                olInfo, pPropSizeRet)) {
      return offloadResultToUR(Res);
    }
  }

  if (pPropValue) {
    if (auto Res =
            olGetDeviceInfo(reinterpret_cast<ol_device_handle_t>(hDevice),
                            olInfo, propSize, pPropValue)) {
      return offloadResultToUR(Res);
    }
    // Need to explicitly map this type
    if (olInfo == OL_DEVICE_INFO_TYPE) {
      auto urPropPtr = reinterpret_cast<ur_device_type_t *>(pPropValue);
      auto olPropPtr = reinterpret_cast<ol_device_type_t *>(pPropValue);

      switch (*olPropPtr) {
      case OL_DEVICE_TYPE_CPU:
        *urPropPtr = UR_DEVICE_TYPE_CPU;
        break;
      case OL_DEVICE_TYPE_GPU:
        *urPropPtr = UR_DEVICE_TYPE_GPU;
        break;
      default:
        break;
      }
    }
  }

  return UR_RESULT_SUCCESS;
}

// Device partitioning is not supported in Offload, and won't be for some time.
// This means urDeviceRetain/Release are no-ops because all devices are root
// devices.

UR_APIEXPORT ur_result_t UR_APICALL urDeviceRetain(ur_device_handle_t) {
  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urDeviceRelease(ur_device_handle_t) {
  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL
urDevicePartition(ur_device_handle_t, const ur_device_partition_properties_t *,
                  uint32_t, ur_device_handle_t *, uint32_t *) {
  return UR_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
