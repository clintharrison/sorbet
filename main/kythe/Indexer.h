#ifndef SORBET_MAIN_KYTHE_INDEXER_H
#define SORBET_MAIN_KYTHE_INDEXER_H

#include "RecordFinder.h"
#include "ast/ast.h"
#include "core/core.h"

namespace sorbet::realmain::kythe {

class Indexer final {
public:
    static void writeFileNodeJson(const core::GlobalState &gs, ast::ParsedFile &pf, std::string_view corpus,
                                  std::ostream &out);

private:
};

} // namespace sorbet::realmain::kythe
#endif
