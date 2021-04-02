#include "GetServer.h"
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <stdio.h>

#define MAX_SERVICE_SIZE 1024 * 64
#define MAX_QUERY_SIZE   1024 * 8

void GetServerlist()
{
    try
    {
        do {
            SC_HANDLE SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (SCMan == NULL) 
            {
                std::cout << "OpenSCManager failed." << std::endl;
                break;
            }
            LPENUM_SERVICE_STATUS service_status;
            DWORD cbBytesNeeded = NULL;
            DWORD ServicesReturned = NULL;
            DWORD ResumeHandle = NULL;

            service_status = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, MAX_SERVICE_SIZE);


            BOOL ESS = EnumServicesStatus(SCMan,                        //���
                SERVICE_WIN32,                                            //��������
                SERVICE_STATE_ALL,                                        //�����״̬
                (LPENUM_SERVICE_STATUS)service_status,                    //���������ϵͳ����Ľṹ
                MAX_SERVICE_SIZE,                                        //�ṹ�Ĵ�С
                &cbBytesNeeded,                                            //������������շ�������ķ���
                &ServicesReturned,                                        //������������շ��ط��������
                &ResumeHandle);                                            //���������������һ�ε��ñ���Ϊ0������Ϊ0����ɹ�
            if (ESS == NULL) 
            {
                std::cout << "EnumServicesStatus Failed." << std::endl;
                break;
            }
            for (int i = 0; i < static_cast<int>(ServicesReturned); i++)
            {
                //std::wstring strName = service_status[i].lpDisplayName;
                //std::wcout << "ServiceName: " << strName.c_str() << "\t";
                switch (service_status[i].ServiceStatus.dwCurrentState)
                { // ����״̬
                case SERVICE_CONTINUE_PENDING:
                    std::cout << "CONTINUE_PENDING" << std::endl;
                    break;
                case SERVICE_PAUSE_PENDING:
                    std::cout << "PAUSE_PENDING" << std::endl;
                    break;
                case SERVICE_PAUSED:
                    std::cout << "PAUSED" << std::endl;
                    break;
                case SERVICE_RUNNING:
                    std::cout << "RUNNING" << std::endl;
                    break;
                case SERVICE_START_PENDING:
                    std::cout << "START_PENDING" << std::endl;
                    break;
                case SERVICE_STOPPED:
                    std::cout << "STOPPED" << std::endl;
                    break;
                default:
                    std::cout << "UNKNOWN" << std::endl;
                    break;
                }
                DWORD dwBytesRead = 0, cbBuffSize = 0, dwError = 0;
                LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL;                //������ϸ��Ϣ�ṹ
                LPSERVICE_DESCRIPTION lpsd = NULL;
                SC_HANDLE service_curren = NULL;                            //��ǰ�ķ�����
                service_curren = OpenService(SCMan, service_status[i].lpServiceName, SERVICE_QUERY_CONFIG);            //�򿪵�ǰ����
                lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, MAX_QUERY_SIZE);                        //�����ڴ棬 ���Ϊ8kb 

                if (NULL == QueryServiceConfig(service_curren, lpServiceConfig, MAX_QUERY_SIZE, &ResumeHandle))
                {
                    std::cout << "QueryServiceConfig Failed." << std::endl;
                    break;
                }
                if (NULL == QueryServiceConfig2(service_curren, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwBytesRead))
                {
                    dwError = ::GetLastError();
                    if (ERROR_INSUFFICIENT_BUFFER == dwError)
                    {
                        cbBuffSize = dwBytesRead;
                        lpsd = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBuffSize);
                    }
                    else
                    {
                        std::cout << "QueryServiceConfig2 Failed." << std::endl;
                        break;
                    }
                }
                if (NULL == QueryServiceConfig2(service_curren, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)lpsd, cbBuffSize, &dwBytesRead))
                {
                    std::cout << "QueryServiceConfig2 Failed." << std::endl;
                    break;
                }
                // Print the configuration information.

                _tprintf(TEXT("%s configuration: \n"), service_status[i].lpDisplayName);
                _tprintf(TEXT("  Type: 0x%x\n"), lpServiceConfig->dwServiceType);
                _tprintf(TEXT("  Start Type: 0x%x\n"), lpServiceConfig->dwStartType);
                _tprintf(TEXT("  Error Control: 0x%x\n"), lpServiceConfig->dwErrorControl);
                _tprintf(TEXT("  Binary path: %s\n"), lpServiceConfig->lpBinaryPathName);
                _tprintf(TEXT("  Account: %s\n"), lpServiceConfig->lpServiceStartName);

                if (lpsd->lpDescription != NULL && lstrcmp(lpsd->lpDescription, TEXT("")) != 0)
                    _tprintf(TEXT("  Description: %s\n"), lpsd->lpDescription);
                if (lpServiceConfig->lpLoadOrderGroup != NULL && lstrcmp(lpServiceConfig->lpLoadOrderGroup, TEXT("")) != 0)
                    _tprintf(TEXT("  Load order group: %s\n"), lpServiceConfig->lpLoadOrderGroup);
                if (lpServiceConfig->dwTagId != 0)
                    _tprintf(TEXT("  Tag ID: %d\n"), lpServiceConfig->dwTagId);
                if (lpServiceConfig->lpDependencies != NULL && lstrcmp(lpServiceConfig->lpDependencies, TEXT("")) != 0)
                    _tprintf(TEXT("  Dependencies: %s\n"), lpServiceConfig->lpDependencies);
                
                CloseServiceHandle(service_curren);
                LocalFree(lpServiceConfig);
                LocalFree(lpsd);
            }
            CloseServiceHandle(SCMan);
        } while (false);
    }
    catch(std::exception &ec)
    {
        std::string strError = ec.what();
        printf("********* exception: %s *********\n", strError.c_str());
    }
    return;
}