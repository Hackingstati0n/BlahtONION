#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>

// Códigos de escape ANSI para cores
#define COLOR_GREEN_LIME "\x1b[38;5;10m"
#define COLOR_RESET "\x1b[0m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RED "\x1b[31m"

// Estrutura para armazenar o conteúdo baixado
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Função de callback para escrever dados em memória
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        printf("Erro: Não foi possível alocar memória.\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Função para baixar o conteúdo de uma URL
char *download_url(const char *url) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  // Inicializa a memória
    chunk.size = 0;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Erro ao inicializar CURL.\n");
        free(chunk.memory);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Erro ao baixar URL: %s\n", curl_easy_strerror(res));
        free(chunk.memory);
        chunk.memory = NULL;
    }

    curl_easy_cleanup(curl);
    return chunk.memory;
}

// Função para remover espaços em branco e caracteres de nova linha
char *trim(char *str) {
    if (!str) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

// Função para obter o código de status de uma URL
int get_status_code(const char *url) {
    CURL *curl;
    long response_code = -1;
    curl = curl_easy_init();
    if (!curl) return -1;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Requisição HEAD
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (curl_easy_perform(curl) == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl);
    return (int)response_code;
}

// Função para exibir a ASCII art
void print_ascii_art() {
    printf(COLOR_GREEN_LIME);
    printf("  ____  _       _     _      ____  _   _ _____ ____  \n");
    printf(" |  _ \\| |     | |   | |    / __ \\| \\ | |_   _/ __ \\ \n");
    printf(" | |_) | | __ _| |__ | |_  | |  | |  \\| | | || |  | |\n");
    printf(" |  _ <| |/ _` | '_ \\| __| | |  | | . ` | | || |  | |\n");
    printf(" | |_) | | (_| | | | | |_  | |__| | |\\  |_| || |__| |\n");
    printf(" |____/|_|\\__,_|_| |_|\\__|  \\____/|_| \\_|_____\\____/ \n");
    printf(COLOR_RESET);
    printf("\nCriado por David A. Mascaro\n\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <URL>\n", argv[0]);
        return 1;
    }

    // Exibe a ASCII art
    print_ascii_art();

    // URL da lista de diretórios
    const char *wordlist_url = "https://raw.githubusercontent.com/digination/dirbuster-ng/refs/heads/master/wordlists/common.txt";

    // Baixa o conteúdo da lista de diretórios
    char *wordlist_content = download_url(wordlist_url);
    if (!wordlist_content) {
        fprintf(stderr, "Erro ao baixar a lista de diretórios.\n");
        return 1;
    }

    // URL base fornecida pelo usuário
    char *url_base = argv[1];
    size_t len = strlen(url_base);
    if (len > 0 && url_base[len - 1] == '/') {
        url_base[len - 1] = '\0'; // Remove a barra final, se houver
    }

    // Cabeçalho da saída
    printf("[+] Iniciando varredura em: %s%s%s\n", COLOR_CYAN, url_base, COLOR_RESET);
    printf("[+] Método: HEAD\n");
    printf("[+] Mostrando apenas status 200:\n\n");

    // Processa a lista de diretórios
    char *line = strtok(wordlist_content, "\n");
    while (line != NULL) {
        char *trimmed_line = trim(line);
        if (strlen(trimmed_line) == 0 || trimmed_line[0] == '#') {
            line = strtok(NULL, "\n");
            continue; // Ignora linhas vazias ou comentários
        }

        // Codifica o diretório para URL
        CURL *curl = curl_easy_init();
        if (!curl) {
            fprintf(stderr, "Erro ao inicializar CURL.\n");
            free(wordlist_content);
            return 1;
        }

        char *encoded = curl_easy_escape(curl, trimmed_line, 0);
        if (!encoded) {
            fprintf(stderr, "Erro ao codificar: %s\n", trimmed_line);
            line = strtok(NULL, "\n");
            continue;
        }

        // Constrói a URL completa
        char target_url[4096];
        snprintf(target_url, sizeof(target_url), "%s/%s", url_base, encoded);

        // Verifica o status da URL
        int status = get_status_code(target_url);
        if (status == 200) {
            printf("[+] %s%s%s\n", COLOR_GREEN_LIME, target_url, COLOR_RESET); // Exibe apenas URLs com status 200
        }

        curl_free(encoded);
        curl_easy_cleanup(curl);

        line = strtok(NULL, "\n");
    }

    free(wordlist_content);
    printf("\n[+] Varredura concluída.\n");
    return 0;
}
