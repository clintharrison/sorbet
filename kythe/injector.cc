#include "ast/ast.h"
#include "cfg/CFG.h"
#include "common/formatting.h"
#include "main/options/options.h"
#include "main/pipeline/semantic_extension/SemanticExtension.h"
#include <cxxopts.hpp>
#include <optional>

using namespace std;

namespace sorbet::pipeline::semantic_extension {
// empty

class KytheIndexerSemanticExtension : public SemanticExtension {
    optional<string> outputPath;

public:
    KytheIndexerSemanticExtension(optional<string> outputPath) {
        this->outputPath = outputPath;
    }

    virtual void finishTypecheckFile(const core::GlobalState &, const core::FileRef &) const override {}

    virtual void finishTypecheck(const core::GlobalState &) const override {}

    virtual void typecheck(const core::GlobalState &gs, cfg::CFG &cfg, ast::MethodDef &md) const override {
        fmt::print("finished type checking {}\n", cfg.file.data(gs).path());
    }

    virtual void run(core::MutableContext &, ast::ClassDef *) const override {}

    virtual ~KytheIndexerSemanticExtension() {}

    virtual std::unique_ptr<SemanticExtension> deepCopy(const core::GlobalState &from, core::GlobalState &to) override {
        return make_unique<KytheIndexerSemanticExtension>(this->outputPath);
    }

    virtual void merge(const core::GlobalState &from, core::GlobalState &to, core::NameSubstitution &subst) override {}
};

class KytheIndexerSemanticExtensionProvider : public SemanticExtensionProvider {
public:
    virtual void injectOptions(cxxopts::Options &opts) const override {
        opts.add_options("kythe-indexer")("index-out", "Write the Kythe index to this file", cxxopts::value<string>());
    }

    virtual std::unique_ptr<SemanticExtension> readOptions(cxxopts::ParseResult &options) const override {
        return make_unique<KytheIndexerSemanticExtension>(options["kythe-indexer"].as<string>());
    }

    virtual std::unique_ptr<SemanticExtension> defaultInstance() const override {
        optional<string> outputPath;
        return make_unique<KytheIndexerSemanticExtension>(outputPath);
    }

    virtual ~KytheIndexerSemanticExtensionProvider(){};
};

vector<SemanticExtensionProvider *> SemanticExtensionProvider::getProviders() {
    static KytheIndexerSemanticExtensionProvider provider;
    return {&provider};
}
} // namespace sorbet::pipeline::semantic_extension