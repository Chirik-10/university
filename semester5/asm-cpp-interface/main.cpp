#include <iostream>
using namespace std;

extern "C" void __stdcall string_compression(char* input_s, char* result_s);

int main() {
    const int max_lenght = 200;
    char input_string[max_lenght];
    char compressed_string[max_lenght];

    cout << "Введите строку для сжатия: ";
    cin.getline(input_string, max_lenght);
    if (strlen(input_string) == 0) {
        cout << "Строка пустая." << endl;
        return 0;
    }
    string_compression(input_string, compressed_string);
    cout << "Сжатая строка: " << compressed_string << endl;
    return 0;
}
