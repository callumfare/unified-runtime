//===--------- ur2offload.hpp - LLVM Offload Adapter ----------------------===//
//
// Copyright (C) 2024 Intel Corporation
//
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM
// Exceptions. See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <offload_api.h>
#include <ur_api.h>

inline ur_result_t offloadResultToUR(offload_result_t Result) {
  switch (Result) {
  case OFFLOAD_RESULT_SUCCESS:
    return UR_RESULT_SUCCESS;
  case OFFLOAD_RESULT_ERROR_UNSUPPORTED_ENUMERATION:
    return UR_RESULT_ERROR_UNSUPPORTED_ENUMERATION;
  default:
    return UR_RESULT_ERROR_UNKNOWN;
  }
}
