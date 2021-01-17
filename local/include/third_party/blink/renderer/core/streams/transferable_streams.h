// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Functions used to build transferable streams.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_TRANSFERABLE_STREAMS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_TRANSFERABLE_STREAMS_H_

#include <memory>

#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

class ExceptionState;
class MessagePort;
class ReadableStream;
class ReadableStreamTransferringOptimizer;
class ScriptState;
class UnderlyingSourceBase;
class WritableStream;

// Creates the writable side of a cross-realm identity transform stream, using
// |port| for communication. |port| must be entangled with another MessagePort
// which is passed to CreateCrossRealmTransformReadable().
// Equivalent to SetUpCrossRealmTransformWritable in the standard:
// https://streams.spec.whatwg.org/#abstract-opdef-setupcrossrealmtransformwritable
CORE_EXPORT WritableStream* CreateCrossRealmTransformWritable(ScriptState*,
                                                              MessagePort* port,
                                                              ExceptionState&);

// Creates the readable side of a cross-realm identity transform stream. |port|
// is used symmetrically with CreateCrossRealmTransformWritable().
// Equivalent to SetUpCrossRealmTransformReadable in the standard:
// https://streams.spec.whatwg.org/#abstract-opdef-setupcrossrealmtransformreadable
CORE_EXPORT ReadableStream* CreateCrossRealmTransformReadable(
    ScriptState*,
    MessagePort* port,
    std::unique_ptr<ReadableStreamTransferringOptimizer> optimizer,
    ExceptionState&);

// Creates a ReadableStream that is identical to the concatenation of
// a ReadableStream created with `source1` and a ReadableStream created with
// `source2`.
// The implementation is optimized with an assumption that `source2` is (much)
// longer than `source1`.
CORE_EXPORT ReadableStream* CreateConcatenatedReadableStream(
    ScriptState*,
    UnderlyingSourceBase* source1,
    UnderlyingSourceBase* source2);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_TRANSFERABLE_STREAMS_H_
