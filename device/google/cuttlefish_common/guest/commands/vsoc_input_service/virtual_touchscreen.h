#pragma once
/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "virtual_device_base.h"

namespace vsoc_input_service {

class VirtualTouchScreen : public VirtualDeviceBase {
 public:
  VirtualTouchScreen(uint32_t width, uint32_t lenght);

 protected:
  virtual const std::vector<const uint32_t>& GetEventTypes() const;
  virtual const std::vector<const uint32_t>& GetKeys() const;
  virtual const std::vector<const uint32_t>& GetProperties() const;
  virtual const std::vector<const uint32_t>& GetAbs() const;
};

}  // namespace vsoc_input_service
