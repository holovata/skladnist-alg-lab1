#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

typedef unsigned long long ull;
const int MAX_STRINGS = 1000000; // Максимальна кількість рядків у множині
const int MAX_OPERATIONS = 1000000; // Максимальна кількість операцій

// Структура для обчислення поліноміального хеша
struct PolynomialHash {
    static const int MOD = (int)1e9 + 9; // просте число для модуля поліноміального хешування
    static const int BASE = 111111; // BASE - основа хешування

    static vector<int> power_mod_base; // степені бази по модулю MOD
    vector<int> prefix_mod_hash; // Хеші префіксів по модулю MOD

    PolynomialHash(const string& input_string) {
        prefix_mod_hash.resize(input_string.size() + 1, 0);
        int string_size = input_string.size();

        // Обчислюємо степені бази, якщо їх не вистачає для роботи з рядком input_string
        while ((int)power_mod_base.size() <= string_size) {
            power_mod_base.push_back(1LL * power_mod_base.back() * BASE % MOD);
        }

        // Обчислюємо хеші для префіксів рядка
        for (int i = 0; i < string_size; ++i) {
            prefix_mod_hash[i + 1] = (prefix_mod_hash[i] + 1LL * input_string[i] * power_mod_base[i]) % MOD;
        }
    }

    // Метод для отримання поліноміального хеша підрядка [start, start + length)
    int get_substring_hash(int start, int length) const {
        int hash_mod = prefix_mod_hash[start + length] - prefix_mod_hash[start];
        if (hash_mod < 0) hash_mod += MOD;
        return hash_mod;
    }
};

vector<int> PolynomialHash::power_mod_base{ 1 };

// Генерацфя випадкового рядка
string generate_random_string() {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<int> length_distribution(1, 15);
    uniform_int_distribution<int> char_distribution(0, 25);

    int length = length_distribution(generator);
    string random_string;
    for (int i = 0; i < length; ++i) {
        random_string += charset[char_distribution(generator)];
    }
    return random_string;
}

// Функція пошуку палиндромів
ull find_palindromes(const unordered_set<string>& string_set) {
    ull total_palindromes = 0; // Лічильник загальної кількості паліндромів
    unordered_set<string> palindrome_set; // Множина для збереження унікальних паліндромних підрядків

    // Обробляємо кожен рядок у множині для пошуку паліндромів
    for (const string& current_string : string_set) {
        if (current_string.length() <= 1) continue; // Пропускаємо рядки довж <= 1

        string reversed_string = current_string;
        reverse(reversed_string.begin(), reversed_string.end()); // Перевертаємо рядок

        // Обчислюємо поліноміальні хеші для оригінального рядка і перевернутого
        PolynomialHash hash_original(current_string);
        PolynomialHash hash_reversed(reversed_string);

        int string_length = current_string.size();  // Отримуємо довжину рядка

        // Шукаємо паліндроми з непарною довжиною (центр у символі 'center')
        for (int center = 0; center < string_length; center++) {
            // Пошук паліндромів, довжина > 1
            for (int len = 1; len <= center && len + center < string_length; len++) {
                int left = center - len; // Лівий кінець паліндрома
                int right = center + len; // Правий кінець паліндрома

                // Порівнюємо хеші підрядків для перевірки на паліндромність
                int original_hash = hash_original.get_substring_hash(left, right - left + 1);
                int reversed_hash = hash_reversed.get_substring_hash(string_length - right - 1, right - left + 1);

                if (original_hash == reversed_hash) {
                    total_palindromes++;
                    palindrome_set.insert(current_string.substr(left, right - left + 1)); // Додаємо унікальний паліндром
                }
                else {
                    break;
                }
            }
        }

        // Шукаємо паліндроми з парною довжиною (центр між символами 'center' і 'center + 1')
        for (int center = 0; center < string_length - 1; center++) {
            for (int len = 0; len <= center && len + center + 1 < string_length; len++) {
                int left = center - len; // Лівий кінець
                int right = center + len + 1; // Правий кінець (на один символ правіше)

                // Порівнюємо хеші для перевірки на паліндромність
                int original_hash = hash_original.get_substring_hash(left, right - left + 1);
                int reversed_hash = hash_reversed.get_substring_hash(string_length - right - 1, right - left + 1);

                if (original_hash == reversed_hash) {
                    total_palindromes++;
                    palindrome_set.insert(current_string.substr(left, right - left + 1));
                }
                else {
                    break;
                }
            }
        }
    }

    return total_palindromes;
}

