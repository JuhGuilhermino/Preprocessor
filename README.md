# Pré-processador em C++

## Compilação

```bash
mkdir build && cd build
cmake ..
make
```

## Execução
```bash
./preprocessor nome_do_arquivo
```

## To-do
- [x] **Adaptar Lexer e Parser a nova gramática**.
- [ ] Embutir o pré-processador no Lexer. 
- [ ] Ao encontrar erro léxico, informar: erro, localização (nº da linha) e toke.
- [x] **Criar a tabela de símbolos**.
- [x] **Modificar o Parser para povoar a tabela de símbolos**.
- [x] **Flag para o Parser retornar a ATS e tabela de símbolos**.
- [x] Flag para printar a lista de tokens.
- [ ] Flag dos erros léxicos.
- [ ] Flag de sugestões de correção léxica e sintática.
- [ ] **Implementar o analisador semântico**
  

