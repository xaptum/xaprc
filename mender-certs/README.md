# mender-certs

This package installs the public certificates used by the mender client to

- authenticate the Xaptum mender server when establishing a TLS connection
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