// Автоматична генерация рядків
void generate_strings_and_measure_time(int num_strings) {
    unordered_set<string> string_set;

    auto start_operations_time = high_resolution_clock::now(); 

    for (int i = 0; i < num_strings; ++i) {
        string random_string = generate_random_string();
        string_set.insert(random_string); // Додаємо рядок в множину
    }

    auto end_operations_time = high_resolution_clock::now(); 
    auto duration_operations = duration_cast<milliseconds>(end_operations_time - start_operations_time).count();
    cout << "Generated " << num_strings << " strings in: " << duration_operations << " ms\n";

    auto start_palindrome_time = high_resolution_clock::now(); 

    ull total_palindromes = find_palindromes(string_set);

    auto end_palindrome_time = high_resolution_clock::now();
    auto duration_palindrome = duration_cast<milliseconds>(end_palindrome_time - start_palindrome_time).count();
    cout << "Palindrome search time for " << num_strings << " strings: " << duration_palindrome << " ms\n";
    cout << "Total palindromes found: " << total_palindromes << "\n";
}

int main() {
    cout << "Choose mode:\n";
    cout << "1. Manual input\n";
    cout << "2. Automatic generation of strings\n";
    int choice;
    cin >> choice;
    cin.ignore(); // Игнорируем символ новой строки после ввода числа
    if (choice == 1) {
        // Ручний ввід
        unordered_set<string> string_set; // Множина для збереження рядків
        string line_input;
        int operation_count = 0; // Лічильник операцій

        auto start_operations_time = high_resolution_clock::now();

        // Введення даних від користувача у форматі "+ рядок", "- рядок" або "? рядок"
        while (getline(cin, line_input)) {
            if (line_input.empty()) continue;
            if (line_input == "#") break; // кінець введення

            // Перевіряємо кількість операцій
            if (++operation_count > MAX_OPERATIONS) {
                cerr << "Error: Too many operations\n";
                return 1; // Завершуємо програму, якщо кількість операцій перевищує ліміт
            }

            char operation = line_input[0]; // тип операції (+, -, ?)
            if (operation != '+' && operation != '-' && operation != '?') {
                cerr << "Unknown operation: " << operation << "\n";
                continue;
            }

            // Виділяємо рядок після операції та пробілу
            string current_string = line_input.substr(2);

            // Перевіряємо рядок (непорожній, не більше 15 символів)
            if (current_string.empty() || current_string.length() > 15) {
                cerr << "String is empty or longer than 15 symbols: " << current_string << "\n";
                continue;
            }

            // Перевірка на вміст рядка
            bool valid_string = true;
            for (char c : current_string) {
                if (!isalpha(c) || !islower(c)) {
                    cerr << "Error: String contains non-lowercase Latin letters: " << current_string << "\n";
                    valid_string = false;
                    break;
                }
            }
            if (!valid_string) continue; 

            // Обробка операцій +, -, ?
            if (operation == '+') {
                if (string_set.size() >= MAX_STRINGS) {
                    cerr << "Error: Too many strings in the set\n";
                    return 1; // Завершуємо програму, якщо кількість рядків перевищує ліміт
                }
                string_set.insert(current_string);
            }
            else if (operation == '-') {
                string_set.erase(current_string);
            }
            else if (operation == '?') {
                if (string_set.find(current_string) != string_set.end()) {
                    cout << "yes\n";
                }
                else {
                    cout << "no\n";
                }
            }
        }

        auto end_operations_time = high_resolution_clock::now();
        auto duration_operations = duration_cast<milliseconds>(end_operations_time - start_operations_time).count();
        cout << "Operations time: " << duration_operations << " ms\n";

        auto start_palindrome_time = high_resolution_clock::now();

        ull total_palindromes = find_palindromes(string_set);

        auto end_palindrome_time = high_resolution_clock::now();
        auto duration_palindrome = duration_cast<milliseconds>(end_palindrome_time - start_palindrome_time).count();
        cout << "Palindrome search time: " << duration_palindrome << " ms\n";
        cout << "Total palindromes count: " << total_palindromes << "\n";
    }

    else if (choice == 2) {
        // Автоматична генерація
        cout << "Choose number of strings to generate:\n";
        cout << "1. 1,000,000 strings\n";
        cout << "2. 500,000 strings\n";
        cout << "3. 100,000 strings\n";
        int num_choice;
        cin >> num_choice;

        int num_strings = 0;
        if (num_choice == 1) {
            num_strings = 1000000;
        }
        else if (num_choice == 2) {
            num_strings = 500000;
        }
        else if (num_choice == 3) {
            num_strings = 100000;
        }

        generate_strings_and_measure_time(num_strings);
    }

    return 0;
}
