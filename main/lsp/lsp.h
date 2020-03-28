#ifndef RUBY_TYPER_LSPLOOP_H
#define RUBY_TYPER_LSPLOOP_H

#include "core/core.h"
#include "main/lsp/LSPIndexer.h"
#include "main/lsp/LSPMessage.h"
#include "main/lsp/LSPPreprocessor.h"
#include "main/lsp/LSPTypecheckerCoordinator.h"
#include <chrono>
#include <optional>

//  _     ____  ____
// | |   / ___||  _ _\
// | |   \___ \| |_) |
// | |___ ___) |  __/
// |_____|____/|_|
//
//
// This is an implementation of LSP protocol (version 3.13) for Sorbet
namespace sorbet::realmain::lsp {

class MarkupContent;
class LSPInput;
class LSPConfiguration;
class LSPQueuePreemptionTask;

class LSPLoop {
    friend class LSPWrapper;
    friend class LSPQueuePreemptionTask;

    /** Encapsulates the active configuration for the language server. */
    const std::shared_ptr<const LSPConfiguration> config;
    /** Protects outgoingQueue. */
    const std::shared_ptr<absl::Mutex> taskQueueMutex;
    /** Contains the output of LSPPreprocessor -- messages that have been converted into tasks. */
    const std::shared_ptr<TaskQueueState> taskQueue GUARDED_BY(taskQueueMutex);
    const std::shared_ptr<core::lsp::TypecheckEpochManager> epochManager;

    /** The LSP preprocessor standardizes incoming messages and combines edits. */
    LSPPreprocessor preprocessor;
    /** The LSP typechecker coordinator typechecks file updates and runs queries. */
    LSPTypecheckerCoordinator typecheckerCoord;
    /** The LSP indexer indexes incoming edits and makes fast/slow path decisions. */
    LSPIndexer indexer;
    /** A WorkerPool with 0 workers. */
    std::unique_ptr<WorkerPool> emptyWorkers;
    /**
     * The time that LSP last sent metrics to statsd -- if `opts.statsdHost` was specified.
     */
    std::chrono::time_point<std::chrono::steady_clock> lastMetricUpdateTime;
    /** ID of the main thread, which actually processes LSP requests and performs typechecking. */
    std::thread::id mainThreadId;

    void runTask(std::unique_ptr<LSPTask> task);

    /** Returns `true` if 5 minutes have elapsed since LSP last sent counters to statsd. */
    bool shouldSendCountersToStatsd(std::chrono::time_point<std::chrono::steady_clock> currentTime) const;
    /** Sends counters to statsd. */
    void sendCountersToStatsd(std::chrono::time_point<std::chrono::steady_clock> currentTime);

    /** Returns a set of tasks for the given LSPMessage. If said message is an edit, the edits are committed. */
    std::vector<std::unique_ptr<LSPTask>> commitAndGetTasksForMessage(LSPMessage &msg);

public:
    LSPLoop(std::unique_ptr<core::GlobalState> initialGS, WorkerPool &workers,
            const std::shared_ptr<LSPConfiguration> &config, std::unique_ptr<const OwnedKeyValueStore> kvstore);
    /**
     * Runs the language server on a dedicated thread. Returns the final global state if it exits cleanly, or nullopt
     * on error.
     *
     * Reads input messages from the provided input object.
     */
    std::optional<std::unique_ptr<core::GlobalState>> runLSP(std::shared_ptr<LSPInput> input);
    void processRequest(std::unique_ptr<LSPMessage> msg);
    void processRequest(const std::string &json);
    /**
     * Processes a batch of requests. Performs pre-processing to avoid unnecessary work.
     */
    void processRequests(std::vector<std::unique_ptr<LSPMessage>> messages);
    /**
     * (For tests only) Retrieve the number of times typechecking has run.
     */
    int getTypecheckCount();
};

std::optional<std::string> findDocumentation(std::string_view sourceCode, int beginIndex);
bool hasSimilarName(const core::GlobalState &gs, core::NameRef name, std::string_view pattern);
bool hideSymbol(const core::GlobalState &gs, core::SymbolRef sym);
std::unique_ptr<MarkupContent> formatRubyMarkup(MarkupKind markupKind, std::string_view rubyMarkup,
                                                std::optional<std::string_view> explanation);
std::string prettyTypeForMethod(const core::GlobalState &gs, core::SymbolRef method, core::TypePtr receiver,
                                core::TypePtr retType, const core::TypeConstraint *constraint);
std::string prettyTypeForConstant(const core::GlobalState &gs, core::SymbolRef constant);
core::TypePtr getResultType(const core::GlobalState &gs, core::TypePtr type, core::SymbolRef inWhat,
                            core::TypePtr receiver, const core::TypeConstraint *constr);
SymbolKind symbolRef2SymbolKind(const core::GlobalState &gs, core::SymbolRef sym);

} // namespace sorbet::realmain::lsp
#endif // RUBY_TYPER_LSPLOOP_H