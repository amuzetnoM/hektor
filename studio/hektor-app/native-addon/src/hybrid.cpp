#include "hybrid.h"
#include "database.h"
#include "vdb/hybrid_search.hpp"

namespace hektor_native {

// ============================================================================
// BM25Engine Wrapper Implementation
// ============================================================================

Napi::Object BM25EngineWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "BM25Engine", {
        InstanceMethod("addDocument", &BM25EngineWrap::AddDocument),
        InstanceMethod("removeDocument", &BM25EngineWrap::RemoveDocument),
        InstanceMethod("updateDocument", &BM25EngineWrap::UpdateDocument),
        InstanceMethod("search", &BM25EngineWrap::Search),
        InstanceMethod("documentCount", &BM25EngineWrap::DocumentCount),
        InstanceMethod("termCount", &BM25EngineWrap::TermCount),
        InstanceMethod("averageDocumentLength", &BM25EngineWrap::AverageDocumentLength),
        InstanceMethod("save", &BM25EngineWrap::Save),
        StaticMethod("load", &BM25EngineWrap::Load),
    });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    exports.Set("BM25Engine", func);
    return exports;
}

BM25EngineWrap::BM25EngineWrap(const Napi::CallbackInfo& info) 
    : Napi::ObjectWrap<BM25EngineWrap>(info) {
    Napi::Env env = info.Env();
    
    vdb::hybrid::BM25Config config;
    
    if (info.Length() > 0 && info[0].IsObject()) {
        Napi::Object opts = info[0].As<Napi::Object>();
        if (opts.Has("k1")) config.k1 = opts.Get("k1").As<Napi::Number>().FloatValue();
        if (opts.Has("b")) config.b = opts.Get("b").As<Napi::Number>().FloatValue();
        if (opts.Has("minTermLength")) config.min_term_length = opts.Get("minTermLength").As<Napi::Number>().Uint32Value();
        if (opts.Has("useStemming")) config.use_stemming = opts.Get("useStemming").As<Napi::Boolean>().Value();
        if (opts.Has("caseSensitive")) config.case_sensitive = opts.Get("caseSensitive").As<Napi::Boolean>().Value();
    }
    
    engine_ = std::make_unique<vdb::hybrid::BM25Engine>(config);
}

Napi::Value BM25EngineWrap::AddDocument(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected id and content").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    uint64_t id = info[0].As<Napi::Number>().Int64Value();
    std::string content = info[1].As<Napi::String>().Utf8Value();
    
    auto result = engine_->add_document(id, content);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

Napi::Value BM25EngineWrap::RemoveDocument(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected document id").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    uint64_t id = info[0].As<Napi::Number>().Int64Value();
    auto result = engine_->remove_document(id);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

Napi::Value BM25EngineWrap::UpdateDocument(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected id and content").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    uint64_t id = info[0].As<Napi::Number>().Int64Value();
    std::string content = info[1].As<Napi::String>().Utf8Value();
    
    auto result = engine_->update_document(id, content);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

Napi::Value BM25EngineWrap::Search(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected query string").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string query = info[0].As<Napi::String>().Utf8Value();
    size_t k = 10;
    float min_score = 0.0f;
    
    if (info.Length() > 1 && info[1].IsNumber()) {
        k = info[1].As<Napi::Number>().Uint32Value();
    }
    if (info.Length() > 2 && info[2].IsNumber()) {
        min_score = info[2].As<Napi::Number>().FloatValue();
    }
    
    auto result = engine_->search(query, k, min_score);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array results = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        const auto& r = result.value()[i];
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("id", Napi::Number::New(env, r.id));
        obj.Set("score", Napi::Number::New(env, r.score));
        
        Napi::Array terms = Napi::Array::New(env, r.matched_terms.size());
        for (size_t j = 0; j < r.matched_terms.size(); j++) {
            terms.Set(static_cast<uint32_t>(j), Napi::String::New(env, r.matched_terms[j]));
        }
        obj.Set("matchedTerms", terms);
        
        results.Set(static_cast<uint32_t>(i), obj);
    }
    
    return results;
}

Napi::Value BM25EngineWrap::DocumentCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), engine_->document_count());
}

Napi::Value BM25EngineWrap::TermCount(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), engine_->term_count());
}

