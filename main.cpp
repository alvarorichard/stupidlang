#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int main(int argc, char **argv) {
    // Uso: ./hello_compiler <arquivo fonte>
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo fonte>" << std::endl;
        return 1;
    }

    // Lê o arquivo fonte
    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Erro: não foi possível abrir o arquivo " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string source = buffer.str();

    // Parser simples: remove espaços e verifica se o token é "hello"
    std::string token;
    for (char c : source) {
        if (!isspace(c))
            token.push_back(c);
    }
    if (token != "hello") {
        std::cerr << "Erro de sintaxe: esperado 'hello'" << std::endl;
        return 1;
    }

    // Cria o contexto LLVM e o módulo para o programa gerado
    LLVMContext context;
    auto module = std::make_unique<Module>("HelloLangModule", context);
    IRBuilder<> builder(context);

    // Define a função main: int main()
    FunctionType *mainType = FunctionType::get(builder.getInt32Ty(), false);
    Function *mainFunc = Function::Create(mainType, Function::ExternalLinkage, "main", module.get());
    BasicBlock *entry = BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);

    // Cria uma string global "Hello, World!"
    Value *helloWorldStr = builder.CreateGlobalStringPtr("Hello, World!");

    // Declara a função puts: int puts(const char *)
    std::vector<Type*> putsArgs { builder.getInt8Ty()->getPointerTo() };
    FunctionType *putsType = FunctionType::get(builder.getInt32Ty(), putsArgs, false);
    FunctionCallee putsFunc = module->getOrInsertFunction("puts", putsType);

    // Chama puts("Hello, World!")
    builder.CreateCall(putsFunc, helloWorldStr);

    // Retorna 0
    builder.CreateRet(ConstantInt::get(builder.getInt32Ty(), 0));

    // Verifica o módulo para erros
    std::string errStr;
    raw_string_ostream errStream(errStr);
    if (verifyModule(*module, &errStream)) {
        std::cerr << "Erro na verificação do módulo: " << errStream.str() << std::endl;
        return 1;
    }

    // Imprime o LLVM IR na saída padrão (pode ser redirecionado para um arquivo)
    module->print(outs(), nullptr);

    return 0;
}
