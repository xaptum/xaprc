FROM debian:stretch-20190228
MAINTAINER Xaptum

ARG DEBIAN_FRONTEND=noninteractive

RUN echo "deb http://ftp.debian.org/debian stretch-backports main" >> /etc/apt/sources.list

RUN apt-get update                                         &&  \
    apt-get install -y --no-install-recommends                 \
      apt-utils                                                \
      git                                                      \
      gcc g++                                                  \
      make                                                     \
      cmake                                                    \
      libevent-dev libjansson-dev                              \
      python3 python3-dev python3-pip                          \
      python3-setuptools python3-wheel                         \
      python3-venv                                         &&  \
    apt-get clean                                          &&  \
    rm -rf /var/lib/apt/lists/*                            &&  \
    rm -rf /var/cache/apt/{pkgcache,srcpkgcache}.bin
