#include "Declaration.h"

Declaration::Declaration() {
}

Declaration::Declaration(SYNONYM& syn, Attribute attr) : synonym(syn), attribute(attr) {
}

SYNONYM Declaration::getSynonym() {
    return this->synonym;
}

Attribute Declaration::getAttribute() {
    return this->attribute;
}

bool Declaration::hasAttribute() {
    return this->attribute != Attribute::NONE;
}

void Declaration::assignDefaultAttribute(Attribute attr) {
    this->attribute = attr;
}

bool Declaration::operator==(Declaration other) const {
    return this->synonym == other.synonym &&
        this->attribute == other.attribute;
}

size_t Declaration::getHash() const {
    size_t str_hash = std::hash<std::string>{}(synonym);
    size_t int_hash = std::hash<int>{}(static_cast<int>(attribute));
    size_t seed = 0;
    seed ^= str_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= int_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    return seed;
}

void Declaration::replaceSynonym(std::string& syn) {
    this->synonym = syn;
}
