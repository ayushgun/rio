#!/bin/bash

# MIT License
# Copyright (c) 2024 Ayush Gundawar <ayushgundawar (at) gmail (dot) com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# Determine the number of hardware threads based on the operating system
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    core_count=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    core_count=$(sysctl -n hw.ncpu)
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

# Ensure the include/rio directory exists
mkdir -p ../include/rio

echo "#pragma once" > ../include/rio/core_count.hpp
echo "#define HARDWARE_CONCURRENCY $core_count" >> ../include/rio/core_count.hpp
