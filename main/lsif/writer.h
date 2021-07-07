#ifndef SORBET_MAIN_LSIF_WRITER_H
#define SORBET_MAIN_LSIF_WRITER_H

#include "core/core.h"

namespace sorbet::realmain::lsif {
struct LsifPosition {
    int line;
    int character;
};

class Writer {
    std::ostream &outputStream;
    int nextId;

public:
    Writer(std::ostream &out) : outputStream(out), nextId(0) {}

    int emitResultSet();
    int emitDefinitionResult();
    int emitNext(int outV, int inV);
    int emitRange(sorbet::core::Loc::Detail start, sorbet::core::Loc::Detail end);
    int emitDocument(std::string_view language, std::string_view uri);
    int emitProject(std::string_view language);
    int emitContains(int outV, std::vector<int> &inVs);
};
} // namespace sorbet::realmain::lsif

#endif