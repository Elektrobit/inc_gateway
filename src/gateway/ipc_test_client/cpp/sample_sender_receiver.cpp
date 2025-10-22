/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#include "sample_sender_receiver.h"

#include "score/concurrency/notification.h"
#include "score/mw/com/impl/handle_type.h"
#include "score/mw/com/impl/proxy_event.h"
#include <score/assert.hpp>
#include <score/gateway/common.h>
#include <score/gateway/datatype.h>
#include <score/hash.hpp>
#include <score/optional.hpp>

#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <type_traits>
#include <utility>

using namespace std::chrono_literals;
using score::mw::com::InstanceSpecifier;
using score::mw::com::SamplePtr;
using score::mw::com::impl::HandleType;
using score::mw::com::impl::ProxyEvent;
using score::mw::com::impl::ServiceHandleContainer;

namespace score::gateway {

namespace {

class SampleReceiver {
public:
    explicit SampleReceiver(const InstanceSpecifier& instance_specifier)
        : instance_specifier_{instance_specifier}, last_received_{}, received_{0U}
    {
    }

    void ReceiveSample(const MapApiLanesStamped& map) noexcept
    {
        std::cout << ToString(instance_specifier_, ": Received sample: ", map.x, ", ",
                              map.string_data.data(), "\n");

        if (CheckReceivedSample(map)) {
            received_ += 1U;
        }
        last_received_ = map.x;
    }

    std::size_t GetReceivedSampleCount() const noexcept { return received_; }

private:
    bool CheckReceivedSample(const MapApiLanesStamped& map) const noexcept
    {
        if (last_received_.has_value()) {
            if (map.x <= last_received_.value()) {
                std::cerr << ToString(instance_specifier_,
                                      ": The received sample is out of order. Expected that ",
                                      map.x, " > ", last_received_.value(), "\n");
                return false;
            }
        }

        return true;
    }

