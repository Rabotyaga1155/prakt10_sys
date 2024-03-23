#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <string>

DWORD WINAPI ThreadFunction2(LPVOID lpParam) {
	std::cout << "Поток 2 создан.\n";
	std::cout << "Поток 2 завершен.\n";
	return 0;
}

DWORD WINAPI ThreadFunction1(LPVOID lpParam) {
	std::cout << "Поток 1 создан.\n";
	std::string s;
	std::cout << "Введите строку в поток 1 для записи в файл: \n";
	std::cin >> s;
	

	std::ofstream file("file.txt");
	if (file.is_open()) {
		file << s;
		file.close();
		std::cout << "Записано в файл'.\n";
	}
	else {
		std::cerr << "Ошибка.\n";
	}

	return 0;
}

int main() {
	setlocale(LC_ALL, "ru");

	wchar_t lpszComLine[130];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	HANDLE hThread,hInheritProcess;
	DWORD IDThread;

	hThread = GetCurrentProcess();

	// Создание наследуемого дубликата псевдодескриптора текущего процесса
	if (!DuplicateHandle(
		GetCurrentProcess(), // дескриптор текущего процесса
		hThread,            // исходный псевдодескриптор процесса
		GetCurrentProcess(), // дескриптор текущего процесса
		&hInheritProcess,    // новый дескриптор процесса
		0,                   // этот параметр игнорируется
		TRUE,                // новый дескриптор наследуемый
		DUPLICATE_SAME_ACCESS)) // доступ не изменяем
	{
		std::cerr << "Псевдодескриптор не был скопирован.\n";
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	std::cout << "\nВведите любой знак для начала дочернего процесса: ";
	_getch();

	// устанавливаем атрибуты нового процесса
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	wsprintf(lpszComLine, L"C:\\Users\\Incl\\Desktop\\prakt10_por\\x64\\Debug\\prakt10_2.exe %d", (int)hInheritProcess);

	if (!CreateProcess(
		NULL,
		lpszComLine,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
	)) {
		std::cerr << "Новый процесс не создан.\n";
		_cputs("Нажмите любую клавишу, чтобы завершить.\n");
		_getch();
		return GetLastError();
	}

	std::cout << "PID: " << pi.dwProcessId << std::endl;
	std::cout << "Дескриптор: " << pi.hProcess << std::endl;

	WaitForSingleObject(pi.hProcess, INFINITE);

	TerminateThread(pi.hThread, 0);
	TerminateProcess(pi.hProcess, 0);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hInheritProcess);

	#pragma region DuplicateHandleFile

	HANDLE hFile = CreateFile(L"test.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "Не удалось открыть файл.\n";
		return 1;
	}

	HANDLE hFile2;
	if (!DuplicateHandle(GetCurrentProcess(), hFile, GetCurrentProcess(), &hFile2, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		std::cerr << "Не удалось дублировать дескриптор файла.\n";
		return 1;
	}

	char buffer[1024] = "";
	DWORD dwWritten;
	if (!WriteFile(hFile2, buffer, strlen(buffer), &dwWritten, NULL)) {
		std::cerr << "Не удалось записать данные в файл.\n";
		return 1;
	}

	CloseHandle(hFile);
	CloseHandle(hFile2);

	std::ifstream file("test.txt");
	if (!file) {
		std::cerr << "Не удалось открыть файл.\n";
		return 1;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::cout << "Строка из файла записанная дескриптором: " << line << '\n';
	}

	file.close();

	#pragma endregion DuplicateHandleFile

	HANDLE hThread1 = NULL;
	HANDLE hThread2 = NULL;
	char input;

	while (true) {
		if (hThread1 != NULL) {
			WaitForSingleObject(hThread1, INFINITE);
		}
		if (hThread2 != NULL) {
			WaitForSingleObject(hThread2, INFINITE);
		}
		std::cout << "\nВыберите действие:\n";
		std::cout << "1. Создать поток 1\n";
		std::cout << "2. Создать поток 2\n";
		std::cout << "3. Завершить поток 1\n";
		std::cout << "4. Завершить поток 2\n";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin >> input;

		switch (input) {
		case '1':
			if (hThread1 == NULL) {
				hThread1 = CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
				std::cout << "ID потока 1: " << GetThreadId(hThread1) << std::endl;
				if (hThread1 == NULL) {
					std::cerr << "Не удалось создать поток 2.\n";
				}
			}
			else {
				std::cerr << "Поток 2 уже создан.\n";
			}
			break;
		case '2':
			if (hThread2 == NULL) {
				hThread2 = CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
				std::cout << "ID потока 2: " << GetThreadId(hThread2) << std::endl;
				if (hThread2 == NULL) {
					std::cerr << "Не удалось создать поток 1.\n";
				}
			}
			else {
				std::cerr << "Поток 1 уже создан.\n";
			}
			break;
		case '3':
			if (hThread1 != NULL) {
				TerminateThread(hThread1, 0);
				CloseHandle(hThread1);
				hThread1 = NULL;
				std::cout << "Поток 2 завершен.\n";
			}
			else {
				std::cerr << "Поток 2 не был создан.\n";
			}
			break;
		case '4':
			if (hThread2 != NULL) {
				TerminateThread(hThread2, 0);
				CloseHandle(hThread2);
				hThread2 = NULL;
				std::cout << "Поток 1 завершен.\n";
			}
			else {
				std::cerr << "Поток 1 не был создан.\n";
			}
			break;
		
		default:
			std::cerr << "Неверно.\n";
			break;
		}
	}


	return 0;
}
