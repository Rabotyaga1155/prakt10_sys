// practica10_addition.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <windows.h>
#include <conio.h>
#include <iostream>

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "ru");

	int value = atoi(argv[1]);
	std::cout << "Полученное значение: " << atoi(argv[1]) << std::endl;
	

	HANDLE hThread;
	char c;

	// преобразуем символьное представление дескриптора в число
	hThread = (HANDLE)atoi(argv[1]);

	// ждем команды о завершении потока
	while (true) {
		std::cout << "\nВведите 't' для завершения процесса: ";
		c = _getch();
		if (c == 't') {
			std::cout << "t\n";
			break;
		}
	}

	TerminateThread(hThread, 0);
	CloseHandle(hThread);

	return 0;
}