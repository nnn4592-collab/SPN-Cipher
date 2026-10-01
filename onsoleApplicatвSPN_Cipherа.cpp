#include <iostream>
#include <string>
#include <bitset>

using namespace std;

// Количество раундов шифрования
const int ROUNDS = 8;

// Основной ключ
const bitset<8> KEY("10110101");

// Таблица S-Box для замены 4-битных значений
const int SBOX[16] = {
    0xE, 0x4, 0xD, 0x1,
    0x2, 0xF, 0xB, 0x8,
    0x3, 0xA, 0x6, 0xC,
    0x5, 0x9, 0x0, 0x7
};

// Обратная таблица S-Box для расшифрования
const int INV_SBOX[16] = {
    0xE, 0x3, 0x4, 0x8,
    0x1, 0xC, 0xA, 0xF,
    0x7, 0xD, 0x9, 0x6,
    0xB, 0x2, 0x0, 0x5
};

// Выполняем операцию XOR
bitset<8> XORBits(bitset<8> a, bitset<8> b)
{
    return a ^ b; // Сравниваем каждый бит двух значений
}

// Получаем ключ для текущего раунда
bitset<8> GetRoundKey(int round)
{
    bitset<8> key = KEY;

    // Переводим ключ из bitset в число
    unsigned char value = static_cast<unsigned char>(key.to_ulong());

    // Определяем количество сдвигов
    int shift = round % 8;

    // Циклически сдвигаем биты ключа
    value = static_cast<unsigned char>(
        (value << shift) | (value >> (8 - shift))
        );

    // Возвращаем полученный ключ
    return bitset<8>(value);
}

// Применяем S-Box
bitset<8> ApplySBox(bitset<8> input)
{
    // Переводим 8 бит в число
    int value = static_cast<int>(input.to_ulong());

    // Берём первые 4 бита
    int high = (value >> 4) & 0x0F;

    // Берём последние 4 бита
    int low = value & 0x0F;

    // Заменяем первую половину по таблице S-Box
    high = SBOX[high];

    // Заменяем вторую половину по таблице S-Box
    low = SBOX[low];

    // Соединяем две половины обратно в 8 бит
    int result = (high << 4) | low;

    return bitset<8>(result);
}

// Обратная операция S-Box
bitset<8> ApplyInverseSBox(bitset<8> input)
{
    int value = static_cast<int>(input.to_ulong());

    // Разделяем 8 бит на две половины
    int high = (value >> 4) & 0x0F;
    int low = value & 0x0F;

    // Используем обратную таблицу
    high = INV_SBOX[high];
    low = INV_SBOX[low];

    // Объединяем половины
    int result = (high << 4) | low;

    return bitset<8>(result);
}

// P-Box переставляет биты
bitset<8> ApplyPBox(bitset<8> input)
{
    bitset<8> output;

    // Переставляем каждый бит по заданному правилу
    output[7] = input[7];
    output[6] = input[3];
    output[5] = input[6];
    output[4] = input[2];
    output[3] = input[5];
    output[2] = input[1];
    output[1] = input[4];
    output[0] = input[0];

    return output;
}

// Обратная P-Box для расшифрования
bitset<8> ApplyInversePBox(bitset<8> input)
{
    bitset<8> output;

    // Возвращаем биты в исходные позиции
    output[7] = input[7];
    output[3] = input[6];
    output[6] = input[5];
    output[2] = input[4];
    output[5] = input[3];
    output[1] = input[2];
    output[4] = input[1];
    output[0] = input[0];

    return output;
}

