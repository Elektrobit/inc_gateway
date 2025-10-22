# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
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

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library")
load("@rules_shell//shell:sh_test.bzl", "sh_test")
load("@score-baselibs//score/language/safecpp:toolchain_features.bzl", "COMPILER_WARNING_FEATURES")

cc_binary(
    name = "ipc_bridge_cpp",
    srcs = [
        "cpp/main.cpp",
        "cpp/sample_sender_receiver.cpp",
        "cpp/sample_sender_receiver.h",
    ],
    features = COMPILER_WARNING_FEATURES,
    visibility = ["//visibility:public"],
    deps = [
        "//src/gateway/common",
        "@boost.program_options",
        "@communication//score/mw/com",
        "@score-baselibs//score/language/futurecpp",
    ],
)

rust_library(
    name = "ipc_bridge_gen_rs",
    srcs = ["rust/ipc_bridge_gen.rs"],
    deps = [
        ":ipc_bridge_gen_cpp",
        "@communication//score/mw/com/impl/rust:mw_com",
        "@crate_index//:libc",
    ],
)

rust_binary(
    name = "ipc_bridge_rs",
    srcs = ["rust/ipc_bridge.rs"],
    features = ["link_std_cpp_lib"],
    rustc_flags = ["-Clink-arg=-lstdc++"],
    visibility = ["//visibility:public"],
    deps = [
        ":ipc_bridge_gen_rs",
        "@communication//score/mw/com/impl/rust:mw_com",
        "@crate_index//:clap",
        "@crate_index//:futures",
    ],
)

cc_library(
    name = "ipc_bridge_gen_cpp",
    srcs = ["rust/ipc_bridge_gen.cpp"],
    features = COMPILER_WARNING_FEATURES,
    implementation_deps = [
        "//src/gateway/common",
        "@communication//score/mw/com/impl/rust:bridge_macros",
    ],
)

# Rust code dumps core when run in parallel with unit tests
sh_test(
    name = "integration_test_ipc_bridge",
    srcs = [
        "test_ipc_bridge.sh",
    ],
    data = [
        ":ipc_bridge_cpp",
        ":ipc_bridge_rs",
        "//src/gateway/common:common.sh",
    ],
    tags = ["exclusive"],
)
