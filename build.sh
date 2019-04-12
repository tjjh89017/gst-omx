#!/bin/bash

./configure --target=aarch64-unknown-linux-gnueabi  CFLAGS=-D__LINUX_MEDIA_NAS__ --with-omx-header-path=$PWD/omx/khronos --with-omx-target=generic


