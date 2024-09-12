#include <offload_api.h>
#include <ur/ur.hpp>
#include <ur_api.h>

#include "ur2offload.hpp"

UR_APIEXPORT ur_result_t UR_APICALL
urPlatformGet(ur_adapter_handle_t *, uint32_t, uint32_t NumEntries,
              ur_platform_handle_t *phPlatforms, uint32_t *pNumPlatforms) {
    offload_result_t Res = offloadPlatformGet(
        NumEntries, reinterpret_cast<offload_platform_handle_t *>(phPlatforms),
        pNumPlatforms);

    return offloadResultToUR(Res);
}

UR_APIEXPORT ur_result_t UR_APICALL
urPlatformGetInfo(ur_platform_handle_t hPlatform, ur_platform_info_t propName,
                  size_t propSize, void *pPropValue, size_t *pPropSizeRet) {
    UrReturnHelper ReturnValue(propSize, pPropValue, pPropSizeRet);

    offload_platform_info_t olInfo;
    switch (propName) {
    case UR_PLATFORM_INFO_NAME:
        olInfo = OFFLOAD_PLATFORM_INFO_NAME;
        break;
    case UR_PLATFORM_INFO_VENDOR_NAME:
        olInfo = OFFLOAD_PLATFORM_INFO_VENDOR_NAME;
        break;
    case UR_PLATFORM_INFO_VERSION:
        olInfo = OFFLOAD_PLATFORM_INFO_VERSION;
        break;
    case UR_PLATFORM_INFO_EXTENSIONS:
        return ReturnValue("");
    case UR_PLATFORM_INFO_PROFILE:
        return ReturnValue("FULL_PROFILE");
    case UR_PLATFORM_INFO_BACKEND:
        olInfo = OFFLOAD_PLATFORM_INFO_BACKEND;
        break;
    default:
        return UR_RESULT_ERROR_INVALID_ENUMERATION;
    }

    auto Res = offloadPlatformGetInfo(
        reinterpret_cast<offload_platform_handle_t>(hPlatform), olInfo,
        propSize, pPropValue, pPropSizeRet);

    // Need to explicitly map this type
    if (olInfo == OFFLOAD_PLATFORM_INFO_BACKEND) {
        auto urPropPtr = reinterpret_cast<ur_platform_backend_t *>(pPropValue);
        auto olPropPtr =
            reinterpret_cast<offload_platform_backend_t *>(pPropValue);

        switch (*olPropPtr) {
        case OFFLOAD_PLATFORM_BACKEND_CUDA:
            *urPropPtr = UR_PLATFORM_BACKEND_CUDA;
            break;
        case OFFLOAD_PLATFORM_BACKEND_AMDGPU:
            *urPropPtr = UR_PLATFORM_BACKEND_HIP;
            break;
        default:
            break;
        }
    }

    return offloadResultToUR(Res);
}
