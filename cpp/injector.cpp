#include "injector.h"
#include <iostream>
#include <TlHelp32.h>

bool inject_dll(DWORD pid, const std::wstring& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return false;
    }

    BOOL isTarget64Bit = FALSE;
    BOOL isCurrentProcess64Bit = FALSE;
    IsWow64Process(GetCurrentProcess(), &isCurrentProcess64Bit);
    IsWow64Process(hProcess, &isTarget64Bit);

    // Check if architectures match
    if (isCurrentProcess64Bit != isTarget64Bit) {
        std::cerr << "Architecture mismatch. Injector and target process must be both 32-bit or both 64-bit." << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Allocate memory in the target process
    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, (dllPath.size() + 1) * sizeof(wchar_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (pDllPath == NULL) {
        std::cerr << "Failed to allocate memory. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, pDllPath, dllPath.c_str(), (dllPath.size() + 1) * sizeof(wchar_t), NULL)) {
        std::cerr << "Failed to write to process memory. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get the address of LoadLibraryA
    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
    if (pLoadLibrary == NULL) {
        std::cerr << "Failed to get LoadLibraryA address. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create a remote thread to call LoadLibraryA
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllPath, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for the thread to finish
    WaitForSingleObject(hThread, 10000);

    // Check the exit code of the thread
    DWORD exitCode;
    if (GetExitCodeThread(hThread, &exitCode) && exitCode == 0) {
        std::wcerr << L"LoadLibraryW failed in the remote process." << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hThread);
        return false;
    }

    // Clean up
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "DLL injected successfully" << std::endl;
    return true;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        std::wcerr << L"Usage: " << argv[0] << L" <PID> <DLL_PATH>" << std::endl;
        return 1;
    }

    DWORD pid = std::wcstoul(argv[1], nullptr, 10);
    std::wstring dllPath = argv[2];


    if (inject_dll(pid, dllPath)) {
        std::wcout << L"Injection successful" << std::endl;
    } else {
        std::wcerr << L"Injection failed" << std::endl;
        return 1;
    }

    return 0;
}