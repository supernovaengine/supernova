// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "AiTypes.h"

#include <chrono>
#include <string>

namespace doriax::editor::ai {

// True only for a temporary HTTP 429. Permanent billing/quota failures use
// the same status but must be shown immediately instead of retried.
bool isRetryableRateLimit(long status, const ProviderResponse& error);

// Uses the longest precise hint from Retry-After, structured
// google.rpc.RetryInfo, and the provider's "try again in ..." detail.
// Rate-limit reset headers are a fallback because they commonly describe
// time until a bucket is full, not the earliest successful retry.
std::chrono::milliseconds rateLimitRetryDelay(
    const HttpResponse& response, const std::string& detail,
    const std::string& structuredRetryDelay = {});

} // namespace doriax::editor::ai