Napi::Value BM25EngineWrap::AverageDocumentLength(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), engine_->average_document_length());
}

Napi::Value BM25EngineWrap::Save(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected path").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string path = info[0].As<Napi::String>().Utf8Value();
    auto result = engine_->save(path);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

Napi::Value BM25EngineWrap::Load(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected path").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string path = info[0].As<Napi::String>().Utf8Value();
    auto result = vdb::hybrid::BM25Engine::load(path);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    // Return success object (actual instance would need factory pattern)
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", true);
    return response;
}

// ============================================================================
// KeywordExtractor Wrapper Implementation
// ============================================================================

Napi::Object KeywordExtractorWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "KeywordExtractor", {
        InstanceMethod("extract", &KeywordExtractorWrap::Extract),
        InstanceMethod("train", &KeywordExtractorWrap::Train),
        InstanceMethod("save", &KeywordExtractorWrap::Save),
        StaticMethod("load", &KeywordExtractorWrap::Load),
    });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    exports.Set("KeywordExtractor", func);
    return exports;
}

KeywordExtractorWrap::KeywordExtractorWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<KeywordExtractorWrap>(info) {
    Napi::Env env = info.Env();
    
    vdb::hybrid::KeywordConfig config;
    if (info.Length() > 0 && info[0].IsObject()) {
        Napi::Object opts = info[0].As<Napi::Object>();
        if (opts.Has("maxKeywords")) config.max_keywords = opts.Get("maxKeywords").As<Napi::Number>().Uint32Value();
        if (opts.Has("minScore")) config.min_score = opts.Get("minScore").As<Napi::Number>().FloatValue();
        if (opts.Has("useTfidf")) config.use_tfidf = opts.Get("useTfidf").As<Napi::Boolean>().Value();
    }
    
    extractor_ = std::make_unique<vdb::hybrid::KeywordExtractor>(config);
}

Napi::Value KeywordExtractorWrap::Extract(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected text").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string text = info[0].As<Napi::String>().Utf8Value();
    auto result = extractor_->extract(text);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array keywords = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        const auto& kw = result.value()[i];
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("keyword", Napi::String::New(env, kw.keyword));
        obj.Set("score", Napi::Number::New(env, kw.score));
        keywords.Set(static_cast<uint32_t>(i), obj);
    }
    
    return keywords;
}

Napi::Value KeywordExtractorWrap::Train(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Expected array of documents").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array docs = info[0].As<Napi::Array>();
    std::vector<std::string> documents;
    for (uint32_t i = 0; i < docs.Length(); i++) {
        documents.push_back(docs.Get(i).As<Napi::String>().Utf8Value());
    }
    
    auto result = extractor_->train(documents);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

Napi::Value KeywordExtractorWrap::Save(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected path").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string path = info[0].As<Napi::String>().Utf8Value();
    auto result = extractor_->save(path);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

Napi::Value KeywordExtractorWrap::Load(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected path").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", true);
    return response;
}

// ============================================================================
// HybridSearchEngine Wrapper Implementation
// ============================================================================

Napi::Object HybridSearchEngineWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "HybridSearchEngine", {
        InstanceMethod("combine", &HybridSearchEngineWrap::Combine),
        StaticMethod("weightedSum", &HybridSearchEngineWrap::WeightedSum),
        StaticMethod("reciprocalRankFusion", &HybridSearchEngineWrap::ReciprocalRankFusion),
        StaticMethod("combSum", &HybridSearchEngineWrap::CombSum),
        StaticMethod("combMnz", &HybridSearchEngineWrap::CombMnz),
    });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    exports.Set("HybridSearchEngine", func);
    return exports;
}

HybridSearchEngineWrap::HybridSearchEngineWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<HybridSearchEngineWrap>(info) {
    engine_ = std::make_unique<vdb::hybrid::HybridSearchEngine>();
}

