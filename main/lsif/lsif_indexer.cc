#include "main/lsif/lsif_indexer.h"
#include "ast/ast.h"
#include "ast/treemap/treemap.h"
#include "main/lsif/writer.h"
#include <iostream>

using namespace std;
namespace sorbet::realmain::lsif {

ast::ExpressionPtr LSIFIndexer::postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr) {
    auto &klass = ast::cast_tree_nonnull<ast::ClassDef>(expr);
    const auto &gs = ctx.state;
    if (ast::isa_tree<ast::EmptyTree>(klass.name)) {
        return expr;
    }

    fmt::print("class: {}\n", klass.name.toString(gs));
    return expr;
}

ast::ExpressionPtr LSIFIndexer::postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr) {
    auto &method = ast::cast_tree_nonnull<ast::MethodDef>(expr);
    const auto &gs = ctx.state;
    if (method.name == core::Names::staticInit()) {
        return expr;
    }
    output_stream << fmt::format("method: {}\n", method.name.toString(gs));
    return expr;
}

std::string uriForFile(const core::GlobalState &gs, core::FileRef file) {
    auto corpus = "<corpus>";
    return fmt::format("file:///{}/{}", corpus, file.data(gs).path());
}

void LSIFIndexer::emitIndex(const core::GlobalState &gs, vector<ast::ParsedFile> &typecheck_result, ostream &out) {
    Writer writer{cout};

    auto project = writer.emitProject("ruby");

    vector<int> documents;

    for (auto &pf : typecheck_result) {
        core::Context ctx{gs, core::Symbols::root(), pf.file};
        vector<int> ranges;
        LSIFIndexer indexer{cout};
        ast::TreeMap::apply(ctx, indexer, move(pf.tree));

        auto uri = uriForFile(gs, pf.file);
        auto document = writer.emitDocument("ruby", uri);
        writer.emitContains(document, indexer.ranges);

        documents.push_back(document);
    }

    writer.emitContains(project, documents);
}
} // namespace sorbet::realmain::lsif