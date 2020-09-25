# mender-certs

This package installs the public certificate used by the mender client to

- validate the signature on firmware artifacts

## CMake Options

The following CMake configuration options are supported.

| Option              | Values    | Default     | Description                                    |
|---------------------|-----------|-------------|------------------------------------------------|
| MENDER_INSTALL_PATH |           | /etc/mender | The directory to install the certs in.         |
| MENDER_ENV          | dev, prod | dev         | Install certs for the prod or dev environment. |

## Installing

```bash
cd build
cmake --build . --target install
```


## License
Copyright 2018-2019 Xaptum, Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this work except in compliance with the License. You may obtain a copy of
the License from the LICENSE.txt file or at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.
