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

#ifndef SCORE_GATEWAY_PAYLOAD_TRANSFORMATION_INTERFACE_H
#define SCORE_GATEWAY_PAYLOAD_TRANSFORMATION_INTERFACE_H

#include <memory>

namespace score::socom {
class Runtime;
}

namespace score::gateway {

class Payload_transformation_plugin_interface;
using Payload_transformation_plugin_deleter = void (*)(Payload_transformation_plugin_interface*);

// TODO it might make sense to have a pure C interface so that e.g. Rust can also implement the
// plugin interface, but this requires socom to have a C interface as well
class Payload_transformation_plugin_interface {
public:
    using Uptr = std::unique_ptr<Payload_transformation_plugin_interface,
                                 Payload_transformation_plugin_deleter>;

    // default destructor needs to be in cpp file to work around ODR violations when dlopen()
    // plugins
    virtual ~Payload_transformation_plugin_interface();
};

/// \brief Signature of the plugin main function.
// It could be anything else, like a factory.
using Payload_transformation_plugin_factory =
    Payload_transformation_plugin_interface::Uptr (*)(::score::socom::Runtime&);

} // namespace score::gateway

#endif // SCORE_MW_COM_IPC_BRIDGE_SAMPLE_SENDER_RECEIVER_H
