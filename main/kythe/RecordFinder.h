#ifndef SORBET_MAIN_KYTHE_RECORDFINDER_H
#define SORBET_MAIN_KYTHE_RECORDFINDER_H

#include "ast/ast.h"
#include "core/core.h"
#include "main/kythe/VName.h"

namespace sorbet::realmain::kythe {

class RecordFinder {
    // TODO: can we use a vector instead of this map
    VName toVName(const core::GlobalState &gs, core::FileRef file, ast::ClassDef &classOrModule);
    VName toVName(const core::GlobalState &gs, core::FileRef file, ast::MethodDef &classOrModule);

public:
    RecordFinder(const core::GlobalState &gs);

    ast::ExpressionPtr postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr);
    ast::ExpressionPtr postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr);

    UnorderedMap<core::ClassOrModuleRef, VName> vnameForClass;
    UnorderedMap<core::MethodRef, VName> vnameForMethod;
};

} // namespace sorbet::realmain::kythe

#endif // SORBET_MAIN_KYTHE_RECORDFINDER_H