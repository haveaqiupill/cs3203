#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../../Commons/Relationship.h"
#include "../Query/QueryArgType.h"

/*
* Lookup table storing information
* on what kind of replacements can be done for the different relationships.
* For example, Follows can only have arguments replaced with numbers, not strings.
*/
const std::unordered_map <QueryArgType,
    std::unordered_set<Relationship>> rsReplacementLookup{
    {QueryArgType::NUM,
    {Relationship::FOLLOWS,
    Relationship::FOLLOWS_STAR,
    Relationship::PARENT,
    Relationship::NEXT,
    Relationship::NEXT_STAR,
    Relationship::AFFECTS,
    Relationship::AFFECTS_STAR,
    Relationship::NEXT_BIP,
    Relationship::NEXT_BIP_STAR,
    Relationship::AFFECTS_BIP}
    },

    {QueryArgType::NAME,
    {Relationship::MODIFIES,
    Relationship::USES,
    Relationship::CALLS,
    Relationship::CALLS_STAR}
    }
};
