#!/bin/bash

# *******************************************************************************
# Copyright (c) 2025 Elektrobit Automotive GmbH
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

set -euxo pipefail

common_location=$(find * -name common.sh | head -n 1)

. "$common_location"

# this cannot work if both sender and receiver expect 20 samples
# the sender has to send infinite samples and the receiver has to terminate after having received some
CPP_EXAMPLE_CMD="$(find * -name ipc_bridge_cpp) -s $MANIFEST_LOCATION --cycle-time 10"
RUST_EXAMPLE_CMD="$(find * -name ipc_bridge_rs) -s $MANIFEST_LOCATION --cycle-time 10"

setup

echo -e "\n\n\nRunning Rust receiver and Rust sender"
run_receiver_sender "$RUST_EXAMPLE_CMD" "$RUST_EXAMPLE_CMD"

echo -e "\n\n\nRunning C++ receiver and C++ sender"
run_receiver_sender "$CPP_EXAMPLE_CMD" "$CPP_EXAMPLE_CMD"

echo -e "\n\n\nRunning Rust receiver and C++ sender"
run_receiver_sender "$RUST_EXAMPLE_CMD" "$CPP_EXAMPLE_CMD"

echo -e "\n\n\nRunning C++ receiver and Rust sender"
run_receiver_sender "$CPP_EXAMPLE_CMD" "$RUST_EXAMPLE_CMD"
