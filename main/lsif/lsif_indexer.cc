#include "main/lsif/lsif_indexer.h"
#include "ast/ast.h"
#include "ast/treemap/treemap.h"
#include "main/lsif/writer.h"
#include <iostream>

using namespace std;
namespace sorbet::realmain::lsif {

std::string posStringForLoc(const core::GlobalState &gs, core::FileRef file, core::LocOffsets loc) {
    auto start = core::Loc::offset2Pos(file.data(gs), loc.beginPos());
    auto end = core::Loc::offset2Pos(file.data(gs), loc.endPos());
    return fmt::format("(L{}:{}-L{}:{})", start.line, start.column, end.line, end.column);
}

ast::ExpressionPtr LSIFIndexer::postTransformLocal(core::Context ctx, ast::ExpressionPtr expr) {
    auto &local = ast::cast_tree_nonnull<ast::Local>(expr);
    const auto &gs = ctx.state;
    // ok i think i maybe understand what to do here?
    fmt::print("local variable: _name={} unique={} loc={}\n", local.localVariable._name.toString(gs),
               local.localVariable.unique, posStringForLoc(gs, ctx.file, local.loc));

    return expr;
}

ast::ExpressionPtr LSIFIndexer::postTransformAssign(core::Context ctx, ast::ExpressionPtr expr) {
    auto &assign = ast::cast_tree_nonnull<ast::Assign>(expr);
    const auto &gs = ctx.state;
    // we want to
    fmt::print("assign: {} loc={}\n", assign.toStringWithTabs(gs), posStringForLoc(gs, ctx.file, assign.loc));

    return expr;
}

ast::ExpressionPtr LSIFIndexer::postTransformSend(core::Context ctx, ast::ExpressionPtr expr) {
    auto &send = ast::cast_tree_nonnull<ast::Send>(expr);
    const auto &gs = ctx.state;
    fmt::print("send: {} on {} (in {})\n{}\n-----------\n", send.fun.toString(gs), send.recv.toString(gs),
               ctx.owner.toStringFullName(gs), send.recv.toString(gs));
    return expr;
}

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
    ENFORCE(method.loc.exists());

    auto resultSet = writer.emitResultSet();

    auto start = core::Loc::offset2Pos(ctx.file.data(gs), method.loc.beginPos());
    auto end = core::Loc::offset2Pos(ctx.file.data(gs), method.loc.endPos());

    auto range = writer.emitRange(start, end);
    writer.emitNext(range, resultSet);
    ranges.push_back(range);
    method.symbol.toStringFullName(gs);
    if (method.symbol.data(gs)->resultType) {
        fmt::print("method {} returns {}\n", method.name.toString(gs), method.symbol.data(gs)->resultType.toString(gs));
    } else {
        fmt::print("method {} (on {})\n", method.name.toString(gs), ctx.owner.toStringFullName(gs));
    }

    rangeIdForSymbol[method.symbol] = range;
    resultSetIdForSymbol[method.symbol] = resultSet;

    return expr;
}

std::string uriForFile(const core::GlobalState &gs, core::FileRef file) {
    auto corpus = "<corpus>";
    return fmt::format("file:///{}/{}", corpus, file.data(gs).path());
}

void LSIFIndexer::emitIndex(const core::GlobalState &gs, vector<ast::ParsedFile> &typecheck_result, ostream &out) {
    Writer writer{out};

    auto project = writer.emitProject("ruby");

    vector<int> documents;

    for (auto &pf : typecheck_result) {
        core::Context ctx{gs, core::Symbols::root(), pf.file};
        vector<int> ranges;
        LSIFIndexer indexer{writer};

        auto uri = uriForFile(gs, pf.file);
        auto document = writer.emitDocument("ruby", uri);

        ast::TreeMap::apply(ctx, indexer, move(pf.tree));

        writer.emitContains(document, indexer.ranges);

        documents.push_back(document);
    }

    writer.emitContains(project, documents);
}
} // namespace sorbet::realmain::lsif