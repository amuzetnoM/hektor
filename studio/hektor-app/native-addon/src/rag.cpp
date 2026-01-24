#include "rag.h"
#include "database.h"
#include "vdb/framework_integration.hpp"

#ifdef VDB_USE_LLAMA_CPP
#include "vdb/llm/llm_engine.hpp"
#endif

namespace hektor_native {

// ============================================================================
// RAGEngine Wrapper Implementation
// ============================================================================

Napi::Object RAGEngineWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "RAGEngine", {
        InstanceMethod("chunkDocument", &RAGEngineWrap::ChunkDocument),
        InstanceMethod("buildContext", &RAGEngineWrap::BuildContext),
        InstanceMethod("formatPrompt", &RAGEngineWrap::FormatPrompt),
        InstanceMethod("rerank", &RAGEngineWrap::Rerank),
    });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    exports.Set("RAGEngine", func);
    return exports;
}

RAGEngineWrap::RAGEngineWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<RAGEngineWrap>(info) {
    Napi::Env env = info.Env();
    
    vdb::framework::RAGConfig config;
    if (info.Length() > 0 && info[0].IsObject()) {
        Napi::Object opts = info[0].As<Napi::Object>();
        if (opts.Has("chunkSize")) config.chunk_size = opts.Get("chunkSize").As<Napi::Number>().Uint32Value();
        if (opts.Has("chunkOverlap")) config.chunk_overlap = opts.Get("chunkOverlap").As<Napi::Number>().Uint32Value();
        if (opts.Has("maxContextLength")) config.max_context_length = opts.Get("maxContextLength").As<Napi::Number>().Uint32Value();
    }
    
    engine_ = std::make_unique<vdb::framework::RAGEngine>(config);
}

Napi::Value RAGEngineWrap::ChunkDocument(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected document text").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string document = info[0].As<Napi::String>().Utf8Value();
    auto result = engine_->chunk_document(document);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array chunks = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        chunks.Set(static_cast<uint32_t>(i), Napi::String::New(env, result.value()[i]));
    }
    
    return chunks;
}

Napi::Value RAGEngineWrap::BuildContext(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected query and search results").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string query = info[0].As<Napi::String>().Utf8Value();
    
    std::vector<vdb::QueryResult> search_results;
    if (info[1].IsArray()) {
        Napi::Array arr = info[1].As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); i++) {
            Napi::Object obj = arr.Get(i).As<Napi::Object>();
            vdb::QueryResult r;
            r.id = obj.Get("id").As<Napi::Number>().Int64Value();
            r.distance = obj.Has("distance") ? obj.Get("distance").As<Napi::Number>().FloatValue() : 0.0f;
            r.score = obj.Has("score") ? obj.Get("score").As<Napi::Number>().FloatValue() : 1.0f;
            search_results.push_back(r);
        }
    }
    
    auto result = engine_->build_context(query, search_results);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    const auto& ctx = result.value();
    Napi::Object context = Napi::Object::New(env);
    context.Set("query", Napi::String::New(env, ctx.query));
    
    Napi::Array docs = Napi::Array::New(env, ctx.documents.size());
    for (size_t i = 0; i < ctx.documents.size(); i++) {
        docs.Set(static_cast<uint32_t>(i), Napi::String::New(env, ctx.documents[i]));
    }
    context.Set("documents", docs);
    context.Set("totalTokens", Napi::Number::New(env, ctx.total_tokens));
    
    return context;
}

Napi::Value RAGEngineWrap::FormatPrompt(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected query and context").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string query = info[0].As<Napi::String>().Utf8Value();
    
    vdb::framework::RAGContext context;
    if (info[1].IsObject()) {
        Napi::Object obj = info[1].As<Napi::Object>();
        context.query = obj.Has("query") ? obj.Get("query").As<Napi::String>().Utf8Value() : query;
        
        if (obj.Has("documents") && obj.Get("documents").IsArray()) {
            Napi::Array docs = obj.Get("documents").As<Napi::Array>();
            for (uint32_t i = 0; i < docs.Length(); i++) {
                context.documents.push_back(docs.Get(i).As<Napi::String>().Utf8Value());
            }
        }
    }
    
    std::string system_prompt = "";
    if (info.Length() > 2 && info[2].IsString()) {
        system_prompt = info[2].As<Napi::String>().Utf8Value();
    }
    
    std::string prompt = engine_->format_prompt(query, context, system_prompt);
    return Napi::String::New(env, prompt);
}

