#include "framework.h"

namespace hektor_native {

// ============================================================================
// TensorFlowEmbedder Wrapper - Optional Feature
// ============================================================================

Napi::Object TensorFlowEmbedderWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TensorFlowEmbedder", {
        InstanceMethod("embed", &TensorFlowEmbedderWrap::Embed),
        InstanceMethod("embedBatch", &TensorFlowEmbedderWrap::EmbedBatch),
        InstanceMethod("dimension", &TensorFlowEmbedderWrap::Dimension),
        InstanceMethod("isLoaded", &TensorFlowEmbedderWrap::IsLoaded),
        StaticMethod("exportForTraining", &TensorFlowEmbedderWrap::ExportForTraining),
    });
    
    exports.Set("TensorFlowEmbedder", func);
    return exports;
}

TensorFlowEmbedderWrap::TensorFlowEmbedderWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TensorFlowEmbedderWrap>(info) {}

Napi::Value TensorFlowEmbedderWrap::Embed(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value TensorFlowEmbedderWrap::EmbedBatch(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value TensorFlowEmbedderWrap::Dimension(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 0);
}

Napi::Value TensorFlowEmbedderWrap::IsLoaded(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), false);
}

Napi::Value TensorFlowEmbedderWrap::ExportForTraining(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", false);
    result.Set("error", "TensorFlow support not compiled");
    return result;
}

// ============================================================================
// PyTorchEmbedder Wrapper - Optional Feature
// ============================================================================

Napi::Object PyTorchEmbedderWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "PyTorchEmbedder", {
        InstanceMethod("embed", &PyTorchEmbedderWrap::Embed),
        InstanceMethod("embedBatch", &PyTorchEmbedderWrap::EmbedBatch),
        InstanceMethod("dimension", &PyTorchEmbedderWrap::Dimension),
        InstanceMethod("isLoaded", &PyTorchEmbedderWrap::IsLoaded),
        StaticMethod("exportForTraining", &PyTorchEmbedderWrap::ExportForTraining),
        StaticMethod("fromTrained", &PyTorchEmbedderWrap::FromTrained),
    });
    
    exports.Set("PyTorchEmbedder", func);
    return exports;
}

PyTorchEmbedderWrap::PyTorchEmbedderWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<PyTorchEmbedderWrap>(info) {}

Napi::Value PyTorchEmbedderWrap::Embed(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value PyTorchEmbedderWrap::EmbedBatch(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value PyTorchEmbedderWrap::Dimension(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 0);
}

Napi::Value PyTorchEmbedderWrap::IsLoaded(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), false);
}

Napi::Value PyTorchEmbedderWrap::ExportForTraining(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", false);
    result.Set("error", "PyTorch support not compiled");
    return result;
}

Napi::Value PyTorchEmbedderWrap::FromTrained(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", false);
    result.Set("error", "PyTorch support not compiled");
    return result;
}

// ============================================================================
// ONNXEmbedder Wrapper - Primary Embedding Support
// ============================================================================

Napi::Object ONNXEmbedderWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ONNXEmbedder", {
        InstanceMethod("embed", &ONNXEmbedderWrap::Embed),
        InstanceMethod("embedBatch", &ONNXEmbedderWrap::EmbedBatch),
        InstanceMethod("dimension", &ONNXEmbedderWrap::Dimension),
        InstanceMethod("isLoaded", &ONNXEmbedderWrap::IsLoaded),
    });
    
    exports.Set("ONNXEmbedder", func);
    return exports;
}

ONNXEmbedderWrap::ONNXEmbedderWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ONNXEmbedderWrap>(info) {}

Napi::Value ONNXEmbedderWrap::Embed(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // Return placeholder embedding
    Napi::Array vec = Napi::Array::New(env, 384);
    for (uint32_t i = 0; i < 384; i++) {
        vec.Set(i, Napi::Number::New(env, 0.0));
    }
    return vec;
}

Napi::Value ONNXEmbedderWrap::EmbedBatch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Array::New(env);
}

Napi::Value ONNXEmbedderWrap::Dimension(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), 384);
}

Napi::Value ONNXEmbedderWrap::IsLoaded(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), false);
}

} // namespace hektor_native
