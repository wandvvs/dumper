#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <string>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

namespace hardware_dumper
{
	struct hardware_dump
	{
		std::string date;
		std::string computer_name;
		std::string user_name;
		std::string hwid;
        std::string clipboard_text;
        std::string cpu;
        std::string gpu;

		void print_fields() const
		{
           std::cout << "========================== HARDWARE INFORMATION ===========================\n" << std::endl;
            
            std::cout << "HWID: " << hwid << std::endl;
            std::cout << "CPU: " << cpu << std::endl;
            std::cout << "GPU: " << gpu << std::endl;
			std::cout << "Date: " << date << std::endl;
			std::cout << "Computer Name: " << computer_name << std::endl;
			std::cout << "User Name: " << user_name << std::endl;
            std::cout << "Clipboard Text: " << clipboard_text << std::endl; 

            std::cout << "===========================================================================\n" << std::endl;
		}
	};
	
	std::string get_computer_name()
	{
		char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
		DWORD size = sizeof(computer_name) / sizeof(computer_name[0]);

		if (GetComputerNameA(computer_name, &size))
		{
			return std::string(computer_name);
		}
		else
		{
			std::cerr << "Failed to get computer name: " << GetLastError() << std::endl;
		}

		return "";
	}

	std::string get_current_user()
	{
		char user_name[255];
		DWORD size = sizeof(user_name) / sizeof(user_name[0]);

		if (GetUserNameA(user_name, &size))
		{
			return std::string(user_name);
		}
		else
		{
			std::cerr << "Failed to get current user name: " << GetLastError() << std::endl;
		}

		return "";
	}

	std::string get_hwid()
	{
		HW_PROFILE_INFO hw_profile_info;
		if (GetCurrentHwProfile(&hw_profile_info))
		{
			std::wstring temp(hw_profile_info.szHwProfileGuid);
			std::string hwid(temp.begin(), temp.end());

			return hwid;
		}
		else
		{
			std::cerr << "Failed to get hwid: " << GetLastError() << std::endl;
		}

		return "";
	}

	std::string get_clipboard_text()
	{
		if (!OpenClipboard(nullptr))
		{
			return "";
		}

		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == nullptr)
		{
			CloseClipboard();
			return "";
		}

		char* pszText = static_cast<char*>(GlobalLock(hData));
		if (pszText == nullptr)
		{
			CloseClipboard();
			return "";
		}

		std::string clipboard_text(pszText);

		GlobalUnlock(hData);

		CloseClipboard();

		return clipboard_text;
	}

	std::string get_current_date()
	{
		std::time_t time = std::time(nullptr);
		struct tm local_time;
		localtime_s(&local_time, &time);

		std::stringstream ss;
		ss << std::put_time(&local_time, "%d-%m-%Y %H:%M:%S");

		return ss.str();
	}

    std::string get_cpu_name()
    {
        std::string cpu_name = "Unknown";
        HRESULT hres;

        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            return cpu_name;
        }

        hres = CoInitializeSecurity(
            NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE,
            NULL
        );

        if (FAILED(hres))
        {
            CoUninitialize();
            return cpu_name;
        }

        IWbemLocator* p_loc = NULL;
        hres = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID*)&p_loc
        );

        if (FAILED(hres))
        {
            CoUninitialize();
            return cpu_name;
        }

        IWbemServices* p_svc = NULL;
        hres = p_loc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &p_svc
        );

        if (FAILED(hres))
        {
            p_loc->Release();
            CoUninitialize();
            return cpu_name;
        }

        hres = CoSetProxyBlanket(
            p_svc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE
        );

        if (FAILED(hres))
        {
            p_svc->Release();
            p_loc->Release();
            CoUninitialize();
            return cpu_name;
        }

        IEnumWbemClassObject* p_enumerator = NULL;
        hres = p_svc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_Processor"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &p_enumerator
        );

        if (FAILED(hres))
        {
            p_svc->Release();
            p_loc->Release();
            CoUninitialize();
            return cpu_name;
        }

        IWbemClassObject* p_cls_obj = NULL;
        ULONG u_return = 0;

        while (p_enumerator)
        {
            HRESULT hr = p_enumerator->Next(WBEM_INFINITE, 1, &p_cls_obj, &u_return);
            if (0 == u_return)
            {
                break;
            }

            VARIANT vt_prop;
            hr = p_cls_obj->Get(L"Name", 0, &vt_prop, 0, 0);
            if (hr == S_OK)
            {
                cpu_name = std::string(_bstr_t(vt_prop.bstrVal));
                VariantClear(&vt_prop);
            }
            p_cls_obj->Release();
        }

        p_svc->Release();
        p_loc->Release();
        p_enumerator->Release();
        CoUninitialize();

        return cpu_name;
    }

    std::string get_gpu_name()
    {
        std::string video_card_name = "Unknown";
        HRESULT hres;

        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            return video_card_name;
        }

        hres = CoInitializeSecurity(
            NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_DEFAULT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE,
            NULL
        );

        if (FAILED(hres))
        {
            CoUninitialize();
            return video_card_name;
        }

        IWbemLocator* p_loc = NULL;
        hres = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID*)&p_loc
        );

        if (FAILED(hres))
        {
            CoUninitialize();
            return video_card_name;
        }

        IWbemServices* p_svc = NULL;
        hres = p_loc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &p_svc
        );

        if (FAILED(hres))
        {
            p_loc->Release();
            CoUninitialize();
            return video_card_name;
        }

        hres = CoSetProxyBlanket(
            p_svc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE
        );

        if (FAILED(hres))
        {
            p_svc->Release();
            p_loc->Release();
            CoUninitialize();
            return video_card_name;
        }

        IEnumWbemClassObject* p_enumerator = NULL;
        hres = p_svc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_VideoController"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &p_enumerator
        );

        if (FAILED(hres))
        {
            p_svc->Release();
            p_loc->Release();
            CoUninitialize();
            return video_card_name;
        }

        IWbemClassObject* p_cls_obj = NULL;
        ULONG u_return = 0;

        while (p_enumerator)
        {
            HRESULT hr = p_enumerator->Next(WBEM_INFINITE, 1, &p_cls_obj, &u_return);
            if (0 == u_return)
            {
                break;
            }

            VARIANT vt_prop;
            hr = p_cls_obj->Get(L"Caption", 0, &vt_prop, 0, 0);
            if (hr == S_OK)
            {
                video_card_name = std::string(_bstr_t(vt_prop.bstrVal));
                VariantClear(&vt_prop);
            }
            p_cls_obj->Release();
        }

        p_svc->Release();
        p_loc->Release();
        p_enumerator->Release();
        CoUninitialize();

        return video_card_name;
    }


	hardware_dump get_hardware_struct()
	{
		return
		{
			get_current_date(),
			get_computer_name(),
			get_current_user(),
			get_hwid(),
			get_clipboard_text(),
            get_cpu_name(),
            get_gpu_name()
		};
	}
}