Napi::Value RAGEngineWrap::Rerank(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected query and results").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string query = info[0].As<Napi::String>().Utf8Value();
    
    std::vector<vdb::QueryResult> results;
    if (info[1].IsArray()) {
        Napi::Array arr = info[1].As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); i++) {
            Napi::Object obj = arr.Get(i).As<Napi::Object>();
            vdb::QueryResult r;
            r.id = obj.Get("id").As<Napi::Number>().Int64Value();
            r.distance = obj.Has("distance") ? obj.Get("distance").As<Napi::Number>().FloatValue() : 0.0f;
            r.score = obj.Has("score") ? obj.Get("score").As<Napi::Number>().FloatValue() : 1.0f;
            results.push_back(r);
        }
    }
    
    auto result = engine_->rerank(query, results);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array reranked = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        const auto& r = result.value()[i];
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("id", Napi::Number::New(env, r.id));
        obj.Set("distance", Napi::Number::New(env, r.distance));
        obj.Set("score", Napi::Number::New(env, r.score));
        reranked.Set(static_cast<uint32_t>(i), obj);
    }
    
    return reranked;
}

// ============================================================================
// LLMEngine Wrapper Implementation
// ============================================================================

Napi::Object LLMEngineWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LLMEngine", {
        InstanceMethod("load", &LLMEngineWrap::Load),
        InstanceMethod("isLoaded", &LLMEngineWrap::IsLoaded),
        InstanceMethod("unload", &LLMEngineWrap::Unload),
        InstanceMethod("modelName", &LLMEngineWrap::ModelName),
        InstanceMethod("contextSize", &LLMEngineWrap::ContextSize),
        InstanceMethod("vocabSize", &LLMEngineWrap::VocabSize),
        InstanceMethod("generate", &LLMEngineWrap::Generate),
        InstanceMethod("chat", &LLMEngineWrap::Chat),
        InstanceMethod("countTokens", &LLMEngineWrap::CountTokens),
        InstanceMethod("embed", &LLMEngineWrap::Embed),
    });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    exports.Set("LLMEngine", func);
    return exports;
}

LLMEngineWrap::LLMEngineWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LLMEngineWrap>(info) {
#ifdef VDB_USE_LLAMA_CPP
    engine_ = std::make_unique<vdb::llm::LLMEngine>();
#endif
}

Napi::Value LLMEngineWrap::Load(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
#ifdef VDB_USE_LLAMA_CPP
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected model path").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string path = info[0].As<Napi::String>().Utf8Value();
    
    vdb::llm::LLMConfig config;
    if (info.Length() > 1 && info[1].IsObject()) {
        Napi::Object opts = info[1].As<Napi::Object>();
        if (opts.Has("contextSize")) config.context_size = opts.Get("contextSize").As<Napi::Number>().Uint32Value();
        if (opts.Has("gpuLayers")) config.gpu_layers = opts.Get("gpuLayers").As<Napi::Number>().Int32Value();
        if (opts.Has("threads")) config.threads = opts.Get("threads").As<Napi::Number>().Uint32Value();
    }
    
    auto result = engine_->load(path, config);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
#else
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", false);
    response.Set("error", "LLM support not compiled (VDB_USE_LLAMA_CPP=OFF)");
    return response;
#endif
}

Napi::Value LLMEngineWrap::IsLoaded(const Napi::CallbackInfo& info) {
#ifdef VDB_USE_LLAMA_CPP
    return Napi::Boolean::New(info.Env(), engine_ && engine_->is_loaded());
#else
    return Napi::Boolean::New(info.Env(), false);
#endif
}

Napi::Value LLMEngineWrap::Unload(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
#ifdef VDB_USE_LLAMA_CPP
    engine_->unload();
#endif
    return Napi::Boolean::New(env, true);
}

Napi::Value LLMEngineWrap::ModelName(const Napi::CallbackInfo& info) {
#ifdef VDB_USE_LLAMA_CPP
    return Napi::String::New(info.Env(), engine_->model_name());
#else
    return Napi::String::New(info.Env(), "");
#endif
}

Napi::Value LLMEngineWrap::ContextSize(const Napi::CallbackInfo& info) {
#ifdef VDB_USE_LLAMA_CPP
    return Napi::Number::New(info.Env(), engine_->context_size());
#else
    return Napi::Number::New(info.Env(), 0);
#endif
}

Napi::Value LLMEngineWrap::VocabSize(const Napi::CallbackInfo& info) {
#ifdef VDB_USE_LLAMA_CPP
    return Napi::Number::New(info.Env(), engine_->vocab_size());
#else
    return Napi::Number::New(info.Env(), 0);
#endif
}

