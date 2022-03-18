#ifndef SORBET_MAIN_KYTHE_VNAME_H
#define SORBET_MAIN_KYTHE_VNAME_H

#include "absl/strings/str_replace.h"
#include "ast/ast.h"
#include "core/core.h"

namespace sorbet::kythe {

class VName {
public:
    VName() : signature("<root>"), path(""), language(""), root(""), corpus("") {}

    VName(std::string signature, std::string path, std::string language, std::string root, std::string corpus)
        : signature(signature), path(path), language(language), root(root), corpus(corpus) {}

    std::string_view getSignature() const {
        return signature;
    }

    std::string_view getPath() const {
        return path;
    }

    std::string_view getLanguage() const {
        return language;
    }

    std::string_view getRoot() const {
        return root;
    }

    std::string_view getCorpus() const {
        return corpus;
    }

    std::string toKytheUriString() const {
        auto c = corpus.empty() ? "" : "//" + uriEscape(language);
        auto l = language.empty() ? "" : "?lang=" + uriEscape(language);
        auto p = path.empty() ? "" : "?path=" + uriEscape(path);
        auto r = root.empty() ? "" : "?root=" + uriEscape(root);
        auto s = signature.empty() ? "" : "#" + uriEscape(signature);
        return "kythe:" + c + l + p + r + s;
    }

private:
    std::string signature;
    std::string path;
    std::string language;
    std::string root;
    std::string corpus;

    static std::string uriEscape(std::string_view str) {
        // TODO: do this a real way
        return absl::StrReplaceAll(str, {
                                            {"#", "%23"},
                                            {"/", "%2F"},
                                            {":", "%3A"},
                                            {"@", "%40"},
                                        });
    }
};

} // namespace sorbet::kythe

#endif
