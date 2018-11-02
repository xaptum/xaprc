# xbridge

A userspace application to bridge two network interfaces.

This is accomplished by capturing and injecting raw packets on both
interfaces, using libpcap.

This is part of the pass-through mode for XAP-RW-001.

## Supported Product IDs

| Model #    | Description                | USB Vendor ID | USB Product ID |
|------------|----------------------------|---------------|----------------|
| XAP-RW-001 | Xaptum Router Card w/ WiFi | 0x2FE0        | 0x8B02         |

## Installation

### From Source

#### Build Dependencies

* CMake (version 3.0 or higher)
* A C99-compliant compiler
* libpcap

### Building the Binary

``` bash
# Create a subdirectory to hold the build
mkdir -p build
cd build

# Configure the build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build xbridge
cmake --build .

# Install xbridge
cmake --build . --target install
```
### CMake Options

The following CMake configuration options are supported.

| Option               | Values         | Default    | Description                                |
|----------------------|----------------|------------|--------------------------------------------|
| CMAKE_BUILD_TYPE     | Release        |            | With full optimizations.                   |
|                      | Debug          |            | With debug symbols.                        |
|                      | RelWithDebInfo |            | With full optimizations and debug symbols. |
| CMAKE_INSTALL_PREFIX | <string>       | /usr/local | The directory to install the library in.   |

## Usage

### Using Systemd

A [`systemd`]() unit file to manage xbridge is included in this repo.

``` bash
# Enable xbridge to start on boot
systemctl enable xbridge

# Start xbridge manually
systemctl start xbridge
```

## License
Copyright 2018 Xaptum, Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this work except in compliance with the License. You may obtain a copy of
the License from the LICENSE.txt file or at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.