Napi::Value LLMEngineWrap::Generate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
#ifdef VDB_USE_LLAMA_CPP
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected prompt").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string prompt = info[0].As<Napi::String>().Utf8Value();
    
    vdb::llm::GenerationParams params;
    if (info.Length() > 1 && info[1].IsObject()) {
        Napi::Object opts = info[1].As<Napi::Object>();
        if (opts.Has("maxTokens")) params.max_tokens = opts.Get("maxTokens").As<Napi::Number>().Uint32Value();
        if (opts.Has("temperature")) params.temperature = opts.Get("temperature").As<Napi::Number>().FloatValue();
        if (opts.Has("topP")) params.top_p = opts.Get("topP").As<Napi::Number>().FloatValue();
        if (opts.Has("topK")) params.top_k = opts.Get("topK").As<Napi::Number>().Uint32Value();
    }
    
    auto result = engine_->generate(prompt, params);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return Napi::String::New(env, result.value());
#else
    Napi::Error::New(env, "LLM support not compiled").ThrowAsJavaScriptException();
    return env.Null();
#endif
}

Napi::Value LLMEngineWrap::Chat(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
#ifdef VDB_USE_LLAMA_CPP
    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Expected messages array").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::vector<vdb::llm::ChatMessage> messages;
    Napi::Array arr = info[0].As<Napi::Array>();
    for (uint32_t i = 0; i < arr.Length(); i++) {
        Napi::Object msg = arr.Get(i).As<Napi::Object>();
        vdb::llm::ChatMessage m;
        m.role = msg.Get("role").As<Napi::String>().Utf8Value();
        m.content = msg.Get("content").As<Napi::String>().Utf8Value();
        messages.push_back(m);
    }
    
    vdb::llm::GenerationParams params;
    if (info.Length() > 1 && info[1].IsObject()) {
        Napi::Object opts = info[1].As<Napi::Object>();
        if (opts.Has("maxTokens")) params.max_tokens = opts.Get("maxTokens").As<Napi::Number>().Uint32Value();
        if (opts.Has("temperature")) params.temperature = opts.Get("temperature").As<Napi::Number>().FloatValue();
    }
    
    auto result = engine_->chat(messages, params);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return Napi::String::New(env, result.value());
#else
    Napi::Error::New(env, "LLM support not compiled").ThrowAsJavaScriptException();
    return env.Null();
#endif
}

Napi::Value LLMEngineWrap::CountTokens(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
#ifdef VDB_USE_LLAMA_CPP
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected text").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string text = info[0].As<Napi::String>().Utf8Value();
    return Napi::Number::New(env, engine_->count_tokens(text));
#else
    return Napi::Number::New(env, 0);
#endif
}

Napi::Value LLMEngineWrap::Embed(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
#ifdef VDB_USE_LLAMA_CPP
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected text").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string text = info[0].As<Napi::String>().Utf8Value();
    auto result = engine_->embed(text);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    return VectorToJS(env, result.value());
#else
    Napi::Error::New(env, "LLM support not compiled").ThrowAsJavaScriptException();
    return env.Null();
#endif
}

// ============================================================================
// Placeholder implementations for adapters (LangChain, LlamaIndex)
// ============================================================================

Napi::Object LangChainAdapterWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LangChainAdapter", {
        InstanceMethod("similaritySearch", &LangChainAdapterWrap::SimilaritySearch),
        InstanceMethod("similaritySearchWithScore", &LangChainAdapterWrap::SimilaritySearchWithScore),
        InstanceMethod("maxMarginalRelevanceSearch", &LangChainAdapterWrap::MaxMarginalRelevanceSearch),
        InstanceMethod("asRetriever", &LangChainAdapterWrap::AsRetriever),
    });
    
    exports.Set("LangChainAdapter", func);
    return exports;
}

LangChainAdapterWrap::LangChainAdapterWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LangChainAdapterWrap>(info) {}

