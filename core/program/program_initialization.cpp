/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "program_initialization.h"

#include "core/compiler_interface/linker.h"
#include "core/device/device.h"
#include "core/helpers/string.h"
#include "core/memory_manager/graphics_allocation.h"
#include "core/memory_manager/memory_manager.h"
#include "core/memory_manager/unified_memory_manager.h"
#include "core/program/program_info.h"

namespace NEO {

GraphicsAllocation *allocateGlobalsSurface(NEO::SVMAllocsManager *const svmAllocManager, NEO::Device &device, size_t size, bool constant, LinkerInput *const linkerInput, const void *const initData) {
    bool globalsAreExported = false;
    if (linkerInput != nullptr) {
        globalsAreExported = constant ? linkerInput->getTraits().exportsGlobalConstants : linkerInput->getTraits().exportsGlobalVariables;
    }

    if (globalsAreExported && (svmAllocManager != nullptr)) {
        NEO::SVMAllocsManager::SvmAllocationProperties svmProps = {};
        svmProps.coherent = false;
        svmProps.readOnly = constant;
        svmProps.hostPtrReadOnly = constant;
        auto ptr = svmAllocManager->createSVMAlloc(device.getRootDeviceIndex(), size, svmProps);
        DEBUG_BREAK_IF(ptr == nullptr);
        if (ptr == nullptr) {
            return nullptr;
        }
        auto svmAlloc = svmAllocManager->getSVMAlloc(ptr);
        UNRECOVERABLE_IF(svmAlloc == nullptr);
        auto gpuAlloc = svmAlloc->gpuAllocation;
        UNRECOVERABLE_IF(gpuAlloc == nullptr);
        device.getMemoryManager()->copyMemoryToAllocation(gpuAlloc, initData, static_cast<uint32_t>(size));
        return svmAllocManager->getSVMAlloc(ptr)->gpuAllocation;
    } else {
        auto allocationType = constant ? GraphicsAllocation::AllocationType::CONSTANT_SURFACE : GraphicsAllocation::AllocationType::GLOBAL_SURFACE;
        auto gpuAlloc = device.getMemoryManager()->allocateGraphicsMemoryWithProperties({device.getRootDeviceIndex(), size, allocationType});
        DEBUG_BREAK_IF(gpuAlloc == nullptr);
        if (gpuAlloc == nullptr) {
            return nullptr;
        }
        memcpy_s(gpuAlloc->getUnderlyingBuffer(), gpuAlloc->getUnderlyingBufferSize(), initData, size);
        return gpuAlloc;
    }
}

} // namespace NEO
