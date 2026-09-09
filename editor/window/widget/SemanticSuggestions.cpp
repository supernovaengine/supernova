// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "SemanticSuggestions.h"
#include "external/IconsFontAwesome6.h"
#include <algorithm>
#include <cmath>

namespace doriax::editor {

SemanticSuggestions::SemanticSuggestions()
    : minPrefixLength(1)
    , maxSuggestions(20)
    , fuzzyMatching(true)
    , caseSensitive(false)
{
}

SemanticSuggestions::~SemanticSuggestions() {
}

void SemanticSuggestions::SetKeywords(const std::unordered_set<std::string>& kw) {
    keywords = kw;
}

void SemanticSuggestions::SetTypes(const std::unordered_set<std::string>& t) {
    types = t;
}

void SemanticSuggestions::SetBuiltinFunctions(const std::unordered_set<std::string>& funcs) {
    builtinFunctions = funcs;
}

void SemanticSuggestions::AddSnippet(const std::string& trigger, const std::string& expansion, const std::string& description) {
    SuggestionItem item;
    item.label = trigger;
    item.insertText = expansion;
    item.detail = description;
    item.kind = SuggestionKind::Snippet;
    snippets.push_back(item);
}

void SemanticSuggestions::ClearSnippets() {
    snippets.clear();
}

void SemanticSuggestions::UpdateDocumentWords(const std::vector<std::string>& lines) {
    documentWords.clear();

    for (const auto& line : lines) {
        size_t i = 0;
        while (i < line.size()) {
            if (isWordChar(line[i])) {
                size_t start = i;
                while (i < line.size() && isWordChar(line[i])) ++i;
                std::string word = line.substr(start, i - start);
                // Only add identifiers with at least 2 characters; skip numbers and
                // words already covered by keyword/type/builtin sources (avoids duplicates)
                if (word.size() >= 2 && !std::isdigit(static_cast<unsigned char>(word[0])) &&
                    keywords.find(word) == keywords.end() &&
                    types.find(word) == types.end() &&
                    builtinFunctions.find(word) == builtinFunctions.end()) {
                    documentWords.insert(word);
                }
            } else {
                ++i;
            }
        }
    }
}

void SemanticSuggestions::AddSymbol(const std::string& name, SuggestionKind kind, const std::string& detail, const std::string& parentType, const std::string& typeInfo) {
    SuggestionItem item;
    item.label = name;
    item.insertText = name;
    item.detail = detail;
    item.parentType = parentType;
    item.typeInfo = typeInfo;
    item.kind = kind;
    symbols.push_back(item);
}

void SemanticSuggestions::ClearSymbols() {
    symbols.clear();
}

void SemanticSuggestions::SetClassParent(const std::string& className, const std::string& parentClass) {
    if (!className.empty() && !parentClass.empty()) {
        classParentMap[className] = parentClass;
    }
}

void SemanticSuggestions::ClearInheritance() {
    classParentMap.clear();
}

bool SemanticSuggestions::isTypeOrAncestor(const std::string& targetType, const std::string& symbolParent) const {
    if (symbolParent == targetType) return true;
    // Walk up the inheritance chain from targetType
    std::string current = targetType;
    int depth = 0;
    while (depth < 20) { // prevent infinite loops
        auto it = classParentMap.find(current);
        if (it == classParentMap.end()) break;
        current = it->second;
        if (current == symbolParent) return true;
        depth++;
    }
    return false;
}

std::string SemanticSuggestions::FindSymbolType(const std::string& name) const {
    for (const auto& symbol : symbols) {
        if (symbol.label == name && !symbol.typeInfo.empty()) {
            return symbol.typeInfo;
        }
    }
    return "";
}

bool SemanticSuggestions::IsKnownClassOrEnum(const std::string& name) const {
    if (name.empty()) return false;
    for (const auto& symbol : symbols) {
        if ((symbol.kind == SuggestionKind::Class || symbol.kind == SuggestionKind::Enum) &&
            symbol.label == name) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> SemanticSuggestions::FindSignatures(const std::string& functionName, const std::string& parentType) const {
    std::vector<std::string> results;
    for (const auto& symbol : symbols) {
        if (symbol.label != functionName) continue;
        if (symbol.kind != SuggestionKind::Method && symbol.kind != SuggestionKind::Function) continue;
        // If parentType specified, match it (including ancestors)
        if (!parentType.empty() && !symbol.parentType.empty()) {
            if (!isTypeOrAncestor(parentType, symbol.parentType)) continue;
        }
        // Only real signatures (must contain a parameter list), no duplicates —
        // generic details like "project function" are not overloads
        if (symbol.detail.empty() || symbol.detail.find('(') == std::string::npos) continue;
        if (std::find(results.begin(), results.end(), symbol.detail) == results.end()) {
            results.push_back(symbol.detail);
        }
    }
    return results;
}

std::vector<SuggestionItem> SemanticSuggestions::GetSuggestions(const SuggestionContext& context) {
    std::vector<SuggestionItem> results;

    const std::string& query = context.currentWord;

    bool isMemberAccess = (context.afterDot || context.afterArrow || context.afterDoubleColon || context.afterColon);

    // Don't suggest if query is too short (unless after member access or explicit Ctrl+Space)
    if (query.length() < static_cast<size_t>(minPrefixLength) &&
        !isMemberAccess && !context.manualInvoke) {
        return results;
    }

    // Member access needs a valid receiver: skip after ')', numeric literals ("3."), etc.
    if (isMemberAccess &&
        (context.previousWord.empty() || std::isdigit(static_cast<unsigned char>(context.previousWord[0])))) {
        return results;
    }

    // If we have a target type, we strictly filter for members of that type (and don't add keywords/globals)
    bool restrictToType = isMemberAccess && !context.targetType.empty();

    if (!isMemberAccess) {
        addCandidates(results, keywords, SuggestionKind::Keyword, query);
        addCandidates(results, types, SuggestionKind::Type, query);
        addCandidates(results, builtinFunctions, SuggestionKind::Function, query);
        addCandidates(results, documentWords, SuggestionKind::Variable, query);

        // Add snippets
        for (const auto& snippet : snippets) {
            int score;
            if (computeMatch(snippet.label, query, score)) {
                SuggestionItem item = snippet;
                item.score = score + 5;
                results.push_back(item);
            }
        }
    } else if (!restrictToType && !query.empty()) {
        // Member access on an unknown type: fall back to plain word suggestions
        // once the user typed at least one character (like VSCode's word-based completion)
        addCandidates(results, documentWords, SuggestionKind::Text, query);
    }

    // Add custom symbols (this includes our engine API methods & properties)
    for (const auto& symbol : symbols) {
        // In C++, skip Lua-only properties (e.g. "alpha", "color") — use getter/setter methods instead
        if (context.isCpp && symbol.kind == SuggestionKind::Property && !symbol.parentType.empty()) {
            continue;
        }

        if (isMemberAccess) {
            // Members of an unknown type are never shown (word fallback handled above)
            if (!restrictToType) continue;
            // Only members of the target type or its ancestors
            if (symbol.parentType.empty()) continue;
            if (!isTypeOrAncestor(context.targetType, symbol.parentType)) continue;
            // Lua ':' is method-call syntax — only callables make sense there
            if (context.afterColon &&
                symbol.kind != SuggestionKind::Method && symbol.kind != SuggestionKind::Function) {
                continue;
            }
        } else {
            // Global scope: top-level symbols (classes, enums, free functions), plus the
            // members of the class the cursor sits in. Everything else needs an accessor
            if (!symbol.parentType.empty() &&
                (context.enclosingType.empty() || !isTypeOrAncestor(context.enclosingType, symbol.parentType))) {
                continue;
            }
        }

        int score;
        if (computeMatch(symbol.label, query, score)) {
            SuggestionItem item = symbol;
            item.score = score;

            // Boost exact parentType match (direct members over inherited)
            if (restrictToType && symbol.parentType == context.targetType) {
                item.score += 50;
            }

            // Own members outrank globals, and declared ones outrank inherited
            if (!isMemberAccess && !symbol.parentType.empty()) {
                item.score += (symbol.parentType == context.enclosingType) ? 20 : 10;
            }

            // Boost kinds that fit the accessor used
            if (context.afterDot) {
                if (context.isCpp) {
                    // C++ '.' is full member access: methods and fields first
                    if (symbol.kind == SuggestionKind::Method || symbol.kind == SuggestionKind::Field ||
                        symbol.kind == SuggestionKind::Function || symbol.kind == SuggestionKind::Constant ||
                        symbol.kind == SuggestionKind::EnumMember) {
                        item.score += 15;
                    }
                } else if (symbol.kind == SuggestionKind::Property || symbol.kind == SuggestionKind::Field ||
                           symbol.kind == SuggestionKind::Constant || symbol.kind == SuggestionKind::EnumMember ||
                           symbol.kind == SuggestionKind::Function) {
                    // Lua '.' favors properties/constants/statics (methods use ':')
                    item.score += 15;
                }
            }
            if (context.afterArrow &&
                (symbol.kind == SuggestionKind::Method || symbol.kind == SuggestionKind::Field)) {
                item.score += 15;
            }
            if (context.afterDoubleColon &&
                (symbol.kind == SuggestionKind::Function || symbol.kind == SuggestionKind::Constant ||
                 symbol.kind == SuggestionKind::EnumMember)) {
                item.score += 15;
            }

            // Prefer the symbol entry (has detail/doc) over the bare Type candidate of the same name
            if (!isMemberAccess &&
                (symbol.kind == SuggestionKind::Class || symbol.kind == SuggestionKind::Enum)) {
                item.score += 9;
            }

            results.push_back(item);
        }
    }

    // Remove duplicates by label, preferring higher score. Snippets are kept
    // separate so e.g. the "if" snippet survives next to the "if" keyword.
    std::unordered_map<std::string, size_t> seen;
    std::vector<SuggestionItem> unique;
    for (const auto& item : results) {
        if (item.kind == SuggestionKind::Snippet) {
            unique.push_back(item);
            continue;
        }
        auto it = seen.find(item.label);
        if (it == seen.end()) {
            seen[item.label] = unique.size();
            unique.push_back(item);
        } else {
            // Keep the one with higher score or better kind
            if (item.score > unique[it->second].score) {
                unique[it->second] = item;
            }
        }
    }
    results = std::move(unique);

    // Remove the exact match of what's being typed (snippets stay — they expand to more text)
    results.erase(
        std::remove_if(results.begin(), results.end(),
            [&query, this](const SuggestionItem& item) {
                if (item.kind == SuggestionKind::Snippet) return false;
                std::string itemLower = caseSensitive ? item.label : toLower(item.label);
                std::string queryLower = caseSensitive ? query : toLower(query);
                return itemLower == queryLower;
            }),
        results.end()
    );

    // Sort by score (descending), then shorter labels first, then alphabetically
    std::sort(results.begin(), results.end(), [](const SuggestionItem& a, const SuggestionItem& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.label.size() != b.label.size()) return a.label.size() < b.label.size();
        return a.label < b.label;
    });

    // Limit results
    if (results.size() > static_cast<size_t>(maxSuggestions)) {
        results.resize(maxSuggestions);
    }

    return results;
}

void SemanticSuggestions::addCandidates(std::vector<SuggestionItem>& results,
                                         const std::unordered_set<std::string>& source,
                                         SuggestionKind kind,
                                         const std::string& query) {
    for (const auto& word : source) {
        int score;
        if (computeMatch(word, query, score)) {
            SuggestionItem item;
            item.label = word;
            item.insertText = word;
            item.kind = kind;
            item.score = score;

            // Boost score for keywords and types
            if (kind == SuggestionKind::Keyword) item.score += 10;
            if (kind == SuggestionKind::Type) item.score += 8;
            if (kind == SuggestionKind::Function) item.score += 6;

            results.push_back(item);
        }
    }
}

bool SemanticSuggestions::isWordBoundary(const std::string& candidate, size_t i) {
    if (i == 0) return true;
    unsigned char c = static_cast<unsigned char>(candidate[i]);
    unsigned char p = static_cast<unsigned char>(candidate[i - 1]);
    if (p == '_' || p == '-' || p == '.') return true;
    if (std::isupper(c) && !std::isupper(p)) return true;      // camelCase hump
    if (std::isdigit(c) && !std::isdigit(p)) return true;      // letter→digit transition
    return false;
}

// Tiered matching, modeled after VSCode's completion filter:
//   1. prefix match           (strongest — what most users expect)
//   2. substring starting at a word boundary ("Pos" -> setPosition)
//   3. boundary-anchored subsequence ("sP" / "spos" -> setPosition);
//      every query char must continue a run or start at a word boundary
//   4. plain substring anywhere
// Scattered fuzzy matches (e.g. "se" -> "translate") are rejected, which is
// the main thing that keeps the list short and precise.
bool SemanticSuggestions::computeMatch(const std::string& candidate, const std::string& query, int& outScore) const {
    outScore = 0;
    if (query.empty()) return true; // Show all candidates when no filter typed yet
    if (query.length() > candidate.length()) return false;

    std::string candCmp = caseSensitive ? candidate : toLower(candidate);
    std::string queryCmp = caseSensitive ? query : toLower(query);

    // Tier 1: prefix match
    if (candCmp.compare(0, queryCmp.size(), queryCmp) == 0) {
        outScore = 2000;
        if (candidate.compare(0, query.size(), query) == 0) outScore += 100; // exact case
        return true;
    }

    if (!fuzzyMatching) return false;

    // Tier 2: substring beginning at a word boundary (e.g. "Pos" in "setPosition")
    size_t found = candCmp.find(queryCmp);
    while (found != std::string::npos) {
        if (isWordBoundary(candidate, found)) {
            outScore = 1200 - static_cast<int>(std::min<size_t>(found, 100)); // earlier is better
            return true;
        }
        found = candCmp.find(queryCmp, found + 1);
    }

    // Tier 3: boundary-anchored subsequence (camelCase / snake_case initials)
    {
        size_t qi = 0;
        size_t lastMatch = std::string::npos;
        int bonus = 0;
        for (size_t i = 0; i < candCmp.size() && qi < queryCmp.size(); ++i) {
            if (candCmp[i] != queryCmp[qi]) continue;
            bool consecutive = (lastMatch != std::string::npos && i == lastMatch + 1);
            bool boundary = isWordBoundary(candidate, i);
            // First query char must start at the very beginning of the candidate
            if (qi == 0 && i != 0) break;
            if (!consecutive && !boundary) continue;
            if (boundary && qi > 0) bonus += 10;
            if (consecutive) bonus += 5;
            lastMatch = i;
            qi++;
        }
        if (qi == queryCmp.size()) {
            outScore = 800 + std::min(bonus, 150);
            return true;
        }
    }

    // Tier 4: plain substring anywhere — only for queries long enough to be
    // intentional. Short unanchored substrings ("et" in "setPosition") are the
    // main source of noise, and Tiers 1-3 already cover short queries precisely.
    if (queryCmp.size() >= 3 && candCmp.find(queryCmp) != std::string::npos) {
        outScore = 400;
        return true;
    }

    return false;
}

std::string SemanticSuggestions::toLower(const std::string& str) const {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

bool SemanticSuggestions::isWordChar(char c) const {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

const char* SemanticSuggestions::GetKindIcon(SuggestionKind kind) {
    switch (kind) {
        case SuggestionKind::Keyword:    return ICON_FA_KEY;
        case SuggestionKind::Type:       return ICON_FA_CUBE;
        case SuggestionKind::Function:   return ICON_FA_CODE;
        case SuggestionKind::Variable:   return ICON_FA_V;
        case SuggestionKind::Snippet:    return ICON_FA_PUZZLE_PIECE;
        case SuggestionKind::Class:      return ICON_FA_C;
        case SuggestionKind::Interface:  return ICON_FA_I;
        case SuggestionKind::Module:     return ICON_FA_BOX;
        case SuggestionKind::Property:   return ICON_FA_WRENCH;
        case SuggestionKind::Method:     return ICON_FA_M;
        case SuggestionKind::Field:      return ICON_FA_F;
        case SuggestionKind::Constant:   return ICON_FA_LOCK;
        case SuggestionKind::Enum:       return ICON_FA_LIST;
        case SuggestionKind::EnumMember: return ICON_FA_LIST_OL;
        case SuggestionKind::Operator:   return ICON_FA_PLUS_MINUS;
        case SuggestionKind::Text:       
        default:                         return ICON_FA_FONT;
    }
}

ImVec4 SemanticSuggestions::GetKindColor(SuggestionKind kind) {
    switch (kind) {
        case SuggestionKind::Keyword:    return ImVec4(0.78f, 0.44f, 0.86f, 1.0f);  // Purple
        case SuggestionKind::Type:       return ImVec4(0.31f, 0.69f, 0.78f, 1.0f);  // Cyan
        case SuggestionKind::Function:   return ImVec4(0.86f, 0.82f, 0.53f, 1.0f);  // Yellow
        case SuggestionKind::Variable:   return ImVec4(0.61f, 0.82f, 0.98f, 1.0f);  // Light blue
        case SuggestionKind::Snippet:    return ImVec4(0.98f, 0.61f, 0.61f, 1.0f);  // Light red
        case SuggestionKind::Class:      return ImVec4(0.98f, 0.78f, 0.31f, 1.0f);  // Orange
        case SuggestionKind::Interface:  return ImVec4(0.31f, 0.98f, 0.78f, 1.0f);  // Teal
        case SuggestionKind::Module:     return ImVec4(0.78f, 0.98f, 0.31f, 1.0f);  // Lime
        case SuggestionKind::Property:   return ImVec4(0.61f, 0.61f, 0.98f, 1.0f);  // Lavender
        case SuggestionKind::Method:     return ImVec4(0.86f, 0.82f, 0.53f, 1.0f);  // Yellow (same as function)
        case SuggestionKind::Field:      return ImVec4(0.61f, 0.82f, 0.98f, 1.0f);  // Light blue (same as variable)
        case SuggestionKind::Constant:   return ImVec4(0.31f, 0.69f, 0.78f, 1.0f);  // Cyan
        case SuggestionKind::Enum:       return ImVec4(0.98f, 0.78f, 0.31f, 1.0f);  // Orange
        case SuggestionKind::EnumMember: return ImVec4(0.78f, 0.61f, 0.31f, 1.0f);  // Brown
        case SuggestionKind::Operator:   return ImVec4(0.86f, 0.86f, 0.86f, 1.0f);  // Gray
        case SuggestionKind::Text:       
        default:                         return ImVec4(0.86f, 0.86f, 0.86f, 1.0f);  // Gray
    }
}

const char* SemanticSuggestions::GetKindName(SuggestionKind kind) {
    switch (kind) {
        case SuggestionKind::Keyword:    return "Keyword";
        case SuggestionKind::Type:       return "Type";
        case SuggestionKind::Function:   return "Function";
        case SuggestionKind::Variable:   return "Variable";
        case SuggestionKind::Snippet:    return "Snippet";
        case SuggestionKind::Class:      return "Class";
        case SuggestionKind::Interface:  return "Interface";
        case SuggestionKind::Module:     return "Module";
        case SuggestionKind::Property:   return "Property";
        case SuggestionKind::Method:     return "Method";
        case SuggestionKind::Field:      return "Field";
        case SuggestionKind::Constant:   return "Constant";
        case SuggestionKind::Enum:       return "Enum";
        case SuggestionKind::EnumMember: return "Enum Member";
        case SuggestionKind::Operator:   return "Operator";
        case SuggestionKind::Text:       
        default:                         return "Text";
    }
}

} // namespace doriax::editor
