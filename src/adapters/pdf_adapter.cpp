// ============================================================================
// VectorDB - PDF Data Adapter Implementation
// ============================================================================

#include "vdb/adapters/pdf_adapter.hpp"
#include <fstream>
#include <sstream>

namespace vdb::adapters {

PDFAdapter::PDFAdapter(const PDFConfig& config) : config_(config) {}

bool PDFAdapter::can_handle(const fs::path& path) const {
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".pdf";
}

bool PDFAdapter::can_handle(std::string_view content) const {
    // PDF files start with "%PDF-"
    return content.size() >= 5 && content.substr(0, 5) == "%PDF-";
}

Result<NormalizedData> PDFAdapter::parse(
    const fs::path& path,
    const ChunkConfig& config
) {
#ifdef HAVE_POPPLER
    // Load PDF document
    auto doc = poppler::document::load_from_file(path.string());
    if (!doc) {
        return std::unexpected(Error{
            ErrorCode::ParseError,
            "Failed to load PDF file: " + path.string()
        });
    }
    
    return parse_pdf_document(doc.get(), config, path.string());
#else
    return std::unexpected(Error{
        ErrorCode::NotImplemented,
        "PDF support requires poppler-cpp library (compile with -DHAVE_POPPLER)"
    });
#endif
}

Result<NormalizedData> PDFAdapter::parse_content(
    std::string_view content,
    const ChunkConfig& config,
    std::string_view source_hint
) {
#ifdef HAVE_POPPLER
    // Write content to temp file (poppler requires file path)
    auto temp_path = fs::temp_directory_path() / "vdb_temp_pdf.pdf";
    {
        std::ofstream out(temp_path, std::ios::binary);
        out.write(content.data(), content.size());
    }
    
    auto result = parse(temp_path, config);
    fs::remove(temp_path);
    return result;
#else
    return std::unexpected(Error{
        ErrorCode::NotImplemented,
        "PDF support requires poppler-cpp library"
    });
#endif
}

Result<void> PDFAdapter::sanitize(NormalizedData& data) {
    // Sanitize each chunk's content
    for (auto& chunk : data.chunks) {
        chunk.content = sanitize_text(chunk.content);
    }
    data.sanitized = true;
    return {};
}

#ifdef HAVE_POPPLER
Result<NormalizedData> PDFAdapter::parse_pdf_document(
    poppler::document* doc,
    const ChunkConfig& config,
    const std::string& source_path
) {
    NormalizedData result;
    result.format = DataFormat::PDF;
    result.source_path = source_path;
    result.confidence = 1.0f;
    
    // Extract metadata
    result.global_metadata = extract_metadata(doc);
    
    // Determine page range
    int num_pages = doc->pages();
    int start = config_.start_page;
    int end = (config_.end_page < 0) ? num_pages : std::min(config_.end_page, num_pages);
    
    // Extract text from each page
    std::string full_text;
    for (int i = start; i < end; ++i) {
        auto page = doc->create_page(i);
        if (!page) continue;
        
        std::string page_text = extract_page_text(page.get());
        if (!page_text.empty()) {
            full_text += page_text;
            full_text += "\n\n"; // Page separator
        }
    }
    
    if (full_text.empty()) {
        result.warnings.push_back("No text extracted from PDF");
    }
    
    // Chunk the full text
    result.chunks = chunk_text(full_text, config, result.global_metadata);
    
    // Add page numbers to metadata
    for (size_t i = 0; i < result.chunks.size(); ++i) {
        result.chunks[i].metadata["page_range"] = 
            std::to_string(start) + "-" + std::to_string(end);
        result.chunks[i].source = source_path;
    }
    
    return result;
}

std::string PDFAdapter::extract_page_text(poppler::page* page) const {
    if (!page) return "";
    
    poppler::page::text_layout_enum layout = config_.preserve_layout 
        ? poppler::page::physical_layout 
        : poppler::page::raw_order_layout;
    
    poppler::ustring text = page->text(layout);
    return text.to_utf8();
}

std::unordered_map<std::string, std::string> PDFAdapter::extract_metadata(
    poppler::document* doc
) const {
    std::unordered_map<std::string, std::string> metadata;
    
    if (!doc) return metadata;
    
    // Extract standard PDF metadata
    auto title = doc->get_title();
    if (!title.empty()) {
        metadata["title"] = title.to_utf8();
    }
    
    auto author = doc->get_author();
    if (!author.empty()) {
        metadata["author"] = author.to_utf8();
    }
    
    auto subject = doc->get_subject();
    if (!subject.empty()) {
        metadata["subject"] = subject.to_utf8();
    }
    
    auto keywords = doc->get_keywords();
    if (!keywords.empty()) {
        metadata["keywords"] = keywords.to_utf8();
    }
    
    auto creator = doc->get_creator();
    if (!creator.empty()) {
        metadata["creator"] = creator.to_utf8();
    }
    
    auto producer = doc->get_producer();
    if (!producer.empty()) {
        metadata["producer"] = producer.to_utf8();
    }
    
    metadata["num_pages"] = std::to_string(doc->pages());
    metadata["pdf_version"] = std::to_string(doc->get_pdf_major_version()) + "." +
                               std::to_string(doc->get_pdf_minor_version());
    
    return metadata;
}
#endif

} // namespace vdb::adapters
