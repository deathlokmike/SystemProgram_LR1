// Lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "Lab1.h"
#include <conio.h>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

// Единственный объект приложения

CWinApp theApp;
HANDLE hMutex;
HANDLE hEventStart;
HANDLE hEventStop;
HANDLE hEventClose;
vector <HANDLE> vEventClose;
HANDLE hEventConfirm;
HANDLE hEventCloseApp;

DWORD WINAPI MyThread(LPVOID LPParametr)
{
    int num = (int)LPParametr;
    WaitForSingleObject(hMutex, INFINITE);
    cout << "Поток № " << num + 1 << " создан." << endl;
    ReleaseMutex(hMutex);
    WaitForSingleObject(vEventClose[num], INFINITE); // Ожидание своего сигнала закрытия
    WaitForSingleObject(hMutex, INFINITE);
    cout << "Поток № " << num + 1 << " завершен." << endl;
    ReleaseMutex(hMutex);
    return 0;
}

void start()
{
    int i = 0;
    hMutex = CreateMutex(NULL, FALSE, "mutex");
    hEventStart = CreateEvent(NULL, FALSE, FALSE, "event_start");
    hEventStop = CreateEvent(NULL, FALSE, FALSE, "event_stop");
    hEventConfirm = CreateEvent(NULL, FALSE, FALSE, "event_confirm");
    hEventCloseApp = CreateEvent(NULL, FALSE, FALSE, "event_close");
    HANDLE hEvents[] = { hEventStart, hEventStop, hEventCloseApp };
    bool cmdNotClosed = true;
    while (cmdNotClosed)
    {
        switch (WaitForMultipleObjects(3, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
        {
        case 0:
        {
            HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)i++, 0, NULL);
            hEventClose = CreateEvent(NULL, TRUE, FALSE, NULL);
            vEventClose.push_back(hEventClose);
            SetEvent(hEventConfirm);
            break;
        }
        case 1:
        {
            if (!vEventClose.empty()) 
            {
                PulseEvent(vEventClose[--i]); 
                CloseHandle(vEventClose[i]);
                vEventClose.pop_back(); 
                SetEvent(hEventConfirm); 
                break;
            }
        }
        case 2:
        {
            cmdNotClosed = false;
            for (auto ev : vEventClose)
            {
                SetEvent(ev);
                CloseHandle(ev);
            }
            SetEvent(hEventConfirm);
            break;
        }
        }
    }
    CloseHandle(hMutex);
    CloseHandle(hEventStart);
    CloseHandle(hEventStop);
    CloseHandle(hEventConfirm);
}

int main()
{
    setlocale(LC_ALL, "");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int nRetCode = 0;
    HMODULE hModule = ::GetModuleHandle(nullptr);
    cout << "Консольное приложение запущено. " << endl;
    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            start();
        }
    }
    else
    {
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}