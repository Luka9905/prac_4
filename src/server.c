#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    int number_to_guess = rand() % 100 + 1;
    int guess;
    int attempts = 0;

    printf("Подключен клиент: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            break; // Клиент отключился
        }

        guess = atoi(buffer);
        attempts++;

        if (guess < number_to_guess) {
            snprintf(buffer, BUFFER_SIZE, "Слишком маленькое число.");
        } else if (guess > number_to_guess) {
            snprintf(buffer, BUFFER_SIZE, "Слишком большое число.");
        } else {
            snprintf(buffer, BUFFER_SIZE, "Поздравляем! Вы угадали число %d за %d попыток.", number_to_guess, attempts);
            send(client_socket, buffer, strlen(buffer), 0);
            break;
        }

        send(client_socket, buffer, strlen(buffer), 0);
    }

    printf("Отключен клиент: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    srand(time(NULL));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Ошибка прослушивания");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен и слушает на порту %d\n", port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Ошибка подключения клиента");
            continue;
        }
        
        handle_client(client_socket, client_addr);
    }

    close(server_socket);
    return 0;
}
