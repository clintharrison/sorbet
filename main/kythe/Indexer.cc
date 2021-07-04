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

// This is the "main" function for the kythe indexer, called once for each parsed file, after typechecking is complete.
void Indexer::indexOneFile(const core::GlobalState &gs, ast::ParsedFile &pf, std::string_view corpus, ostream &out) {
    auto fileVName = KytheJsonWriter::vnameForFile(pf.file.data(gs), corpus);

    // Visit the tree and print out kythe nodes when convenient :shrug:
    RecordFinder finder{gs, out, fileVName};
    core::Context ctx(gs, core::Symbols::root(), pf.file);
    auto tree = ast::TreeMap::apply(ctx, finder, std::move(pf.tree));

    // Write 'boring' file nodes...
    OStreamWrapper osw{out};
    KytheJsonWriter::writeFact(osw, fileVName, "node/kind", "file");
    KytheJsonWriter::writeFact(osw, fileVName, "text", pf.file.data(gs).source());
}

} // namespace sorbet::realmain::kythe