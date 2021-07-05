#ifndef SORBET_MAIN_KYTHE_RECORDFINDER_H
#define SORBET_MAIN_KYTHE_RECORDFINDER_H

#include "ast/ast.h"
#include "core/core.h"
#include "main/kythe/VName.h"

namespace sorbet::realmain::kythe {

// TODO: rename this from "RecordFinder" to something more general.
// We can print as soon as possible in the visitor functions, and process
// methods (and their children--refs I guess?) too. So, this is more than
// just records, and it does more than just Find, it also prints.
class RecordFinder {
    std::ostream &output_stream;
    VName fileVName;

    VName toVName(const core::GlobalState &gs, core::SymbolRef symbol);
    VName toVName(const core::GlobalState &gs, core::LocalVariable local, std::string path);
    VName toVName(const core::GlobalState &gs, ast::Send &send);

public:
    RecordFinder(const core::GlobalState &gs, std::ostream &output_stream, VName fileVName);
    ast::ExpressionPtr postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformConstantLit(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformAssign(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformSend(core::Context ctx, ast::ExpressionPtr expr);
};

} // namespace sorbet::realmain::kythe

#endif // SORBET_MAIN_KYTHE_RECORDFINDER_H