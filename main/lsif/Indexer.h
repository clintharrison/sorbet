#ifndef SORBET_REALMAIN_LSIF_INDEXER_H
#define SORBET_REALMAIN_LSIF_INDEXER_H

#include "ast/ast.h"
#include "core/core.h"

namespace sorbet::realmain::lsif {
class Indexer {
public:
    static void emitForDocumentBegin(core::Context ctx, core::FileRef file);
    static void emitForDocumentEnd(core::Context ctx, core::FileRef file);
    static void emitForMethodDef(core::Context ctx, const ast::ExpressionPtr &tree);
    static void emitForSend(core::Context ctx, sorbet::core::LocOffsets loc,
                            std::shared_ptr<core::DispatchResult> dispatchResult, core::NameRef callerSideName);
    static void emitForProjectBegin(const core::GlobalState &gs);
    static void emitForProjectEnd(const core::GlobalState &gs);
};
} // namespace sorbet::realmain::lsif

#endif