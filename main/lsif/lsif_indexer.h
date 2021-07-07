#ifndef SORBET_MAIN_LSIF_LSIFINDEXER_H
#define SORBET_MAIN_LSIF_LSIFINDEXER_H

#include "ast/ast.h"
#include "core/core.h"

namespace sorbet::realmain::lsif {
class LSIFIndexer {
    std::ostream &output_stream;

    LSIFIndexer(std::ostream &out) : output_stream(out) {}

public:
    static void emitIndex(const core::GlobalState &gs, std::vector<ast::ParsedFile> &typecheck_result,
                          std::ostream &out);

    ast::ExpressionPtr postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr);

    std::vector<int> ranges;
};
} // namespace sorbet::realmain::lsif

#endif