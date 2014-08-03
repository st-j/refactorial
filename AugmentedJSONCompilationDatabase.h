#ifndef AUGMENTEDJSONCOMPILATIONDATABASE_H
#define AUGMENTEDJSONCOMPILATIONDATABASE_H

#include "clang/Tooling/JSONCompilationDatabase.h"

namespace clang {
namespace tooling {
    class AugmentedJSONCompilationDatabase : public JSONCompilationDatabase {

    private:
        std::vector<std::string> additionalCommands;

    public:
        void setAppended(const std::vector<std::string> cmds) {
            additionalCommands = cmds;
        }
        void addAppended(const std::string cmd) {
            additionalCommands.push_back(cmd);
        }

        std::vector<CompileCommand>
        getCompileCommands(StringRef FilePath) const override;

        static AugmentedJSONCompilationDatabase *loadFromFile(
                StringRef FilePath, std::string &ErrorMessage);

    };
}
}

#endif /* !AUGMENTEDJSONCOMPILATIONDATABASE_H */