    const score::mw::com::InstanceSpecifier& instance_specifier_;
    score::cpp::optional<std::uint32_t> last_received_;
    std::size_t received_;
};

score::cpp::optional<std::reference_wrapper<ProxyEvent<MapApiLanesStamped>>>
GetMapApiLanesStampedProxyEvent(IpcBridgeProxy& proxy)
{
    return proxy.map_api_lanes_stamped_;
}

score::Result<HandleType> GetHandleFromSpecifier(const InstanceSpecifier& instance_specifier)
{
    std::cout << ToString(instance_specifier, ": Running as proxy, looking for services\n");
    ServiceHandleContainer<HandleType> handles{};
    do {
        auto handles_result = IpcBridgeProxy::FindService(instance_specifier);
        if (!handles_result.has_value()) {
            return MakeUnexpected<HandleType>(std::move(handles_result.error()));
        }
        handles = std::move(handles_result).value();
        if (handles.size() == 0) {
            std::this_thread::sleep_for(10ms);
        }
    } while (handles.size() == 0);

    std::cout << ToString(instance_specifier, ": Found service, instantiating proxy\n");
    return handles.front();
}

} // namespace

int EventSenderReceiver::RunAsProxy(
    const score::mw::com::InstanceSpecifier& instance_specifier,
    const score::cpp::optional<std::chrono::milliseconds> cycle_time, const std::size_t num_cycles)
{
    constexpr std::size_t SAMPLES_PER_CYCLE = 2U;

    auto handle_result = GetHandleFromSpecifier(instance_specifier);
    if (!handle_result.has_value()) {
        std::cerr << "Unable to find service: " << instance_specifier
                  << ". Failed with error: " << handle_result.error() << ", bailing!\n";
        return EXIT_FAILURE;
    }
    auto handle = handle_result.value();

    auto proxy_result = IpcBridgeProxy::Create(std::move(handle));
    if (!proxy_result.has_value()) {
        std::cerr << "Unable to construct proxy: " << proxy_result.error() << ", bailing!\n";
        return EXIT_FAILURE;
    }
    auto& proxy = proxy_result.value();

    auto map_api_lanes_stamped_event_optional = GetMapApiLanesStampedProxyEvent(proxy);
    if (!map_api_lanes_stamped_event_optional.has_value()) {
        std::cerr << "Could not get MapApiLanesStamped proxy event\n";
        return EXIT_FAILURE;
    }
    auto& map_api_lanes_stamped_event = map_api_lanes_stamped_event_optional.value().get();

    concurrency::Notification event_received;
    if (!cycle_time.has_value()) {
        map_api_lanes_stamped_event.SetReceiveHandler([&event_received, &instance_specifier]() {
            std::cout << ToString(instance_specifier, ": Callback called\n");
            event_received.notify();
        });
    }

    std::cout << ToString(instance_specifier, ": Subscribing to service\n");
    map_api_lanes_stamped_event.Subscribe(SAMPLES_PER_CYCLE);

    score::cpp::optional<char> last_received{};
    SampleReceiver receiver{instance_specifier};
    for (std::size_t cycle = 0U; cycle < num_cycles;) {
        const auto cycle_start_time = std::chrono::steady_clock::now();
        if (cycle_time.has_value()) {
            std::this_thread::sleep_for(*cycle_time);
        }

        const auto received_before = receiver.GetReceivedSampleCount();
        Result<std::size_t> num_samples_received = map_api_lanes_stamped_event.GetNewSamples(
            [&receiver](SamplePtr<MapApiLanesStamped> sample) noexcept {
                // For the GenericProxy case, the void pointer managed by the SamplePtr<void> will
                // be cast to MapApiLanesStamped.
                const MapApiLanesStamped& sample_value = *sample.get();
                receiver.ReceiveSample(sample_value);
            },
            SAMPLES_PER_CYCLE);
        const auto received = receiver.GetReceivedSampleCount() - received_before;

        const bool get_new_samples_api_error = !num_samples_received.has_value();
        const bool mismatch_api_returned_receive_count_vs_sample_callbacks =
            *num_samples_received != received;
        const bool receive_handler_called_without_new_samples =
            *num_samples_received == 0 && !cycle_time.has_value();

        if (get_new_samples_api_error || mismatch_api_returned_receive_count_vs_sample_callbacks ||
            receive_handler_called_without_new_samples) {
            std::stringstream ss;
            ss << instance_specifier << ": Error in cycle " << cycle
               << " during sample reception: ";
            if (!get_new_samples_api_error) {
                if (mismatch_api_returned_receive_count_vs_sample_callbacks) {
                    ss << "number of received samples doesn't match to what IPC claims: "
                       << *num_samples_received << " vs " << received;
                }
                else {
                    ss << "expected at least one new sample, since event-notifier has been called, "
                          "but "
                          "GetNewSamples() didn't provide one! ";
                }
            }
            else {
                ss << std::move(num_samples_received).error();
            }
            ss << ", terminating.\n";
            std::cerr << ss.str();

            map_api_lanes_stamped_event.Unsubscribe();
            return EXIT_FAILURE;
        }

        if (*num_samples_received >= 1U) {
            std::cout << ToString(instance_specifier, ": Proxy received valid data\n");
            cycle += *num_samples_received;
        }

        const auto cycle_duration = std::chrono::steady_clock::now() - cycle_start_time;

        std::cout << ToString(
            instance_specifier, ": Cycle duration ",
            std::chrono::duration_cast<std::chrono::milliseconds>(cycle_duration).count(), "ms\n");

        event_received.reset();
    }

    std::cout << ToString(instance_specifier, ": Unsubscribing...\n");
    map_api_lanes_stamped_event.Unsubscribe();
    std::cout << ToString(instance_specifier, ": and terminating, bye bye\n");
    return EXIT_SUCCESS;
}

int EventSenderReceiver::RunAsSkeleton(const score::mw::com::InstanceSpecifier& instance_specifier,
                                       const std::chrono::milliseconds cycle_time,
                                       const std::size_t num_cycles)
{
    auto create_result = IpcBridgeSkeleton::Create(instance_specifier);
    if (!create_result.has_value()) {
        std::cerr << "Unable to construct skeleton: " << create_result.error() << ", bailing!\n";
        return EXIT_FAILURE;
    }
    auto& skeleton = create_result.value();

    const auto offer_result = skeleton.OfferService();
    if (!offer_result.has_value()) {
        std::cerr << "Unable to offer service for skeleton: " << offer_result.error()
                  << ", bailing!\n";
        return EXIT_FAILURE;
    }
    std::cout << "Starting to send data\n";

    for (std::size_t cycle = 0U; cycle < num_cycles || num_cycles == 0U; ++cycle) {
        auto sample_result = PrepareMapLaneSample(skeleton, cycle);
        if (!sample_result.has_value()) {
            std::cerr << "No sample received. Exiting.\n";
            return EXIT_FAILURE;
        }
        auto sample = std::move(sample_result).value();
        skeleton.map_api_lanes_stamped_.Send(std::move(sample));

        std::this_thread::sleep_for(cycle_time);
    }

    std::cout << "Stop offering service...";
    skeleton.StopOfferService();
    std::cout << "and terminating, bye bye\n";

    return EXIT_SUCCESS;
}

} // namespace score::gateway