Napi::Value LangChainAdapterWrap::SimilaritySearch(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value LangChainAdapterWrap::SimilaritySearchWithScore(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value LangChainAdapterWrap::MaxMarginalRelevanceSearch(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value LangChainAdapterWrap::AsRetriever(const Napi::CallbackInfo& info) {
    return Napi::Object::New(info.Env());
}

Napi::Object LlamaIndexAdapterWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "LlamaIndexAdapter", {
        InstanceMethod("buildIndex", &LlamaIndexAdapterWrap::BuildIndex),
        InstanceMethod("query", &LlamaIndexAdapterWrap::Query),
        InstanceMethod("insert", &LlamaIndexAdapterWrap::Insert),
        InstanceMethod("deleteDocument", &LlamaIndexAdapterWrap::DeleteDocument),
        InstanceMethod("save", &LlamaIndexAdapterWrap::Save),
        StaticMethod("load", &LlamaIndexAdapterWrap::Load),
    });
    
    exports.Set("LlamaIndexAdapter", func);
    return exports;
}

LlamaIndexAdapterWrap::LlamaIndexAdapterWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<LlamaIndexAdapterWrap>(info) {}

Napi::Value LlamaIndexAdapterWrap::BuildIndex(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", true);
    return result;
}

Napi::Value LlamaIndexAdapterWrap::Query(const Napi::CallbackInfo& info) {
    return Napi::Array::New(info.Env());
}

Napi::Value LlamaIndexAdapterWrap::Insert(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", true);
    return result;
}

Napi::Value LlamaIndexAdapterWrap::DeleteDocument(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", true);
    return result;
}

Napi::Value LlamaIndexAdapterWrap::Save(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", true);
    return result;
}

Napi::Value LlamaIndexAdapterWrap::Load(const Napi::CallbackInfo& info) {
    Napi::Object result = Napi::Object::New(info.Env());
    result.Set("success", true);
    return result;
}

// ============================================================================
// DocumentChunker Implementation
// ============================================================================

Napi::Object DocumentChunkerWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "DocumentChunker", {
        InstanceMethod("chunk", &DocumentChunkerWrap::Chunk),
        InstanceMethod("chunkWithMetadata", &DocumentChunkerWrap::ChunkWithMetadata),
    });
    
    exports.Set("DocumentChunker", func);
    return exports;
}

DocumentChunkerWrap::DocumentChunkerWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<DocumentChunkerWrap>(info) {
    chunker_ = std::make_unique<vdb::framework::DocumentChunker>();
}

Napi::Value DocumentChunkerWrap::Chunk(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected document text").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string document = info[0].As<Napi::String>().Utf8Value();
    auto result = chunker_->chunk(document);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array chunks = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        chunks.Set(static_cast<uint32_t>(i), Napi::String::New(env, result.value()[i]));
    }
    
    return chunks;
}

Napi::Value DocumentChunkerWrap::ChunkWithMetadata(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected document text").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string document = info[0].As<Napi::String>().Utf8Value();
    auto result = chunker_->chunk_with_metadata(document);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array chunks = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        const auto& c = result.value()[i];
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("text", Napi::String::New(env, c.text));
        obj.Set("startPos", Napi::Number::New(env, c.start_pos));
        obj.Set("endPos", Napi::Number::New(env, c.end_pos));
        obj.Set("chunkIndex", Napi::Number::New(env, c.chunk_index));
        chunks.Set(static_cast<uint32_t>(i), obj);
    }
    
    return chunks;
}

// ============================================================================
// RAG Utility Functions
// ============================================================================

void RAGUtils::Init(Napi::Env env, Napi::Object& exports) {
    exports.Set("findGgufModels", Napi::Function::New(env, FindGgufModels));
    exports.Set("readGgufMetadata", Napi::Function::New(env, ReadGgufMetadata));
    exports.Set("applyChatTemplate", Napi::Function::New(env, ApplyChatTemplate));
}

Napi::Value RAGUtils::FindGgufModels(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // Return empty array - actual implementation would scan directories
    return Napi::Array::New(env);
}

Napi::Value RAGUtils::ReadGgufMetadata(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // Return empty object - actual implementation would read GGUF file
    return Napi::Object::New(env);
}

Napi::Value RAGUtils::ApplyChatTemplate(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        return Napi::String::New(env, "");
    }
    
    // Simple passthrough - actual implementation would format messages
    if (info[0].IsArray()) {
        Napi::Array msgs = info[0].As<Napi::Array>();
        std::string result;
        for (uint32_t i = 0; i < msgs.Length(); i++) {
            Napi::Object msg = msgs.Get(i).As<Napi::Object>();
            std::string role = msg.Get("role").As<Napi::String>().Utf8Value();
            std::string content = msg.Get("content").As<Napi::String>().Utf8Value();
            result += "<|" + role + "|>\n" + content + "\n";
        }
        return Napi::String::New(env, result);
    }
    
    return Napi::String::New(env, "");
}

} // namespace hektor_native
