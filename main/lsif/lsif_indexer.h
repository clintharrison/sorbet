#ifndef SORBET_MAIN_LSIF_LSIFINDEXER_H
#define SORBET_MAIN_LSIF_LSIFINDEXER_H

#include "ast/ast.h"
#include "core/core.h"
#include "main/lsif/writer.h"

namespace sorbet::realmain::lsif {

class LSIFIndexer {
    Writer &writer;
    UnorderedMap<core::SymbolRef, int> rangeIdForSymbol;
    UnorderedMap<core::SymbolRef, int> resultSetIdForSymbol;
    UnorderedMap<core::SymbolRef, int> definitionIdForSymbol;
    UnorderedMap<core::MethodRef, UnorderedMap<core::LocalVariable, core::Loc>> locForLocalVarInMethod;

    LSIFIndexer(Writer &writer) : writer(writer) {}

public:
    static void emitIndex(const core::GlobalState &gs, std::vector<ast::ParsedFile> &typecheck_result,
                          std::ostream &out);

    ast::ExpressionPtr postTransformLocal(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformSend(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformAssign(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr);

    std::vector<int> ranges;
};
} // namespace sorbet::realmain::lsif

#endif