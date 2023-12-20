#pragma once

#include <string>
#include <unordered_map>

#include "Attribute.h"

typedef std::string SYNONYM;

class Declaration {
 public:
    Declaration();
    Declaration(SYNONYM& syn, Attribute attr);
    SYNONYM getSynonym();
    Attribute getAttribute();
    bool hasAttribute();
    void assignDefaultAttribute(Attribute attr);
    bool operator==(Declaration other) const;
    size_t getHash() const;
    void replaceSynonym(std::string& syn);
 private:
    SYNONYM synonym;
    Attribute attribute;
};

struct dec_hash {
    size_t operator()(const Declaration& dec) const {
        const size_t hash = dec.getHash();
        return hash;
    }
};
