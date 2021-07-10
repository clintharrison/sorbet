#include "main/lsif/Indexer.h"

namespace sorbet::realmain::lsif {

void Indexer::emitForDocumentBegin(core::Context ctx, core::FileRef file) {
    const auto &gs = ctx.state;
    // this is gross, but we need to modify the state now :(
    absl::MutexLock lock{&gs.lsifWriter->outputLock};
    auto id =
        gs.lsifWriter->emitDocument("ruby", fmt::format("file:///{}", file.data(gs).path()), file.data(gs).source());
    auto docInfo = std::make_unique<core::lsif::DocumentInfo>(id);
    gs.lsifWriter->documentInfoForFile.emplace(ctx.file, std::move(docInfo));
}

void Indexer::emitForDocumentEnd(core::Context ctx, core::FileRef file) {
    // const auto &gs = ctx.state;
    // auto &writer = *gs.lsifWriter;
    // auto it = writer.documentInfoForFile.find(ctx.file);
    // if (it == writer.documentInfoForFile.end()) {
    //     // we haven't seen this document? that's weird
    //     return;
    // }
    // auto &docInfo = *it->second;
    // auto docId = docInfo.documentId;
    // writer.emitContains(docId, docInfo.definitionRangeIds);
}

// void Indexer::emitHoverDefinition

void Indexer::emitForMethodDef(core::Context ctx, const ast::ExpressionPtr &expr) {
    auto &method = ast::cast_tree_nonnull<ast::MethodDef>(expr);
    // don't create method defs for fake things like <static-init>
    if (method.name == core::Names::staticInit()) {
        return;
    }

    const auto &gs = ctx.state;
    auto &writer = *gs.lsifWriter;
    // this is gross, but we need to modify the state now :(
    absl::MutexLock lock{&writer.outputLock};
    auto [begin, end] = core::Loc(ctx.file, method.declLoc).position(gs);

    auto it = writer.documentInfoForFile.find(ctx.file);
    if (it == writer.documentInfoForFile.end()) {
        // we haven't seen this document? that's weird
        return;
    }

    auto range = writer.emitRange(begin, end);
    auto resultSet = writer.emitResultSet();

    auto &docInfo = it->second;
    auto docId = docInfo->documentId;
    auto definitionResult = writer.emitVertex("definitionResult");
    writer.emitNextEdge(range, resultSet);
    writer.emitEdge("textDocument/definition", resultSet, definitionResult);
    writer.emitItemEdge(definitionResult, range, docId);

    auto hoverText = fmt::format("```ruby\nTODO hover result for {}\n```", method.symbol.showFullName(gs));
    auto hoverResultId = writer.emitHoverResult("markdown", hoverText);
    writer.emitEdge("textDocument/hover", resultSet, hoverResultId);

    writer.documentInfoForFile[ctx.file]->definitionRangeIds.push_back(range);
    auto defInfo = sorbet::core::lsif::DefinitionInfo(docId, range, resultSet, definitionResult, hoverResultId);
    writer.methodDefinitions.emplace(method.symbol, defInfo);
}

void Indexer::emitForSend(core::Context ctx, sorbet::core::LocOffsets loc,
                          std::shared_ptr<core::DispatchResult> dispatchResult, core::NameRef callerSideName) {
    const auto &gs = ctx.state;
    auto &writer = *gs.lsifWriter;
    // this is gross, but we need to modify the state now :(
    absl::MutexLock lock{&writer.outputLock};

    auto &method = dispatchResult->main.method;
    auto &methodDefs = writer.methodDefinitions;
    auto it = methodDefs.find(method);
    if (it == methodDefs.end()) {
        // hmm, why haven't we seen this method already?
        return;
    }
    auto &defInfo = it->second;

    auto &docInfos = writer.documentInfoForFile;
    auto it2 = docInfos.find(ctx.file);
    if (it2 == docInfos.end()) {
        fmt::print("uh oh, doc info not found!\n");
        return;
    }

    auto [begin, end] = core::Loc(ctx.file, loc).position(gs);
    auto range = writer.emitRange(begin, end);
    writer.emitNextEdge(range, defInfo.resultSetId);

    auto &docInfo = it2->second;
    docInfo->referenceRangeIds.push_back(range);
    defInfo.referenceRangeIds[docInfo->documentId].push_back(range);
}

void Indexer::emitForIdent(core::Context ctx, const core::lsp::IdentResponse identResponse) {
    const auto &gs = ctx.state;
    // there were some <self>s without locs?
    if (!identResponse.termLoc.exists()) {
        return;
    }
    // some <selfRestore> I don't think we need for LSIF? it's not a very sophisticated graph
    if (identResponse.variable._name == core::Names::selfRestore()) {
        return;
    }

    // auto &writer = *gs.lsifWriter;
    fmt::print("visiting ident {} (at {})\n", identResponse.variable._name.show(gs),
               identResponse.termLoc.filePosToString(gs, true));
}

void Indexer::emitForProjectBegin(const core::GlobalState &gs) {
    ENFORCE(gs.lsifWriter);
    // this is gross, but we need to modify the state now :(
    absl::MutexLock lock{&gs.lsifWriter->outputLock};
    // honestly not sure if this is 0.4.0, 0.4.3, or 0.5.0(?) of the LSIF spec.
    // going with the version of lsif-sqlite!
    auto metaDataId = gs.lsifWriter->emitMetaData("0.5.3", "file:///", "sorbet");
    ENFORCE(metaDataId == 1);

    // TODO: stop hardcoding these: uri, conflict resolution, project name, root URI
    auto groupId = gs.lsifWriter->emitGroup("file:///", "takeDB", "<TODO GROUP NAME>", "file:///");
    ENFORCE(groupId == 2)

    auto projectId = gs.lsifWriter->emitProject("ruby", "<TODO PROJECT NAME>");
    ENFORCE(projectId == 3);

    gs.lsifWriter->emitEdge("belongsTo", projectId, groupId);
}

void Indexer::emitForProjectEnd(const core::GlobalState &gs) {
    auto &writer = *gs.lsifWriter;
    // this is gross, but we need to modify the state now :(
    absl::MutexLock lock{&writer.outputLock};

    for (auto &[method, def] : writer.methodDefinitions) {
        auto resultSetId = def.resultSetId;

        if (!def.referenceRangeIds.empty()) {
            auto refResult = writer.emitVertex("referenceResult");
            writer.emitEdge("textDocument/references", resultSetId, refResult);
            for (auto &[docId, rangeIds] : def.referenceRangeIds) {
                writer.emitItemEdge(refResult, rangeIds, docId, "references");
            }
        }

        auto refResult = writer.emitVertex("referenceResult");
        writer.emitEdge("textDocument/references", resultSetId, refResult);
        // TODO: rangeId probably should be "definitionRangeIds", since ruby can have multiple
        writer.emitItemEdge(refResult, def.rangeId, def.documentId, "definitions");
    }

    for (auto &[x, y] : writer.documentInfoForFile) {
        auto docId = y->documentId;
        // I had just gotten to telling someone how modern C++ isn't so bad...
        // and then I had to write this, because I don't know of a better way :(
        auto &refs = y->referenceRangeIds;
        auto &defs = y->definitionRangeIds;
        if (refs.empty() && defs.empty()) {
            continue;
        }
        std::vector<int> allRanges;
        allRanges.reserve(refs.size() + defs.size());
        // RIP
        allRanges.insert(allRanges.end(), std::make_move_iterator(refs.begin()), std::make_move_iterator(refs.end()));
        allRanges.insert(allRanges.end(), std::make_move_iterator(defs.begin()), std::make_move_iterator(defs.end()));
        writer.emitContains(docId, allRanges);
    }

    // emit a "contains" with every file in the project :|
    std::vector<int> documentIds;
    std::transform(writer.documentInfoForFile.begin(), writer.documentInfoForFile.end(),
                   std::back_inserter(documentIds), [&](const auto &pair) { return pair.second->documentId; });
    // TODO: don't hardcode "3" here -- it's the special Project vertex,
    // but eventually we might want more than one?
    // There might be a better way to shard than by "project" though
    writer.emitContains(3, documentIds);
}

} // namespace sorbet::realmain::lsif