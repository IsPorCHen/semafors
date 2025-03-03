#include <windows.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

#define THREAD_COUNT 3
#define SEMAPHORE_LIMIT 1
#define MAX_WAIT_TIME 5000

int sharedResource = 0;
HANDLE semaphore;

DWORD WINAPI threadFunction(LPVOID lpParam) {
    int threadID = (int)lpParam;
    DWORD startTime = GetTickCount();

    while (true) {
        DWORD waitResult = WaitForSingleObject(semaphore, MAX_WAIT_TIME);

        DWORD currentTime = GetTickCount();
        DWORD waitTime = currentTime - startTime;

        if (waitResult == WAIT_TIMEOUT) {
            std::cout << "Поток " << threadID << " не получил доступ к ресурсу за " << waitTime << " миллисекунд и завершится." << std::endl;
            return 0;
        }

        if (waitResult == WAIT_OBJECT_0) {
            std::cout << "Поток " << threadID << " захватил семафор." << std::endl;

            srand(GetTickCount() + threadID);
            int workTime = rand() % 5000 + 1000;
            std::cout << "Поток " << threadID << " работает с ресурсом в течение " << workTime / 1000.0 << " секунд." << std::endl;

            Sleep(workTime);

            sharedResource++;
            std::cout << "Поток " << threadID << " завершил работу. Значение ресурса: " << sharedResource << std::endl;

            ReleaseSemaphore(semaphore, 1, NULL);
            std::cout << "Поток " << threadID << " освободил семафор." << std::endl;

            startTime = GetTickCount();
        }
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "russian");

    semaphore = CreateSemaphore(NULL, SEMAPHORE_LIMIT, SEMAPHORE_LIMIT, NULL);
    if (semaphore == NULL) {
        std::cerr << "Не удалось создать семафор. Код ошибки: " << GetLastError() << std::endl;
        return 1;
    }

    HANDLE threads[THREAD_COUNT];
    DWORD threadIDs[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads[i] = CreateThread(NULL, 0, threadFunction, (LPVOID)i, 0, &threadIDs[i]);
        if (threads[i] == NULL) {
            std::cerr << "Не удалось создать поток " << i << ". Код ошибки: " << GetLastError() << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        DWORD waitResult = WaitForSingleObject(threads[i], INFINITE);
        if (waitResult == WAIT_OBJECT_0) {
            std::cout << "Поток " << i << " завершился." << std::endl;
        }
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        CloseHandle(threads[i]);
    }

    CloseHandle(semaphore);

    std::cout << "Все потоки завершены." << std::endl;

    return 0;
}
