#!/bin/sh

find src/ -name *.h -or -name *.hpp -or -name *.cpp | xargs clang-format -i
