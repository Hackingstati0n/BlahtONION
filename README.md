# Directory Scanner

![image](https://github.com/user-attachments/assets/df6997aa-f3af-4c46-a3e3-54ca12fb8a16)

## Descrição
Este é um scanner de diretórios simples escrito em C utilizando `libcurl`. O programa baixa uma wordlist de um repositório em tempos de execução e nao armazenara nenhum arquivo pesado desnecessario dentro do HD e sim apenas lerá de um repositorio então, testa diretórios em um site especificado pelo usuário, exibindo apenas aqueles que retornam o código de status HTTP `200`.

## Recursos
- Usa `libcurl` para realizar requisições HTTP.
- Faz download de uma wordlist automaticamente.
- Filtra e exibe apenas diretórios com resposta `200 OK`.
- Implementa métodos eficientes para manipulação de strings e memória.

## Dependências
Antes de compilar e rodar o programa, certifique-se de que a `libcurl` esteja instalada:

**Debian/Ubuntu**:
```bash
sudo apt update && sudo apt install libcurl4-openssl-dev
```

**Fedora**:
```bash
sudo dnf install libcurl-devel
```

**Arch Linux**:
```bash
sudo pacman -S curl
```

## Compilação e Execução
Para compilar o programa, utilize `gcc` com a flag `-lcurl`:

```bash
gcc scanner.c -o scanner -lcurl
```

Para executar:
```bash
./scanner <URL>
```

Exemplo:
```bash
./scanner https://exemplo.com
```

## Funcionamento
1. O programa le uma wordlist da Web.
2. Testa cada diretório listado na URL fornecida.
3. Exibe apenas aqueles com resposta HTTP `200 OK`.
4. Remove espaços em branco e caracteres de nova linha.
5. Finaliza a varredura informando a conclusão.

## Dicas:
Para alterar a URL das Wordlists vá ate a linha: #121

## Autor
Criado por **David A. Mascaro**.

## Licença
Este projeto é distribuído sob a licença MIT.