// Функция шифрования
bitset<8> Encrypt(bitset<8> input)
{
    // Сохраняем исходное значение
    bitset<8> value = input;

    cout << "\nИсходное значение: " << value << endl;

    // Выполняем 8 раундов
    for (int round = 1; round <= ROUNDS; round++)
    {
        // Получаем ключ текущего раунда
        bitset<8> roundKey = GetRoundKey(round);

        // XOR: текущее значение + ключ
        value = XORBits(value, roundKey);

        cout << "\nРаунд " << round << endl;
        cout << "Ключ: " << roundKey << endl;
        cout << "XOR:  " << value << endl;

        // Заменяем биты через S-Box
        value = ApplySBox(value);
        cout << "SBOX: " << value << endl;

        // Переставляем биты через P-Box
        value = ApplyPBox(value);
        cout << "PBOX: " << value << endl;
    }

    // После 8 раундов получаем шифротекст
    cout << "\nЗашифрованное значение: " << value << endl;

    return value;
}

// Функция расшифрования
bitset<8> Decrypt(bitset<8> input)
{
    // Получаем зашифрованное значение
    bitset<8> value = input;

    cout << "\nЗашифрованное значение: " << value << endl;

    // Идём от 8-го раунда к 1-му
    for (int round = ROUNDS; round >= 1; round--)
    {
        cout << "\nРаунд " << round << endl;

        // Отменяем перестановку P-Box
        value = ApplyInversePBox(value);
        cout << "PBOX^-1: " << value << endl;

        // Отменяем замену S-Box
        value = ApplyInverseSBox(value);
        cout << "SBOX^-1: " << value << endl;

        // Получаем ключ текущего раунда
        bitset<8> roundKey = GetRoundKey(round);

        // Отменяем XOR с помощью того же ключа
        value = XORBits(value, roundKey);

        cout << "Ключ:     " << roundKey << endl;
        cout << "XOR:      " << value << endl;
    }

    // Получаем исходное значение
    cout << "\nРасшифрованное значение: " << value << endl;

    return value;
}

int main()
{
    // Подключаем русский язык в консоли
    setlocale(LC_ALL, "Russian");

    int choice;

    // Показываем меню
    cout << "1 - Зашифровать\n";
    cout << "2 - Расшифровать\n";
    cout << "Ваш выбор: ";
    cin >> choice;

    string input;

    // Пользователь вводит 8 бит
    cout << "\nВведите 8-битное значение: ";
    cin >> input;

    // Проверяем количество символов
    if (input.length() != 8)
    {
        cout << "Ошибка! Нужно ввести ровно 8 бит.\n";
        system("pause");
        return 0;
    }

    // Проверяем, что введены только 0 и 1
    for (char c : input)
    {
        if (c != '0' && c != '1')
        {
            cout << "Ошибка! Можно вводить только 0 и 1.\n";
            system("pause");
            return 0;
        }
    }

    // Преобразуем строку в 8-битное значение
    bitset<8> value(input);

    // Если выбран режим шифрования
    if (choice == 1)
    {
        Encrypt(value);
    }
    // Если же выбран режим расшифрования
    else if (choice == 2)
    {
        Decrypt(value);
    }
    else
    {
        // Если же пользователь ввёл неправильный вариант
        cout << "Ошибка! Выберите 1 или 2.\n";
    }

    system("pause");

    return 0;
}

//Результат получается так для расшифрования:                                             
//Вводим 8 бит, например 10101010
//Берётся ключ 10110101 и для каждого раунда немного сдвигается
//XOR — исходные 8 бит сравниваются с ключом побитово
//P - Box — биты переставляются по заданному порядку
//Это повторяется 8 раз
//Вход -> XOR -> S -> Box -> P -> Box -> XOR -> S -> Box -> P -> Box  ... -> результат

//Для расшифрования всё наоборот:
//Вводим зашифрованные 8 бит
//Берётся ключ соответствующего раунда
//P - Box ^ -1 — возвращаем биты в исходный порядок
//S - Box ^ -1 — возвращаем значения обратно по обратной таблице
//XOR — выполняем XOR с ключом.
//Это повторяется 8 раз, от 8 раунда к 1
//Получается:
//Зашифрованное → P - Box^-1 → S - Box ^ -1 → XOR → ... → Исходное