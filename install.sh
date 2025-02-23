#!/bin/bash
# build.sh - Script para compilar o compilador HelloLang e gerar o binário final "hello"

# Se o argumento for "clean", remove os arquivos gerados e encerra.
if [ "$1" == "clean" ]; then
    echo "Limpando arquivos gerados..."
    rm -f hello_compiler hello.ll hello.s hello
    exit 0
fi

# Define o diretório do LLVM instalado via Homebrew
LLVM_DIR="/usr/local/opt/llvm"
if [ ! -d "$LLVM_DIR" ]; then
    LLVM_DIR="/opt/homebrew/opt/llvm"
fi
echo "Usando LLVM em: $LLVM_DIR"

# Compila main.cpp para gerar o compilador hello_compiler
echo "Compilando main.cpp para gerar hello_compiler..."
clang++ -std=c++17 -I"$LLVM_DIR/include" -L"$LLVM_DIR/lib" -Wl,-rpath,"$LLVM_DIR/lib" main.cpp -lLLVM -o hello_compiler
if [ $? -ne 0 ]; then
    echo "Erro na compilação de main.cpp"
    exit 1
fi

# Cria um arquivo fonte para a linguagem HelloLang com o conteúdo "hello"
# (Se já existir, ele será sobrescrito)
SOURCE_FILE="program.hl"
echo "hello" > "$SOURCE_FILE"

# Executa o compilador para gerar o LLVM IR (hello.ll) a partir do arquivo fonte
echo "Executando hello_compiler para gerar hello.ll..."
./hello_compiler "$SOURCE_FILE" > hello.ll
if [ $? -ne 0 ]; then
    echo "Erro ao executar hello_compiler"
    exit 1
fi

# Converte o LLVM IR para assembly usando llc (gera hello.s)
echo "Convertendo hello.ll para assembly (hello.s)..."
llc hello.ll -o hello.s
if [ $? -ne 0 ]; then
    echo "Erro ao converter hello.ll para hello.s"
    exit 1
fi

# Compila o assembly para gerar o executável final "hello" usando clang
echo "Compilando hello.s com clang para gerar o executável hello..."
clang hello.s -o hello
if [ $? -ne 0 ]; then
    echo "Erro ao compilar hello.s"
    exit 1
fi

echo "Construção concluída com sucesso. Executável 'hello' gerado."
