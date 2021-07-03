#include "main/kythe/RecordFinder.h"

namespace sorbet::realmain::kythe {

VName RecordFinder::toVName(const core::GlobalState &gs, core::FileRef file, ast::MethodDef &method) {
    auto data = method.symbol.data(gs);

    auto owner = data->owner.showFullName(gs);
    auto name = data->name.show(gs);

    auto signature = name;
    auto path = owner;
    auto language = "ruby";
    auto root = "";
    auto corpus = "";

    return VName{signature, path, language, root, corpus};
}

VName RecordFinder::toVName(const core::GlobalState &gs, core::FileRef file, ast::ClassDef &classOrModule) {
    auto data = classOrModule.symbol.data(gs);

    auto owner = (data->owner != core::Symbols::root()) ? data->owner.showFullName(gs) : "";
    auto name = data->name.show(gs);

    auto signature = name;
    auto path = file.data(gs).path().data();
    auto language = "ruby";
    auto root = "";
    auto corpus = "";

    return VName{signature, path, language, root, corpus};
}

RecordFinder::RecordFinder(const core::GlobalState &gs) : vnameForClass(gs.classAndModulesUsed()) {}

ast::ExpressionPtr RecordFinder::postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr) {
    auto &original = ast::cast_tree_nonnull<ast::ClassDef>(expr);
    // return early if we have the empty tree
    if (ast::isa_tree<ast::EmptyTree>(original.name)) {
        return expr;
    }

    const auto &gs = ctx.state;
    auto vname = toVName(gs, ctx.file, original);

    vnameForClass[original.symbol] = vname;
    return expr;
}

ast::ExpressionPtr RecordFinder::postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr) {
    auto &original = ast::cast_tree_nonnull<ast::MethodDef>(expr);
    // return early if we have the <static-init> fake method
    if (original.name == core::Names::staticInit()) {
        return expr;
    }

    const auto &gs = ctx.state;
    auto vname = toVName(gs, ctx.file, original);

    vnameForMethod[original.symbol] = vname;
    return expr;
}

} // namespace sorbet::realmain::kythe