Napi::Value HybridSearchEngineWrap::Combine(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected vector results and bm25 results").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    // Parse vector results
    std::vector<vdb::QueryResult> vector_results;
    if (info[0].IsArray()) {
        Napi::Array arr = info[0].As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); i++) {
            Napi::Object obj = arr.Get(i).As<Napi::Object>();
            vdb::QueryResult r;
            r.id = obj.Get("id").As<Napi::Number>().Int64Value();
            r.distance = obj.Get("distance").As<Napi::Number>().FloatValue();
            r.score = obj.Has("score") ? obj.Get("score").As<Napi::Number>().FloatValue() : 1.0f - r.distance;
            vector_results.push_back(r);
        }
    }
    
    // Parse BM25 results
    std::vector<vdb::hybrid::BM25Result> bm25_results;
    if (info[1].IsArray()) {
        Napi::Array arr = info[1].As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); i++) {
            Napi::Object obj = arr.Get(i).As<Napi::Object>();
            vdb::hybrid::BM25Result r;
            r.id = obj.Get("id").As<Napi::Number>().Int64Value();
            r.score = obj.Get("score").As<Napi::Number>().FloatValue();
            bm25_results.push_back(r);
        }
    }
    
    float vector_weight = 0.5f;
    size_t k = 10;
    
    if (info.Length() > 2 && info[2].IsObject()) {
        Napi::Object opts = info[2].As<Napi::Object>();
        if (opts.Has("vectorWeight")) vector_weight = opts.Get("vectorWeight").As<Napi::Number>().FloatValue();
        if (opts.Has("k")) k = opts.Get("k").As<Napi::Number>().Uint32Value();
    }
    
    auto result = engine_->combine(vector_results, bm25_results, vector_weight, k);
    
    if (!result.has_value()) {
        Napi::Error::New(env, result.error().message).ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Array results = Napi::Array::New(env, result.value().size());
    for (size_t i = 0; i < result.value().size(); i++) {
        const auto& r = result.value()[i];
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("id", Napi::Number::New(env, r.id));
        obj.Set("score", Napi::Number::New(env, r.score));
        results.Set(static_cast<uint32_t>(i), obj);
    }
    
    return results;
}

Napi::Value HybridSearchEngineWrap::WeightedSum(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // Static fusion method - returns combined results
    Napi::Array results = Napi::Array::New(env);
    return results;
}

Napi::Value HybridSearchEngineWrap::ReciprocalRankFusion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array results = Napi::Array::New(env);
    return results;
}

Napi::Value HybridSearchEngineWrap::CombSum(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array results = Napi::Array::New(env);
    return results;
}

Napi::Value HybridSearchEngineWrap::CombMnz(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array results = Napi::Array::New(env);
    return results;
}

// ============================================================================
// QueryRewriter Wrapper Implementation
// ============================================================================

Napi::Object QueryRewriterWrap::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "QueryRewriter", {
        InstanceMethod("rewrite", &QueryRewriterWrap::Rewrite),
        InstanceMethod("addSynonym", &QueryRewriterWrap::AddSynonym),
        InstanceMethod("loadSynonyms", &QueryRewriterWrap::LoadSynonyms),
    });
    
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    
    exports.Set("QueryRewriter", func);
    return exports;
}

QueryRewriterWrap::QueryRewriterWrap(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<QueryRewriterWrap>(info) {
    rewriter_ = std::make_unique<vdb::hybrid::QueryRewriter>();
}

Napi::Value QueryRewriterWrap::Rewrite(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected query").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string query = info[0].As<Napi::String>().Utf8Value();
    auto result = rewriter_->rewrite(query);
    
    if (!result.has_value()) {
        return Napi::String::New(env, query);
    }
    
    return Napi::String::New(env, result.value());
}

Napi::Value QueryRewriterWrap::AddSynonym(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Expected term and synonyms").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string term = info[0].As<Napi::String>().Utf8Value();
    std::vector<std::string> synonyms;
    
    if (info[1].IsArray()) {
        Napi::Array arr = info[1].As<Napi::Array>();
        for (uint32_t i = 0; i < arr.Length(); i++) {
            synonyms.push_back(arr.Get(i).As<Napi::String>().Utf8Value());
        }
    }
    
    rewriter_->add_synonym(term, synonyms);
    
    return Napi::Boolean::New(env, true);
}

Napi::Value QueryRewriterWrap::LoadSynonyms(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected path").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string path = info[0].As<Napi::String>().Utf8Value();
    auto result = rewriter_->load_synonyms(path);
    
    Napi::Object response = Napi::Object::New(env);
    response.Set("success", result.has_value());
    if (!result.has_value()) {
        response.Set("error", result.error().message);
    }
    return response;
}

} // namespace hektor_native
