/* Copyright 2015 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if GOOGLE_CUDA

#define EIGEN_USE_GPU

#include "tensorflow/core/common_runtime/gpu/gpu_device.h"
#include "tensorflow/core/common_runtime/gpu/process_state.h"

namespace tensorflow {

class GPUDevice : public BaseGPUDevice {
 public:
  GPUDevice(const SessionOptions& options, const string& name,
            Bytes memory_limit, BusAdjacency bus_adjacency, int gpu_id,
            const string& physical_device_desc, Allocator* gpu_allocator,
            Allocator* cpu_allocator)
      : BaseGPUDevice(options, name, memory_limit, bus_adjacency, gpu_id,
                      physical_device_desc, gpu_allocator, cpu_allocator,
                      false /* sync every op */, 1 /* max_streams */) {}

  Allocator* GetAllocator(AllocatorAttributes attr) override {
    if (attr.on_host()) {
      ProcessState* ps = ProcessState::singleton();
      if (attr.gpu_compatible()) {
        return ps->GetCUDAHostAllocator(0);
      } else {
        return cpu_allocator_;
      }
    } else {
      return gpu_allocator_;
    }
  }
};

class GPUDeviceFactory : public BaseGPUDeviceFactory {
 private:
  LocalDevice* CreateGPUDevice(const SessionOptions& options,
                               const string& name, Bytes memory_limit,
                               BusAdjacency bus_adjacency, int gpu_id,
                               const string& physical_device_desc,
                               Allocator* gpu_allocator,
                               Allocator* cpu_allocator) override {
    return new GPUDevice(options, name, memory_limit, bus_adjacency, gpu_id,
                         physical_device_desc, gpu_allocator, cpu_allocator);
  }
};

REGISTER_LOCAL_DEVICE_FACTORY("GPU", GPUDeviceFactory);

}  // namespace tensorflow

#endif  // GOOGLE_CUDA
