#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

using namespace std;

atomic<bool> receiving{ true };

// Функция для получения сообщений от сервера
void receiveMessages(SOCKET clientSocket) {
    char buffer[1024];

    while (receiving) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            cout << "\r" << buffer << endl; // \r для очистки строки
            cout << "> " << flush;
        }
        else if (bytesReceived == 0) {
            cout << "\rСоединение с сервером разорвано" << endl;
            break;
        }
        else {
            if (receiving) {
                cout << "\rОшибка при получении данных" << endl;
            }
            break;
        }
    }
}

int main() {
    printf("CHAT-КЛИЕНТ\n");
    printf("Подключение к серверу на порту 2002\n\n");

    // Инициализация Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Ошибка инициализации Winsock: %d\n", result);
        return 1;
    }

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Ошибка создания сокета: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Ввод IP-адреса сервера
    string serverIP;
    cout << "Введите IP-адрес сервера: ";
    getline(cin, serverIP);
    if (serverIP.empty()) {
        serverIP = "127.0.0.1";
        printf("Используется адрес по умолчанию: 127.0.0.1\n");
    }

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2002);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    // Подключение к серверу
    printf("Подключение к серверу %s:2002...\n", serverIP.c_str());
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Ошибка подключения: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    printf("Подключение установлено!\n\n");

    // Запускаем поток для получения сообщений
    thread receiverThread(receiveMessages, clientSocket);

    // Основной цикл отправки сообщений
    string message;
    while (true) {
        cout << "> ";
        getline(cin, message);

        if (message == "/quit") {
            send(clientSocket, message.c_str(), (int)message.length(), 0);
            break;
        }

        if (!message.empty()) {
            send(clientSocket, message.c_str(), (int)message.length(), 0);
        }
    }

    // Завершение работы
    receiving = false;
    closesocket(clientSocket);

    if (receiverThread.joinable()) {
        receiverThread.join();
    }

    WSACleanup();
    printf("Клиент завершил работу\n");

    system("pause");
    return 0;
}
