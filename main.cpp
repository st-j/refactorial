
#include <yaml-cpp/yaml.h>
#include "yaml-util.h"

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include <clang/Tooling/Refactoring.h>
#include <clang/Sema/SemaConsumer.h>
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"
#include <clang/Tooling/JSONCompilationDatabase.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <iostream>
#include <fstream>

#include <unistd.h>

using namespace clang;
using namespace clang::tooling;
using namespace std;

#include "Transforms/Transforms.h"

static llvm::cl::opt<bool> Help(
        "h",
        llvm::cl::desc("Alias for -help"),
        llvm::cl::Hidden);
static llvm::cl::opt<std::string> refactor_specifications(
        "refactor-specification-file",
        llvm::cl::desc("file with refactoring information, overrides stdin"),
        llvm::cl::Optional);
static llvm::cl::opt<std::string> BuildPath(
        "p",
        llvm::cl::desc("path to compile_commands.json"),
        llvm::cl::Required);

int main(int argc, const char **argv)
{
    llvm::cl::ParseCommandLineOptions(argc, argv);

    std::string ErrorMessage;
    clang::tooling::JSONCompilationDatabase* Compilations = \
        JSONCompilationDatabase::loadFromFile(BuildPath, ErrorMessage);

    std::vector<std::string> additionalCommandLine;
    additionalCommandLine.push_back("-isystem");
    additionalCommandLine.push_back("/usr/include/x86_64-linux-gnu/c++/4.8");

    if (!Compilations) {
        llvm::errs() << "barf " << BuildPath << " " << ErrorMessage << "\n";
        exit(0);
    }

    // read refactoring specifications. either read from stdin or file
    vector<YAML::Node> config;
    if (refactor_specifications.empty()) {
        llvm::errs() << "falling back to read from stdin\n";
        config = YAML::LoadAll(std::cin);
    } else {
        std::ifstream fin(refactor_specifications.c_str(), std::ifstream::binary);
        if (fin) {
            config = YAML::LoadAll(fin);
        } else {
            llvm::errs() << "barf: cannot open file '" << refactor_specifications << "'\n";
            exit(-1);
        }
        fin.close();
    }

    // iterate over refactoring specification
	for(auto configSectionIter = config.begin();
            configSectionIter != config.end();
            ++configSectionIter)
	{
		TransformRegistry::get().config = YAML::Node();

		//figure out which files we need to work on
		const YAML::Node& configSection = *configSectionIter;
		vector<string> inputFiles;
		if(configSection["Files"])
			inputFiles = configSection["Files"].as<vector<string> >();
		if(!configSection["Transforms"]) {
			llvm::errs() << "No transforms specified in this configuration section:\n";
			llvm::errs() << YAML::Dump(configSection) << "\n";
		}
		
		//load up the compilation database
        if (inputFiles.empty()) {
            inputFiles = Compilations->getAllFiles();
            llvm::errs() << "no input files in refactoring yml given. "
                << "will use all " << inputFiles.size() << " files "
                << "from compile_commands.json\n";
        }

        std::vector<std::string>::iterator kt;
        for(kt=inputFiles.begin();kt!=inputFiles.end();++kt) {

            std::vector<clang::tooling::CompileCommand> vec = \
                Compilations->getCompileCommands(*kt);
            std::vector<clang::tooling::CompileCommand>::iterator it;
            for(it=vec.begin();it!=vec.end();++it) {
                std::vector<std::string> cmd = it->CommandLine;
                std::vector<std::string>::iterator jt;
                for(jt=cmd.begin();jt!=cmd.end();++jt) {
                    llvm::errs() << "bla: " << *jt << "\n";
                }
            }
        }

		RefactoringTool rt(*Compilations, inputFiles);

		TransformRegistry::get().config = configSection["Transforms"];
		TransformRegistry::get().replacements = &rt.getReplacements();
		
		//finally, run
		for(auto iter = configSection["Transforms"].begin();
                iter != configSection["Transforms"].end();
                iter++)
		{
			
			llvm::errs() << iter->first.as<string>() +"Transform" << "\n";
			rt.run(new TransformFactory(TransformRegistry::get()[iter->first.as<string>() + "Transform"]));
		}
	}
	return 0;
}
