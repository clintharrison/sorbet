#include "ast/ast.h"
#include "cfg/CFG.h"
#include "common/formatting.h"
#include "kythe/json.h"
#include "main/options/options.h"
#include "main/pipeline/semantic_extension/SemanticExtension.h"
#include <cxxopts.hpp>
#include <optional>

using namespace std;

namespace sorbet::pipeline::semantic_extension {

class KytheIndexerSemanticExtension : public SemanticExtension {
public:
    KytheIndexerSemanticExtension() {}

    bool isFileStaticInit(const core::GlobalState &gs, core::MethodRef sym) const {
        auto name = sym.data(gs)->name;
        if (name.kind() != core::NameKind::UNIQUE) {
            return false;
        }
        return name.dataUnique(gs)->original == core::Names::staticInit();
    }

    bool isClassStaticInit(const core::GlobalState &gs, core::MethodRef sym) const {
        return sym.data(gs)->name == core::Names::staticInit();
    }

    bool isFileOrClassStaticInit(const core::GlobalState &gs, const core::MethodRef sym) const {
        return isFileStaticInit(gs, sym) || isClassStaticInit(gs, sym);
    }

    virtual void finishTypecheckFile(const core::GlobalState &gs, const core::FileRef &f) const override {
        auto fileVName = kythe::Json::vnameForFile(f.data(gs), "corpus"sv);
        rapidjson::OStreamWrapper osw{std::cout};
        kythe::Json::writeFact(osw, fileVName, "node/kind", "file");
    }

    virtual void finishTypecheck(const core::GlobalState &) const override {}

    virtual void typecheck(const core::GlobalState &gs, cfg::CFG &cfg, ast::MethodDef &md) const override {
        // auto filename = cfg.file.data(gs).path();

        // skip class static init for now?
        // if (isFileOrClassStaticInit(gs, md.symbol)) {
        //     return;
        // }

        for (auto &bb : cfg.basicBlocks) {
            for (auto &arg : bb->args) {
                fmt::print("arg: {}\n", arg.toString(gs, cfg));
            }

            for (auto &binding : bb->exprs) {
                // typecase(
                //     binding.value,
                //     [&](cfg::Ident &i) {
                //         fmt::print("ident: {}\n", i.what.data(cfg).)
                //         auto var = Payload::varGet(cs, i.what, builder, irctx, bb->rubyRegionId);
                //         Payload::varSet(cs, bind.bind.variable, var, builder, irctx, bb->rubyRegionId);
                //     },
                //     [&](cfg::Alias &i) {
                //         // We compute the alias map when IREmitterContext is first created, so if an entry is
                //         missing,
                //         // there's a problem.
                //         ENFORCE(irctx.aliases.find(bind.bind.variable) != irctx.aliases.end(),
                //                 "Alias is missing from the alias map");
                //     }, );
                fmt::print("binding {}\n", binding.value.toString(gs, cfg));
            }
            //     // fmt::print("\n{}\n\n", block->toString(gs, cfg));
        }

        fmt::print("finished {}\n", md.symbol.show(gs));
    }

    virtual void run(core::MutableContext &, ast::ClassDef *) const override {}

    virtual ~KytheIndexerSemanticExtension() {}

    virtual std::unique_ptr<SemanticExtension> deepCopy(const core::GlobalState &from, core::GlobalState &to) override {
        return make_unique<KytheIndexerSemanticExtension>();
    }

    virtual void merge(const core::GlobalState &from, core::GlobalState &to, core::NameSubstitution &subst) override {}
};

class KytheIndexerSemanticExtensionProvider : public SemanticExtensionProvider {
public:
    virtual void injectOptions(cxxopts::Options &opts) const override {}

    virtual std::unique_ptr<SemanticExtension> readOptions(cxxopts::ParseResult &options) const override {
        return make_unique<KytheIndexerSemanticExtension>();
    }

    virtual std::unique_ptr<SemanticExtension> defaultInstance() const override {
        return make_unique<KytheIndexerSemanticExtension>();
    }

    virtual ~KytheIndexerSemanticExtensionProvider(){};
};

vector<SemanticExtensionProvider *> SemanticExtensionProvider::getProviders() {
    static KytheIndexerSemanticExtensionProvider provider;
    return {&provider};
}
} // namespace sorbet::pipeline::semantic_extension