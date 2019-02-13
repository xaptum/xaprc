# etc-hosts

This package installs the /etc/hosts for the router card.

## CMake Options

The following CMake configuration options are supported.

| Option                 | Values    | Default | Description                                         |
|------------------------|-----------|---------|-----------------------------------------------------|
| ETC_HOSTS_INSTALL_PATH |           | /etc    | The directory to install the hosts file in.         |
| ETC_HOSTS_ENV          | dev, prod | dev     | Install hosts file for the prod or dev environment. |

## Installing

```bash
cd build
cmake --build . --target install
```
