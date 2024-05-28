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

# Path to the core_count.hpp file
FILE_PATH="include/rio/core_count.hpp"

# Check if the file exists
if [ ! -f "$FILE_PATH" ]; then
    echo "File does not exist: $FILE_PATH"
    exit 1
fi

# Update the constexpr line with the new core count
awk -v count="$core_count" '{
    if ($0 ~ /^constexpr std::size_t hardware_concurrency = /) {
        print "constexpr std::size_t hardware_concurrency = " count ";"
    } else {
        print $0
    }
}' $FILE_PATH > $FILE_PATH.temp

# Replace the old file with the new file
mv $FILE_PATH.temp $FILE_PATH
