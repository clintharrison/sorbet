#include "main/lsif/writer.h"
#include "core/core.h"
#include "core/lsp/Task.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include <optional>

namespace sorbet::realmain::lsif {
// https://microsoft.github.io/language-server-protocol/specifications/lsif/0.5.0/specification/

int Writer::emitNext(int outV, int inV) {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};
    writer.StartObject();
    writer.String("id");
    writer.Int(id);
    writer.String("type");
    writer.String("vertex");
    writer.String("outV");
    writer.Int(outV);
    writer.String("inV");
    writer.Int(inV);
    writer.EndObject();
    outputStream.put('\n');

    return id;
}

int Writer::emitResultSet() {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};
    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("resultSet");

    writer.EndObject();
    outputStream.put('\n');

    return id;
}

int Writer::emitDefinitionResult() {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};
    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("definitionResult");

    writer.EndObject();
    outputStream.put('\n');

    return id;
}

int Writer::emitContains(int outV, std::vector<int> &inVs) {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("edge");

    writer.String("outV");
    writer.Int(outV);

    writer.String("inVs");
    writer.StartArray();
    for (auto inV : inVs) {
        writer.Int(inV);
    }
    writer.EndArray();
    writer.EndObject();
    outputStream.put('\n');

    return id;
}

int Writer::emitProject(std::string_view language) {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("project");

    writer.String("kind");
    writer.String(language.data());

    writer.EndObject();
    outputStream.put('\n');

    return id;
}

int Writer::emitRange(LsifPosition start, LsifPosition end) {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("range");

    writer.String("start");
    writer.StartObject();
    writer.String("line");
    writer.Int(start.line);
    writer.String("character");
    writer.Int(start.character);
    writer.EndObject();

    writer.String("end");
    writer.StartObject();
    writer.String("line");
    writer.Int(end.line);
    writer.String("character");
    writer.Int(end.character);
    writer.EndObject();

    writer.EndObject();
    outputStream.put('\n');

    return id;
}

int Writer::emitDocument(std::string_view language, std::string_view uri) {
    rapidjson::OStreamWrapper osw{outputStream};
    auto id = nextId++;
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("document");

    writer.String("uri");
    writer.String(uri.data());

    writer.String("languageId");
    writer.String(language.data());

    writer.EndObject();
    outputStream.put('\n');

    return id;
}

} // namespace sorbet::realmain::lsif