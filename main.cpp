
#include <yaml-cpp/yaml.h>
#include "yaml-util.h"

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include <clang/Tooling/Refactoring.h>
#include <clang/Sema/SemaConsumer.h>
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <iostream>
#include <fstream>

#include <unistd.h>

using namespace clang;
using namespace clang::tooling;
using namespace std;

#include "Transforms/Transforms.h"

static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::opt<std::string> refactor_specifications(
        llvm::cl::Required, "refactor-specification-file");

int main(int argc, const char **argv)
{	
    CommonOptionsParser OptionsParser(argc, argv);

    // read refactoring specification from stdin
    vector<YAML::Node> config;
    {
        std::cerr << "doing : " << refactor_specifications << "\n";
        std::ifstream fin; fin.open(refactor_specifications.c_str());
        config = YAML::LoadAll(fin);
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
        if (inputFiles.empty())
            inputFiles = OptionsParser.GetSourcePathList();
		RefactoringTool rt(OptionsParser.GetCompilations(), inputFiles);

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
