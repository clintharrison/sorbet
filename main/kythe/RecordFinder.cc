#include "main/kythe/RecordFinder.h"
#include "core/Names.h"
#include "main/kythe/KytheJsonWriter.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

using namespace std;
using namespace rapidjson;

namespace sorbet::realmain::kythe {

VName writeAnchor(const core::GlobalState &gs, OStreamWrapper &osw, core::LocOffsets loc, VName fileVName) {
    auto begin = loc.beginPos();
    auto end = loc.endPos();
    auto anchor = KytheJsonWriter::vnameForAnchor(fileVName, begin, end);

    KytheJsonWriter::writeFact(osw, anchor, "node/kind", "anchor");
    KytheJsonWriter::writeFact(osw, anchor, "loc/start", fmt::to_string(begin));
    KytheJsonWriter::writeFact(osw, anchor, "loc/end", fmt::to_string(end));
    // TODO(clint): ask rbraunstein why the scala one does this?
    // KytheJsonWriter::writeEdge(osw, anchor, "childof", fileVName);

    return anchor;
}

void writeNodeForDefines(const core::GlobalState &gs, OStreamWrapper &osw, core::LocOffsets locs, VName &node,
                         VName &anchor, string_view node_kind, string_view subkind) {
    KytheJsonWriter::writeEdge(osw, anchor, "defines", node);
    KytheJsonWriter::writeEdge(osw, anchor, "defines/binding", node);
    KytheJsonWriter::writeFact(osw, node, "node/kind", node_kind);
    if (!subkind.empty()) {
        KytheJsonWriter::writeFact(osw, node, "subkind", subkind);
    }
}

VName RecordFinder::toVName(const core::GlobalState &gs, core::SymbolRef symbol) {
    auto data = symbol.data(gs);

    auto name = data->name.show(gs);
    string owner;

    if (symbol.isClassOrModule()) {
        owner = (data->owner != core::Symbols::root()) ? data->owner.showFullName(gs) : "";
    } else if (symbol.isMethod()) {
        owner = data->owner.showFullName(gs);
    }

    auto signature = name;
    auto path = owner;
    auto language = "";
    auto root = "";
    auto corpus = "";

    return VName{signature, path, language, root, corpus};
}

RecordFinder::RecordFinder(const core::GlobalState &gs, std::ostream &os, VName fileVName)
    : output_stream(os), fileVName(fileVName) {}

ast::ExpressionPtr RecordFinder::postTransformClassDef(core::Context ctx, ast::ExpressionPtr expr) {
    auto &classDef = ast::cast_tree_nonnull<ast::ClassDef>(expr);
    // return early if we have the empty tree
    if (ast::isa_tree<ast::EmptyTree>(classDef.name)) {
        return expr;
    }
    const auto &gs = ctx.state;
    rapidjson::OStreamWrapper osw{output_stream};

    auto loc = classDef.declLoc;

    // write the anchor for this node, and tie it to the special file node
    auto anchor = writeAnchor(gs, osw, loc, fileVName);

    auto node_kind = "record";                   // kythe name for a module/class
    auto subkind = classDef.symbol.showKind(gs); // language specific subkind

    auto nodeVName = toVName(gs, classDef.symbol);
    writeNodeForDefines(gs, osw, loc, nodeVName, anchor, node_kind, subkind);

    auto ownerSym = classDef.symbol.data(gs)->owner;
    if (ownerSym != core::Symbols::root()) {
        auto ownerVName = toVName(gs, ownerSym);
        KytheJsonWriter::writeEdge(osw, nodeVName, "childof", ownerVName);
    }

    // todo write type edges
    // todo write extends edges

    // return the original expr: the indexer does not modify anything
    return expr;
}

ast::ExpressionPtr RecordFinder::postTransformMethodDef(core::Context ctx, ast::ExpressionPtr expr) {
    auto &methodDef = ast::cast_tree_nonnull<ast::MethodDef>(expr);
    // I think these exist just for type checking, so we can ignore them?
    if (methodDef.name == core::Names::staticInit()) {
        return expr;
    }
    const auto &gs = ctx.state;
    rapidjson::OStreamWrapper osw{output_stream};
    auto loc = methodDef.declLoc;

    // write the anchor for this node, and tie it to the special file node
    auto anchor = writeAnchor(gs, osw, loc, fileVName);

    auto nodeVName = toVName(gs, methodDef.symbol);
    auto node_kind = "function";
    auto subkind = methodDef.symbol.showKind(gs);
    writeNodeForDefines(gs, osw, loc, nodeVName, anchor, node_kind, subkind);

    auto ownerSym = methodDef.symbol.data(gs)->owner;
    if (ownerSym != core::Symbols::root()) {
        auto ownerVName = toVName(gs, ownerSym);
        KytheJsonWriter::writeEdge(osw, nodeVName, "childof", ownerVName);
    }

    // todo write type edges

    return expr;
}

} // namespace sorbet::realmain::kythe