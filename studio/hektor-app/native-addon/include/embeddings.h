#pragma once

#include <napi.h>
#include "common.h"
#include <memory>

// Only include ONNX-dependent headers if ONNX Runtime is available
#ifdef VDB_USE_ONNX_RUNTIME
#include <vdb/embeddings/text.hpp>
#include <vdb/embeddings/image.hpp>
#include <vdb/embeddings/onnx_runtime.hpp>
#endif

namespace hektor_native {

#ifdef VDB_USE_ONNX_RUNTIME

// TextEncoder wrapper
class TextEncoderWrap : public Napi::ObjectWrap<TextEncoderWrap> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    TextEncoderWrap(const Napi::CallbackInfo& info);
    
private:
    Napi::Value Init(const Napi::CallbackInfo& info);
    Napi::Value IsReady(const Napi::CallbackInfo& info);
    Napi::Value Encode(const Napi::CallbackInfo& info);
    Napi::Value EncodeBatch(const Napi::CallbackInfo& info);
    Napi::Value Dimension(const Napi::CallbackInfo& info);
    Napi::Value Device(const Napi::CallbackInfo& info);
    
    std::unique_ptr<vdb::embeddings::TextEncoder> encoder_;
};

// ImageEncoder wrapper
class ImageEncoderWrap : public Napi::ObjectWrap<ImageEncoderWrap> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    ImageEncoderWrap(const Napi::CallbackInfo& info);
    
private:
    Napi::Value Init(const Napi::CallbackInfo& info);
    Napi::Value IsReady(const Napi::CallbackInfo& info);
    Napi::Value Encode(const Napi::CallbackInfo& info);
    Napi::Value EncodePath(const Napi::CallbackInfo& info);
    Napi::Value EncodeBatch(const Napi::CallbackInfo& info);
    Napi::Value Dimension(const Napi::CallbackInfo& info);
    Napi::Value Device(const Napi::CallbackInfo& info);
    
    std::unique_ptr<vdb::embeddings::ImageEncoder> encoder_;
};

// OnnxSession wrapper
class OnnxSessionWrap : public Napi::ObjectWrap<OnnxSessionWrap> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    OnnxSessionWrap(const Napi::CallbackInfo& info);
    
private:
    Napi::Value Run(const Napi::CallbackInfo& info);
    Napi::Value Device(const Napi::CallbackInfo& info);
    Napi::Value InputNames(const Napi::CallbackInfo& info);
    Napi::Value OutputNames(const Napi::CallbackInfo& info);
    Napi::Value InputShape(const Napi::CallbackInfo& info);
    Napi::Value OutputShape(const Napi::CallbackInfo& info);
    
    std::unique_ptr<vdb::embeddings::OnnxSession> session_;
};

// Tokenizer wrapper
class TokenizerWrap : public Napi::ObjectWrap<TokenizerWrap> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    TokenizerWrap(const Napi::CallbackInfo& info);
    
private:
    Napi::Value Encode(const Napi::CallbackInfo& info);
    Napi::Value Decode(const Napi::CallbackInfo& info);
    Napi::Value VocabSize(const Napi::CallbackInfo& info);
    Napi::Value HasToken(const Napi::CallbackInfo& info);
    
    std::unique_ptr<vdb::embeddings::Tokenizer> tokenizer_;
};

// ImagePreprocessor wrapper
class ImagePreprocessorWrap : public Napi::ObjectWrap<ImagePreprocessorWrap> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    ImagePreprocessorWrap(const Napi::CallbackInfo& info);
    
private:
    Napi::Value Process(const Napi::CallbackInfo& info);
    Napi::Value ProcessFile(const Napi::CallbackInfo& info);
    Napi::Value CenterCropAndProcess(const Napi::CallbackInfo& info);
    Napi::Value TargetSize(const Napi::CallbackInfo& info);
    Napi::Value OutputSize(const Napi::CallbackInfo& info);
    
    std::unique_ptr<vdb::embeddings::ImagePreprocessor> preprocessor_;
};

#else // VDB_USE_ONNX_RUNTIME not defined

// Stub implementations when ONNX Runtime is not available
namespace embeddings_stub {
    inline void InitStubs(Napi::Env env, Napi::Object& exports) {
        // Export a flag indicating embeddings are not available
        exports.Set("embeddingsAvailable", Napi::Boolean::New(env, false));
    }
}

#endif // VDB_USE_ONNX_RUNTIME

} // namespace hektor_native
