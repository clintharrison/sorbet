#include "core/lsif/writer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <optional>

// core "write in LSIF format" functions, keeping track of IDs
// any non-writing logic should go elsewhere
namespace sorbet::core::lsif {
// https://microsoft.github.io/language-server-protocol/specifications/lsif/0.5.0/specification/

// TODO: stop hardcoding this?
Writer::Writer() : outputStream(std::cout), nextId(1) {}

int Writer::emitEdge(std::string_view label, int outV, int inV) {
    return emitEdge(nextId++, label, outV, inV);
}

int Writer::emitNextEdge(int outV, int inV) {
    return emitNextEdge(nextId++, outV, inV);
}

int Writer::emitResultSet() {
    return emitResultSet(nextId++);
}

int Writer::emitVertex(std::string_view label) {
    return emitVertex(nextId++, label);
}

int Writer::emitContains(int outV, const std::vector<int> &inVs) {
    return emitContains(nextId++, outV, inVs);
}

int Writer::emitProject(std::string_view language) {
    return emitProject(nextId++, language);
}

int Writer::emitRange(sorbet::core::Loc::Detail start, sorbet::core::Loc::Detail end) {
    return emitRange(nextId++, start, end);
}

int Writer::emitDocument(std::string_view language, std::string_view uri) {
    return emitDocument(nextId++, language, uri);
}

int Writer::emitMetaData(std::string_view version, std::string_view projectRoot, std::string_view toolName) {
    return emitMetaData(nextId++, version, projectRoot, toolName);
}

int Writer::emitItemEdge(int outV, const std::vector<int> &inVs, int shard, std::string_view property) {
    return emitItemEdge(nextId++, outV, inVs, shard, property);
}

int Writer::emitItemEdge(int outV, int inV, int shard, std::string_view property) {
    return emitItemEdge(nextId++, outV, std::vector<int>{inV}, shard, property);
}

int Writer::emitHoverResult(std::string_view kind, std::string_view value) {
    return emitHoverResult(nextId++, kind, value);
}

int Writer::emitGroup(std::string_view uri, std::string_view conflictResolution, std::string_view name,
                      std::string_view rootUri) {
    return emitGroup(nextId++, uri, conflictResolution, name, rootUri);
}

int Writer::emitEdge(int id, std::string_view label, int outV, int inV) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};
    writer.StartObject();
    writer.String("id");
    writer.Int(id);
    writer.String("type");
    writer.String("edge");
    writer.String("label");
    writer.String(label.data());
    writer.String("outV");
    writer.Int(outV);
    writer.String("inV");
    writer.Int(inV);
    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitNextEdge(int id, int outV, int inV) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};
    writer.StartObject();
    writer.String("id");
    writer.Int(id);
    writer.String("type");
    writer.String("edge");
    writer.String("label");
    writer.String("next");
    writer.String("outV");
    writer.Int(outV);
    writer.String("inV");
    writer.Int(inV);
    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitResultSet(int id) {
    rapidjson::OStreamWrapper osw{outputStream};
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
    outputStream.flush();

    return id;
}

int Writer::emitVertex(int id, std::string_view label) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};
    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String(label.data());

    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitContains(int id, int outV, const std::vector<int> &inVs) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("edge");

    writer.String("label");
    writer.String("contains");

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
    outputStream.flush();

    return id;
}

int Writer::emitProject(int id, std::string_view language) {
    rapidjson::OStreamWrapper osw{outputStream};
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
    outputStream.flush();

    return id;
}

int Writer::emitRange(int id, sorbet::core::Loc::Detail start, sorbet::core::Loc::Detail end) {
    rapidjson::OStreamWrapper osw{outputStream};
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
    writer.Int(start.line - 1);
    writer.String("character");
    writer.Int(start.column - 1);
    writer.EndObject();

    writer.String("end");
    writer.StartObject();
    writer.String("line");
    writer.Int(end.line - 1);
    writer.String("character");
    writer.Int(end.column - 1);
    writer.EndObject();

    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitDocument(int id, std::string_view language, std::string_view uri) {
    rapidjson::OStreamWrapper osw{outputStream};
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
    outputStream.flush();

    return id;
}

int Writer::emitMetaData(int id, std::string_view version, std::string_view projectRoot, std::string_view toolName) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("metaData");

    writer.String("version");
    writer.String(version.data());

    writer.String("projectRoot");
    writer.String(projectRoot.data());

    writer.String("toolInfo");
    writer.StartObject();
    writer.String("name");
    writer.String("sorbet");
    writer.EndObject();

    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitItemEdge(int id, int outV, const std::vector<int> &inVs, int shard, std::string_view property) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("edge");

    writer.String("label");
    writer.String("item");

    writer.String("outV");
    writer.Int(outV);

    writer.String("inVs");
    writer.StartArray();
    for (auto inV : inVs) {
        writer.Int(inV);
    }
    writer.EndArray();

    writer.String("shard");
    writer.Int(shard);

    if (!property.empty()) {
        writer.String("property");
        writer.String(property.data());
    }

    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitHoverResult(int id, std::string_view kind, std::string_view value) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("hoverResult");

    writer.String("result");
    writer.StartObject();

    // Alongside contents, we can additionally include a range.
    // But currently we have no reason to...
    // What do other LSIF indexers do?
    writer.String("contents");
    writer.StartObject();

    writer.String("kind");
    writer.String(kind.data());

    writer.String("value");
    writer.String(value.data());

    writer.EndObject();
    writer.EndObject();
    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

int Writer::emitGroup(int id, std::string_view uri, std::string_view conflictResolution, std::string_view name,
                      std::string_view rootUri) {
    rapidjson::OStreamWrapper osw{outputStream};
    rapidjson::Writer writer{osw};

    writer.StartObject();
    writer.String("id");
    writer.Int(id);

    writer.String("type");
    writer.String("vertex");

    writer.String("label");
    writer.String("group");

    writer.String("uri");
    writer.String(uri.data());

    writer.String("conflictResolution");
    writer.String(conflictResolution.data());

    writer.String("name");
    writer.String(name.data());

    writer.String("rootUri");
    writer.String(rootUri.data());

    writer.EndObject();
    outputStream.put('\n');
    outputStream.flush();

    return id;
}

} // namespace sorbet::core::lsif