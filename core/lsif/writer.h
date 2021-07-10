#ifndef SORBET_MAIN_LSIF_WRITER_H
#define SORBET_MAIN_LSIF_WRITER_H

#include "absl/synchronization/mutex.h"
#include "core/Loc.h"
#include "core/lsp/QueryResponse.h"
#include <string>

namespace sorbet::realmain::lsif {
class Indexer;
}

namespace sorbet::core::lsif {

// used to generate "contains" relationships
class DocumentInfo final {
public:
    int documentId;
    std::vector<int> definitionRangeIds;
    std::vector<int> referenceRangeIds;

    DocumentInfo(int id) : documentId(id), definitionRangeIds(), referenceRangeIds() {}
};

class DefinitionInfo final {
public:
    int documentId;
    int rangeId;
    int resultSetId;
    int definitionResultId;
    int hoverResultId;
    // document ID -> vec<range id>
    UnorderedMap<int, std::vector<int>> referenceRangeIds;

    DefinitionInfo(int documentId, int rangeId, int resultSetId, int definitionResultId, int hoverResultId)
        : documentId(documentId), rangeId(rangeId), resultSetId(resultSetId), definitionResultId(definitionResultId),
          hoverResultId(hoverResultId), referenceRangeIds() {}
};

class Writer {
    friend class sorbet::realmain::lsif::Indexer;

    std::ostream &outputStream;
    absl::Mutex outputLock;
    int nextId;
    UnorderedMap<core::FileRef, std::unique_ptr<DocumentInfo>> documentInfoForFile;
    UnorderedMap<core::MethodRef, DefinitionInfo> methodDefinitions;

    int emitResultSet(int id);
    int emitVertex(int id, std::string_view label);
    int emitNextEdge(int id, int outV, int inV);
    int emitRange(int id, sorbet::core::Loc::Detail start, sorbet::core::Loc::Detail end);
    int emitDocument(int id, std::string_view language, std::string_view uri, std::string_view contents);
    int emitProject(int id, std::string_view language, std::string_view name);
    int emitContains(int id, int outV, const std::vector<int> &inVs);
    int emitMetaData(int id, std::string_view version, std::string_view projectRoot, std::string_view toolName);
    int emitEdge(int id, std::string_view label, int outV, int inV);
    int emitItemEdge(int id, int outV, const std::vector<int> &inVs, int shard, std::string_view property);
    int emitHoverResult(int id, std::string_view kind, std::string_view value);
    int emitGroup(int id, std::string_view uri, std::string_view conflictResolution, std::string_view name,
                  std::string_view rootUri);

public:
    Writer();

    Writer(const Writer &) = delete;
    Writer(Writer &&) = delete;

    int emitResultSet();
    int emitVertex(std::string_view label);
    int emitNextEdge(int outV, int inV);
    int emitRange(sorbet::core::Loc::Detail start, sorbet::core::Loc::Detail end);
    int emitDocument(std::string_view language, std::string_view uri, std::string_view contents = {});
    int emitProject(std::string_view language, std::string_view name);
    int emitContains(int outV, const std::vector<int> &inVs);
    int emitMetaData(std::string_view version, std::string_view projectRoot, std::string_view toolName);
    int emitItemEdge(int outV, const std::vector<int> &inVs, int shard, std::string_view property = {});
    int emitItemEdge(int outV, int inV, int shard, std::string_view property = {});
    int emitEdge(std::string_view label, int outV, int inV);
    int emitHoverResult(std::string_view kind, std::string_view value);
    int emitGroup(std::string_view uri, std::string_view conflictResolution, std::string_view name,
                  std::string_view rootUri);
};
} // namespace sorbet::core::lsif

#endif