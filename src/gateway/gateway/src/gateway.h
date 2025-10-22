/********************************************************************************
 * Copyright (c) 2025 Elektrobit Automotive GmbH
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

#ifndef SCORE_MW_COM_IPC_BRIDGE_GATEWAY_H
#define SCORE_MW_COM_IPC_BRIDGE_GATEWAY_H

#include "dlopen.hpp"

#include <score/gateway/someip_plugin_interface.hpp>
#include <score/socom/result.hpp>
#include <score/socom/runtime.hpp>

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

namespace score::gateway {

class Gateway {
    socom::Runtime::Uptr runtime_;
    Dlopen::Uptr dlopen_;
    Someip_network_plugin_interface::Uptr network_plugin_;

    Gateway(socom::Runtime::Uptr runtime, Dlopen::Uptr dlopen,
            Someip_network_plugin_interface::Uptr plugin);

public:
    using Create_result = socom::Result<Gateway, std::string>;

    static Create_result create(std::string const& plugin_path,
                                std::string_view const& network_interface,
                                std::string_view const& ip_address,
                                std::vector<std::string> const& manifests);

    int run(const std::chrono::milliseconds cycle_time, const std::size_t num_cycles);
};

} // namespace score::gateway

#endif // SCORE_MW_COM_IPC_BRIDGE_SAMPLE_SENDER_RECEIVER_H
