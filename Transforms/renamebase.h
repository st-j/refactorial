#ifndef RENAME_TRANSFORMS_H
#define RENAME_TRANSFORMS_H

#include "Transforms.h"
#include <pcrecpp.h>
#include <clang/Lex/Preprocessor.h>

class RenameTransform : public Transform {
public:
  RenameTransform();

protected:

    bool loadConfig(
        const std::string& transformName,
        const std::string& renameKeyName,
        const std::string& ignoreKeyName = "Ignore");

    bool shouldIgnore(clang::SourceLocation L);

    bool nameMatches(
        const clang::NamedDecl *decl,
        std::string &newName,
        bool checkOnly = false);

    bool stringMatches(std::string name, std::string &outNewName);

    bool stmtInSameFileAsDecl(clang::Stmt *S, clang::Decl *D);

    void renameLocation(clang::SourceLocation L, std::string& N);

    const std::string& indent();
    void pushIndent();
    void popIndent();

    std::string loc(clang::SourceLocation L);
    std::string range(clang::SourceRange R);

private:
    int indentLevel;
    std::string indentString;

    std::vector<pcrecpp::RE> ignoreList;

    typedef std::pair<pcrecpp::RE, std::string> REStringPair;
    std::vector<REStringPair> renameList;

    std::map<const clang::Decl *, std::string> nameMap;
    std::map<std::string, std::string> matchedStringMap;
    std::set<std::string> unmatchedStringSet;
};

#endif
