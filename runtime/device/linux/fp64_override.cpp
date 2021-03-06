/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "core/utilities/debug_settings_reader_creator.h"
#include "runtime/os_interface/ocl_reg_path.h"

namespace NEO {

bool releaseFP64Override() {
    auto settingsReader = SettingsReaderCreator::create(oclRegPath);
    return settingsReader->getSetting("OverrideDefaultFP64Settings", -1) == 1;
}

} // namespace NEO
