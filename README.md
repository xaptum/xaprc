### xaprc

This repo contains the Linux firmware and host Linux kernel drivers
for the Xaptum router cards.

The router card itself runs embedded Linux, so the majority of the
firmware is userspace services and configuration.

Services that are useful outside of the router card, like the `enftun`
client, are developed in separate repositories. The kernel driver and
firmware are managed in the same repo because their behaviors are
tightly coupled.

The Buildroot external tree to assemble the full firmware image is
located in a separate
[repo](https://github.com/xaptum/xaptum-buildroot).

## Supported Products

| Model #    | Description                | USB Vendor ID | USB Product ID |
|------------|----------------------------|---------------|----------------|
| XAP-RC-001 | Xaptum Router Card         | 0x2FE0        | 0x8B01         |
| XAP-RW-001 | Xaptum Router Card w/ WiFi | 0x2FE0        | 0x8B02         |

## Organization

Requirements, specifications, and design documents are located on the
project [Wiki](https://github.com/xaptum/xaprc/wiki).

Modifications or questions should be discussed via
[Issues](https://github.com/xaptum/xaprc/issues) on this repo.

New features or modifications should be submitted as [Pull
Requests](https://github.com/xaptum/xaprc/pulls) for code review.

## Related Repositories
| Repo                                                           | Description                                                                                       |
|----------------------------------------------------------------|---------------------------------------------------------------------------------------------------|
| [xaptum-buildroot](https://github.com/xaptum/xaptum-buildroot) | Buildroot external tree for assembling the full firmware image                                    |
| [enftun](https://github.com/xaptum/enftun)                     | Software to manage the `enfX` tunnel interfaces                                                   |
| [xapea00x](https://github.com/xaptum/xapea00x)                 | Kernel driver for the ENF Access cards. Example of best practices for out-of-tree kernel drivers. |

## License
Copyright (c) 2018 Xaptum, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
