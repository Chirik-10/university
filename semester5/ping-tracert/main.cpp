#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

// Определяем структуры для ICMP
typedef struct {
    UCHAR   Ttl;
    UCHAR   Tos;
    UCHAR   Flags;
    UCHAR   OptionsSize;
    PUCHAR  OptionsData;
} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;

typedef struct {
    DWORD Address;
    DWORD Status;
    DWORD RoundTripTime;
    USHORT DataSize;
    USHORT Reserved;
    PVOID Data;
    IP_OPTION_INFORMATION Options;
} ICMP_ECHO_REPLY, * PICMP_ECHO_REPLY;

// Объявляем функции ICMP
extern "C" {
    HANDLE WINAPI IcmpCreateFile(VOID);
    BOOL WINAPI IcmpCloseHandle(HANDLE IcmpHandle);
    DWORD WINAPI IcmpSendEcho(
        HANDLE IcmpHandle,
        DWORD DestinationAddress,
        LPVOID RequestData,
        WORD RequestSize,
        PIP_OPTION_INFORMATION RequestOptions,
        LPVOID ReplyBuffer,
        DWORD ReplySize,
        DWORD Timeout
    );
}

// Константы статусов
#define IP_SUCCESS 0
#define IP_TTL_EXPIRED_TRANSIT 11013
#define IP_REQ_TIMED_OUT 11010

class Traceroute {
private:
    WSADATA wsaData;
    HANDLE hIcmpFile;

public:
    Traceroute() : hIcmpFile(INVALID_HANDLE_VALUE) {
        // Инициализация Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }

        // Создание handle для ICMP
        hIcmpFile = IcmpCreateFile();
        if (hIcmpFile == INVALID_HANDLE_VALUE) {
            WSACleanup();
            throw std::runtime_error("Unable to open ICMP handle");
        }
    }

    ~Traceroute() {
        if (hIcmpFile != INVALID_HANDLE_VALUE) {
            IcmpCloseHandle(hIcmpFile);
        }
        WSACleanup();
    }

    // Функция для преобразования доменного имени в IP-адрес
    std::string ResolveHostname(const std::string& hostname) {
        struct addrinfo hints, * result;
        char ipstr[INET6_ADDRSTRLEN];

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // Используем только IPv4
        hints.ai_socktype = SOCK_STREAM;

        int status = getaddrinfo(hostname.c_str(), NULL, &hints, &result);
        if (status != 0) {
            throw std::runtime_error("Cannot resolve hostname: " + hostname);
        }

        void* addr;
        if (result->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)result->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        else {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)result->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(result->ai_family, addr, ipstr, sizeof(ipstr));
        freeaddrinfo(result);

        return std::string(ipstr);
    }

    // Преобразование строки IP в DWORD (совместимость с inet_pton)
    DWORD IpStringToDword(const std::string& ipAddress) {
        struct in_addr addr;
        if (inet_pton(AF_INET, ipAddress.c_str(), &addr) == 1) {
            return addr.S_un.S_addr;
        }
        return INADDR_NONE;
    }

    // Основная функция трассировки
    void Trace(const std::string& target, int maxHops = 30) {
        std::string ipAddress = ResolveHostname(target);
        std::cout << "Трассировка маршрута к " << target << " [" << ipAddress << "]\n";
        std::cout << "с максимальным числом прыжков " << maxHops << ":\n\n";

        // Преобразование IP-адреса с использованием inet_pton
        DWORD destAddr = IpStringToDword(ipAddress);
        if (destAddr == INADDR_NONE) {
            std::cout << "Ошибка: неверный IP-адрес\n";
            return;
        }

        // Буфер для отправки и получения данных
        char sendData[32] = "Data for ping packet";
        char replyBuffer[sizeof(ICMP_ECHO_REPLY) + sizeof(sendData)];

        bool destinationReached = false;

        for (int ttl = 1; ttl <= maxHops && !destinationReached; ttl++) {
            std::cout << std::setw(2) << ttl << "  ";

            // Инициализация опций
            IP_OPTION_INFORMATION options;
            ZeroMemory(&options, sizeof(options));
            options.Ttl = (UCHAR)ttl;

            // Отправка 3 пакетов для каждого TTL
            std::vector<ULONGLONG> times;
            std::vector<std::string> responses;

            for (int attempt = 0; attempt < 3; attempt++) {
                ULONGLONG startTime = GetTickCount64();

                // Отправка ICMP эхо-запроса
                DWORD result = IcmpSendEcho(
                    hIcmpFile,
                    destAddr,
                    sendData, sizeof(sendData),
                    &options,
                    replyBuffer, sizeof(replyBuffer),
                    2000  // Таймаут 2 секунды
                );

                ULONGLONG endTime = GetTickCount64();
                ULONGLONG responseTime = endTime - startTime;

                if (result != 0) {
                    PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replyBuffer;

                    if (pEchoReply->Status == IP_SUCCESS || pEchoReply->Status == IP_TTL_EXPIRED_TRANSIT) {
                        struct in_addr replyAddr;
                        replyAddr.S_un.S_addr = pEchoReply->Address;
                        char ipStr[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &replyAddr, ipStr, sizeof(ipStr));

                        times.push_back(responseTime);
                        responses.push_back(ipStr);

                        // Проверяем, достигли ли конечного узла
                        if (pEchoReply->Address == destAddr) {
                            destinationReached = true;
                        }
                    }
                    else {
                        times.push_back(0);
                        responses.push_back("*");
                    }
                }
                else {
                    times.push_back(0);
                    responses.push_back("*");
                }
            }

            // Вывод результатов для текущего TTL
            bool allTimeouts = true;
            for (size_t i = 0; i < times.size(); i++) {
                if (responses[i] != "*") {
                    allTimeouts = false;
                    if (times[i] > 0) {
                        std::cout << std::setw(4) << times[i] << " ms  ";
                    }
                    else {
                        std::cout << "    <1 ms  ";
                    }
                }
                else {
                    std::cout << "    *      ";
                }
            }

            // Вывод IP-адреса или сообщения
            if (!allTimeouts && !responses.empty() && responses[0] != "*") {
                std::cout << " " << responses[0];

                // Попытка получить имя хоста
                struct sockaddr_in sa;
                sa.sin_family = AF_INET;
                inet_pton(AF_INET, responses[0].c_str(), &sa.sin_addr);

                char hostname[NI_MAXHOST];
                if (getnameinfo((struct sockaddr*)&sa, sizeof(sa),
                    hostname, NI_MAXHOST, NULL, 0, 0) == 0 && hostname[0] != '\0') {
                    std::cout << "  [" << hostname << "]";
                }
            }
            else {
                std::cout << " Превышен интервал ожидания для запроса";
            }

            std::cout << std::endl;

            // Небольшая задержка между прыжками
            Sleep(500);
        }

        if (!destinationReached) {
            std::cout << "\nТрассировка завершена.\n";
        }
        else {
            std::cout << "\nТрассировка успешно завершена.\n";
        }
    }
};

int main() {
    try {
        Traceroute tracer;
        std::string target;
        std::cout << "Введите доменное имя или IP-адрес для трассировки: ";
        std::getline(std::cin, target);
        tracer.Trace(target);
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
