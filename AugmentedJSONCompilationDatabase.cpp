// 
// AugmentedJSONCompilationDatabase.cpp
// 
// 2014 mzenzes
// 
#include "AugmentedJSONCompilationDatabase.h"

#include "clang/Tooling/JSONCompilationDatabase.h"
#include <memory>

namespace clang {
namespace tooling {


std::vector<CompileCommand>
AugmentedJSONCompilationDatabase::getCompileCommands(StringRef FilePath) const
{
    std::vector<CompileCommand> retval;
    retval = JSONCompilationDatabase::getCompileCommands(FilePath);

    std::vector<CompileCommand>::iterator it;
    for(it=retval.begin();it!=retval.end();++it) {
        it->CommandLine.insert(
                it->CommandLine.end(),
                additionalCommands.begin(),
                additionalCommands.end());
    }

    return retval;
};

AugmentedJSONCompilationDatabase *
AugmentedJSONCompilationDatabase::loadFromFile(
        StringRef FilePath, std::string &ErrorMessage)
{
    JSONCompilationDatabase* db = \
        JSONCompilationDatabase::loadFromFile(FilePath, ErrorMessage);

    return NULL;
};

}
}
