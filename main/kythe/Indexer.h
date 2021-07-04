#ifndef SORBET_MAIN_KYTHE_INDEXER_H
#define SORBET_MAIN_KYTHE_INDEXER_H

#include "RecordFinder.h"
#include "ast/ast.h"
#include "core/core.h"

namespace sorbet::realmain::kythe {

class Indexer final {
public:
    static void indexOneFile(const core::GlobalState &gs, ast::ParsedFile &pf, std::string_view corpus,
                             std::ostream &out);
};

} // namespace sorbet::realmain::kythe
#endif
