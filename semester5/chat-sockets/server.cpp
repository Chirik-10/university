#pragma comment(lib, "Ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cctype>
#include <map>

using namespace std;

// Структура для хранения информации о клиенте
struct ClientInfo {
    SOCKET socket;
    string ip;
    int port;
    string name;
};

vector<ClientInfo> clients;
mutex clientsMutex;
atomic<bool> serverRunning{ true };

// Функция обработки текста
string obrabotka(const string& vhodtxt) {
    if (vhodtxt.empty()) {
        return "Получен пустой текст[0]";
    }

    string res;
    string sentence;
    bool in_sentence = false;

    for (size_t i = 0; i < vhodtxt.length(); i++) {
        char symbol = vhodtxt[i];

        // если символ не пробельный
        if (symbol != ' ' && symbol != '\t' && symbol != '\n' && symbol != '\r') {
            sentence += symbol;
            in_sentence = true;
        }
        else {
            // если пробельный символ, но мы в предложении - добавляем
            if (in_sentence) {
                sentence += symbol;
            }
        }

        // если это конец предложения
        if (symbol == '.' || symbol == '!' || symbol == '?') {
            if (!sentence.empty()) {
                // количество символов (без пробелов)
                int cnt_symbols = 0;
                for (char sym_in_sentence : sentence) {
                    if (!isspace(static_cast<unsigned char>(sym_in_sentence))) {
                        cnt_symbols++;
                    }
                }

                // формируем результат: предложение + [количество символов]
                res += sentence + "[" + to_string(cnt_symbols) + "] ";
                sentence.clear();
                in_sentence = false;
            }
        }
    }

    // обрабатываем оставшийся текст, если предложение не закончилось точкой
    if (!sentence.empty()) {
        int cnt_symbols = 0;
        for (char sym_in_sentence : sentence) {
            if (!isspace(static_cast<unsigned char>(sym_in_sentence))) {
                cnt_symbols++;
            }
        }
        if (!res.empty() && res.back() == ' ') {
            res.pop_back();
        }
        res += sentence + "[" + to_string(cnt_symbols) + "]";
    }

    // удаляем лишний пробел в конце, если он есть
    if (!res.empty() && res.back() == ' ') {
        res.pop_back();
    }

    // возврат результата или исходного текста с [0], если ничего не обработано
    return res.empty() ? vhodtxt + "[0]" : res;
}

// Функция для отправки сообщения всем клиентам
void broadcastMessage(const string& message, SOCKET excludeSocket = INVALID_SOCKET) {
    lock_guard<mutex> lock(clientsMutex);

    for (const auto& client : clients) {
        if (client.socket != excludeSocket) {
            send(client.socket, message.c_str(), (int)message.length(), 0);
        }
    }
}

// Функция для получения текущего времени
string getCurrentTime() {
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &localTime);
    return string(timeStr);
}

// Функция обработки клиента
void handleClient(SOCKET clientSocket, string clientIP, int clientPort) {
    char buffer[1024];
    string clientName = "User_" + to_string(clientPort);

    // Добавляем клиента в список
    {
        lock_guard<mutex> lock(clientsMutex);
        clients.push_back({ clientSocket, clientIP, clientPort, clientName });
    }

    // Уведомляем всех о новом клиенте
    string joinMessage = "[" + getCurrentTime() + "] " + clientName + " (" + clientIP + ":" + to_string(clientPort) + ") присоединился к чату";
    cout << joinMessage << endl;
    broadcastMessage(joinMessage, clientSocket);

    // Отправляем приветственное сообщение новому клиенту
    string welcomeMsg = "Добро пожаловать в чат! Ваше имя: " + clientName +
        "\nКоманды: /help, /users, /name новое_имя, /quit";
    send(clientSocket, welcomeMsg.c_str(), (int)welcomeMsg.length(), 0);

    while (serverRunning) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            break;
        }

        buffer[bytesReceived] = '\0';
        string message(buffer);

        // Обработка команд
        if (message == "/quit") {
            break;
        }
        else if (message == "/help") {
            string helpMsg = "Доступные команды:\n"
                "/quit - выйти из чата\n"
                "/help - справка\n"
                "/users - список пользователей\n"
                "/name новое_имя - сменить имя\n";
            send(clientSocket, helpMsg.c_str(), (int)helpMsg.length(), 0);
        }
        else if (message == "/users") {
            lock_guard<mutex> lock(clientsMutex);
            string usersMsg = "Подключенные пользователи (" + to_string(clients.size()) + "):\n";
            for (const auto& client : clients) {
                usersMsg += "- " + client.name + " (" + client.ip + ":" + to_string(client.port) + ")\n";
            }
            send(clientSocket, usersMsg.c_str(), (int)usersMsg.length(), 0);
        }
        else if (message.length() >= 6 && message.substr(0, 6) == "/name ") {
            string newName = message.substr(6);
            string oldName = clientName;

            // Обновляем имя клиента
            {
                lock_guard<mutex> lock(clientsMutex);
                for (auto& client : clients) {
                    if (client.socket == clientSocket) {
                        client.name = newName;
                        break;
                    }
                }
            }

            clientName = newName;
            string nameChangeMsg = "[" + getCurrentTime() + "] " + oldName + " сменил имя на " + newName;
            cout << nameChangeMsg << endl;
            broadcastMessage(nameChangeMsg);

            string confirmMsg = "Ваше имя изменено на: " + newName;
            send(clientSocket, confirmMsg.c_str(), (int)confirmMsg.length(), 0);
        }
        else {
            string processedMessage = obrabotka(message);
            string formattedMsg = "[" + getCurrentTime() + "] " + clientName + ": " + processedMessage;

            // Дублируем на экране сервера
            cout << formattedMsg << endl;

            // Рассылаем всем клиентам
            broadcastMessage(formattedMsg);
        }
    }

    // Удаляем клиента из списка
    {
        lock_guard<mutex> lock(clientsMutex);
        auto it = remove_if(clients.begin(), clients.end(),
            [clientSocket](const ClientInfo& client) { return client.socket == clientSocket; });
        clients.erase(it, clients.end());
    }

    // Уведомляем всех о выходе клиента
    string leaveMessage = "[" + getCurrentTime() + "] " + clientName + " покинул чат";
    cout << leaveMessage << endl;
    broadcastMessage(leaveMessage);

    closesocket(clientSocket);
}

int main() {
    printf("CHAT-СЕРВЕР\n");
    printf("Порт: 2002\n\n");

    // Инициализация Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Ошибка инициализации Winsock: %d\n", result);
        return 1;
    }
    printf("Winsock инициализирован успешно\n");

    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Ошибка создания сокета: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Сокет создан успешно\n");

    // Разрешаем переиспользование порта
    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(2002);

    // Привязка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Ошибка привязки сокета: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    printf("Сокет привязан к порту 2002\n");

    // Прослушивание порта
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Ошибка прослушивания: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    printf("Сервер запущен и ожидает подключений...\n");

    // Основной цикл сервера
    vector<thread> clientThreads;

    while (serverRunning) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            if (serverRunning) {
                printf("Ошибка принятия соединения: %d\n", WSAGetLastError());
            }
            continue;
        }

        // Получаем информацию о клиенте
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        printf("Новое подключение: %s:%d\n", clientIP, clientPort);

        // Запускаем поток для обработки клиента
        clientThreads.emplace_back(handleClient, clientSocket, string(clientIP), clientPort);
    }

    // Ожидаем завершения всех потоков
    for (auto& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Очистка ресурсов
    closesocket(serverSocket);
    WSACleanup();
    printf("Сервер завершил работу\n");

    system("pause");
    return 0;
}
