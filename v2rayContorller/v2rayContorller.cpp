// v2rayContorller.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <TlHelp32.h>
#include <atlconv.h>
#include <conio.h>
#include <tchar.h>
#include <time.h>
#include <atomic>
#include <cstdio>
#include <future>
#include <iostream>
#include <map>
#include <string>
#include <thread>

using namespace std;
bool checkStat() {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot 调用失败.\n");
        return -1;
    }
    cout << "v2ray status:" << endl;
    bool bMore, flag = false;
    int processId = -1;
    do {
        flag = !lstrcmp(pe32.szExeFile, L"wv2ray.exe");
        if (flag) {
            processId = pe32.th32ProcessID;
            break;
        }
        bMore = Process32Next(hProcessSnap, &pe32);
    } while (bMore);
    if (flag) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        cout << "RUNNING" << endl;
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE),
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        cout << "process ID:\t" << processId << endl;
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED);
        cout << "STOPPED" << endl;
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE),
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        cout << "process ID:\tN/A" << endl;
    }
    CloseHandle(hProcessSnap);
    time_t t = time(NULL);
    cout << "last check:\t" << ctime(&t) << endl;
    return flag;
}

void autoRestart() {
    system("cls");
    bool flag = false;
    int attemp = 0;
    do {
        flag = checkStat();
        if (!flag) {
            cout << "attempt:" << attemp << ",Restarting..." << endl;
            cout << "exec stat:";
            switch ((int)ShellExecute(NULL, L"open",
                                      L"E://v2ray-v3.6-windows-64//wv2ray.exe",
                                      NULL, NULL, SW_SHOW)) {
                case 0:
                    cout << "successful";
                    break;
                case ERROR_FILE_NOT_FOUND:  // same as SE_ERR_FNF
                    cout << "ERROR_FILE_NOT_FOUND";
                    break;
                case ERROR_PATH_NOT_FOUND:  // same as SE_ERR_PNF
                    cout << "ERROR_PATH_NOT_FOUND";
                    break;
                case ERROR_BAD_FORMAT:
                    cout << "ERROR_BAD_FORMAT";
                    break;
                case SE_ERR_ACCESSDENIED:
                    cout << "SE_ERR_ACCESSDENIED";
                    break;
                case SE_ERR_ASSOCINCOMPLETE:
                    cout << "SE_ERR_ASSOCINCOMPLETE";
                    break;
                case SE_ERR_DDEBUSY:
                    cout << "SE_ERR_DDEBUSY";
                    break;
                case SE_ERR_DDEFAIL:
                    cout << "SE_ERR_DDEFAIL";
                    break;
                case SE_ERR_DDETIMEOUT:
                    cout << "SE_ERR_DDETIMEOUT";
                    break;
                case SE_ERR_DLLNOTFOUND:
                    cout << "SE_ERR_DLLNOTFOUND";
                    break;
                case SE_ERR_NOASSOC:
                    cout << "SE_ERR_NOASSOC";
                    break;
                case SE_ERR_OOM:
                    cout << "SE_ERR_OOM";
                    break;
                case SE_ERR_SHARE:
                    cout << "SE_ERR_SHARE";
                    break;
            }
            cout << endl;
            // system("PAUSE");
            this_thread::sleep_for(chrono::milliseconds(1000));
            system("cls");
        } else {
            break;
        }
        if (++attemp >= 5) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                    FOREGROUND_INTENSITY | FOREGROUND_RED);
            cout << "ALL 5 ATTEMPS FAILED. QUITTING." << endl;
            SetConsoleTextAttribute(
                GetStdHandle(STD_OUTPUT_HANDLE),
                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            exit(0);
        }
    } while (!flag);
}

void terminateV2ray() {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot 调用失败.\n");
        return;
    }
    bool bMore, flag = false;
    int processId = -1;
    do {
        flag = !lstrcmp(pe32.szExeFile, L"wv2ray.exe");

        if (flag) {
            HANDLE hProcess =
                OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pe32.th32ProcessID);
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
            break;
        }
        bMore = ::Process32Next(hProcessSnap, &pe32);
    } while (bMore);
    ::CloseHandle(hProcessSnap);
}

char getInput() {
    char input;
    scanf("%c", &input);
    return input;
}

int main() {
    autoRestart();
    char tf = 'n';
    chrono::milliseconds span(1000);
    while (tf != 'y') {
        future<char> fut = async(getInput);
        while (fut.wait_for(span) == future_status::timeout) {
            autoRestart();
            cout << "Do you wish to terminate it?(y/n) ";
        }
        tf = fut.get();
        if (tf == 'y') {
            terminateV2ray();
            this_thread::sleep_for(chrono::milliseconds(1000));
            system("cls");
            checkStat();
            return 0;
        }
    }

    return 0;
}