#include "main/kythe/Indexer.h"
#include "ast/ast.h"
#include "ast/treemap/treemap.h"
#include "core/Context.h"
#include "main/kythe/KytheJsonWriter.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

using namespace std;
using namespace rapidjson;
namespace sorbet::realmain::kythe {

void Indexer::writeFileNodeJson(const core::GlobalState &gs, ast::ParsedFile &pf, std::string_view corpus,
                                ostream &out) {
    OStreamWrapper osw(out);
    Writer<OStreamWrapper> writer(osw);

    RecordFinder finder{gs};
    core::Context ctx(gs, core::Symbols::root(), pf.file);
    auto tree = ast::TreeMap::apply(ctx, finder, std::move(pf.tree));

    auto fileVName = KytheJsonWriter::vnameForFile(pf.file.data(gs), corpus);

    for (auto &&pr : finder.vnameForClass) {
        // *** Create anchor ***
        auto klass = pr.first.data(gs);

        // "E" for example
        // fmt::print("klass={}", pr.first.show(gs));
        auto loc = klass->loc();
        auto begin = loc.offsets().beginPos();
        auto end = loc.offsets().endPos();
        auto vnameForBinding = KytheJsonWriter::vnameForAnchor(fileVName, begin, end);

        KytheJsonWriter::writeFact(writer, vnameForBinding, "node/kind", "anchor");
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, vnameForBinding, "loc/start", fmt::to_string(begin));
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, vnameForBinding, "loc/end", fmt::to_string(end));
        writer.Reset(osw);
        osw.Put('\n');

        // *** Create semantic edges ***
        auto semanticVName = pr.second;
        // TODO: do we need defines and defines/binding?
        KytheJsonWriter::writeEdge(writer, vnameForBinding, "defines", semanticVName);
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeEdge(writer, vnameForBinding, "defines/binding", semanticVName);
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, semanticVName, "node/kind", "record");
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, semanticVName, "subkind", pr.first.showKind(gs));
        writer.Reset(osw);
        osw.Put('\n');
    }

    for (auto &&pr : finder.vnameForMethod) {
        // *** Create anchor for the method ***
        auto method = pr.first.data(gs);
        auto loc = method->loc();
        auto begin = loc.offsets().beginPos();
        auto end = loc.offsets().endPos();
        auto vnameForBinding = KytheJsonWriter::vnameForAnchor(fileVName, begin, end);

        KytheJsonWriter::writeFact(writer, vnameForBinding, "node/kind", "anchor");
        writer.Reset(osw);
        osw.Put('\n');

        // The anchor loc here seems to be wrong?? But I'm not sure why?
        // fmt::print("METHOD<{}>!\n{}", pr.first.data(gs)->name.toString(gs), loc.toString(gs));
        KytheJsonWriter::writeFact(writer, vnameForBinding, "loc/start", fmt::to_string(begin));
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, vnameForBinding, "loc/end", fmt::to_string(end));
        writer.Reset(osw);
        osw.Put('\n');

        // *** Create semantic edges ***
        auto semanticVName = pr.second;
        // TODO: do we need defines and defines/binding?
        KytheJsonWriter::writeEdge(writer, vnameForBinding, "defines", semanticVName);
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeEdge(writer, vnameForBinding, "defines/binding", semanticVName);
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, semanticVName, "node/kind", "function");
        writer.Reset(osw);
        osw.Put('\n');

        KytheJsonWriter::writeFact(writer, semanticVName, "subkind", pr.first.showKind(gs));
        writer.Reset(osw);
        osw.Put('\n');

        auto kls = method->owner.asClassOrModuleRef();
        auto enclosingClassVName = finder.vnameForClass[kls];
        KytheJsonWriter::writeEdge(writer, semanticVName, "childof", enclosingClassVName);
        writer.Reset(osw);
        osw.Put('\n');
    }

    KytheJsonWriter::writeFact(writer, fileVName, "node/kind", "file");
    writer.Reset(osw);
    osw.Put('\n');

    KytheJsonWriter::writeFact(writer, fileVName, "text", pf.file.data(gs).source());
    writer.Flush();
    osw.Put('\n');
}

} // namespace sorbet::realmain::kythe