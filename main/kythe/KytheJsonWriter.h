#ifndef SORBET_MAIN_KYTHE_KYTHEJSONWRITER_H
#define SORBET_MAIN_KYTHE_KYTHEJSONWRITER_H

#include "absl/strings/escaping.h"
#include "main/kythe/VName.h"
#include "rapidjson/document.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"

namespace sorbet::realmain::kythe {

class KytheJsonWriter {
public:
    template <typename T> static void writeVName(rapidjson::Writer<T> &writer, const VName &node) {
        writer.StartObject();

        writer.String("signature");
        writer.String(node.getSignature().data());

        if (!node.getPath().empty()) {
            writer.String("path");
            writer.String(node.getPath().data());
        }

        if (!node.getLanguage().empty()) {
            writer.String("language");
            writer.String(node.getLanguage().data());
        }

        if (!node.getRoot().empty()) {
            writer.String("root");
            writer.String(node.getRoot().data());
        }

        if (!node.getCorpus().empty()) {
            // writer.String("corpus");
            // writer.String(node.getCorpus().data());
        }

        writer.EndObject();
    }

    template <typename T>
    static void writeFact(rapidjson::Writer<T> &writer, const VName &node, const std::string_view &fact_name,
                          const std::string_view &fact_val) {
        writer.StartObject();

        writer.String("source");
        writeVName(writer, node);

        writer.String("fact_name");
        writer.String(fmt::format("/kythe/{}", fact_name));

        writer.String("fact_value");
        writer.String(absl::Base64Escape(fact_val));

        writer.EndObject();
    }

    template <typename T>
    static void writeEdge(rapidjson::Writer<T> &writer, const VName &source, const std::string &edge_name,
                          const VName &target) {
        writer.StartObject();

        writer.String("source");
        writeVName(writer, source);

        writer.String("edge_kind");
        writer.String(fmt::format("/kythe/edge/{}", edge_name));

        writer.String("target");
        writeVName(writer, target);

        writer.String("fact_name");
        writer.String("/");

        writer.EndObject();
    }

    template <typename T>
    static void writeOrdinalEdge(rapidjson::Writer<T> writer, const VName &source, const std::string &edge_name,
                                 const VName &target, unsigned int ordinal) {
        writer.StartObject();

        writer.String("source");
        writeVName(writer, source);

        writer.String("edge_kind");
        writer.String(fmt::format("/kythe/edge/{}.{}"), edge_name, ordinal);

        writer.String("target");
        writeVName(writer, target);

        writer.String("fact_name");
        writer.String("/");

        writer.EndObject();
    }

    static VName vnameForAnchor(VName fileVName, uint begin, uint end) {
        auto signature = fmt::format("@{}:{}", begin, end);
        return VName{signature, fileVName.getPath().data(), fileVName.getLanguage().data(), fileVName.getRoot().data(),
                     fileVName.getCorpus().data()};
    }

    static VName vnameForFile(const core::File &f, std::string_view corpus) {
        VName vname{"", std::string{f.path()}, "", "", corpus.data()};
        return vname;
    }
};

} // namespace sorbet::realmain::kythe

#endif