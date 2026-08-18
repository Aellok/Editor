#define WIN32_LEAN_AND_MEAN
#include "WinSaveInterface.h"
#include <Windows.h>
#include <shobjidl.h>
#include <stdio.h>
#include "System\Utils\File.h"
#include "System\String.h"



bool WinDialog_Save(void* FileData,u32 FileSize)
{
	File f;
	bool Success = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
	{
		return false;
	}
	IFileSaveDialog* pSaveDialog;
	hr = CoCreateInstance(CLSID_FileSaveDialog,NULL,CLSCTX_ALL,IID_PPV_ARGS(&pSaveDialog));
	if (FAILED(hr))
	{
		goto SaveDialog;
	}
	
	hr = pSaveDialog->Show(NULL);
	if (FAILED(hr))
	{
		goto GetResult;
	}

	IShellItem* pItem;
	hr = pSaveDialog->GetResult(&pItem);
	if (FAILED(hr))
	{
		goto GetResult;
	}
	PWSTR filePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	if (FAILED(hr))
	{
		goto GetDisplayName;
	}
	//Write out the data.
	s8 FilePathBuffer[FILENAME_MAX];
	WideCharToMultiByte(CP_UTF8, 0, filePath, -1, FilePathBuffer, FILENAME_MAX, NULL, NULL);
	

	f.Open(FilePathBuffer, "wb");
	f.Write(FileData, FileSize);
	f.Close();
	
	CoTaskMemFree(filePath);
GetDisplayName:
	pItem->Release();
GetResult:
	pSaveDialog->Release();
SaveDialog:
	CoUninitialize();

	return SUCCEEDED(hr);
}
DialogInfo WinDialog_Load()
{
	DialogInfo info = {0,0,0};

	File f;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
	{
		return {0,0,0};
	}
	IFileOpenDialog* pOpenDialog;
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pOpenDialog));
	if (FAILED(hr))
	{
		goto OpenDialog;
	}
	hr = pOpenDialog->Show(NULL);
	if (FAILED(hr))
	{
		goto GetResult;
	}

	IShellItem* pItem;
	hr = pOpenDialog->GetResult(&pItem);
	if (FAILED(hr))
	{
		goto GetResult;
	}
	PWSTR filePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	if (FAILED(hr))
	{
		goto GetDisplayName;
	}
	//Write out the data.
	
	WideCharToMultiByte(CP_UTF8, 0, filePath, -1, info.FilePath, FILENAME_MAX, NULL, NULL);

	f.Load(info.FilePath);

	info.Size = f.FileSize;
	info.data = malloc(info.Size);
	
	f.Read(info.data, f.FileSize);
	f.Close();

	CoTaskMemFree(filePath);
GetDisplayName:
	pItem->Release();
GetResult:
	pOpenDialog->Release();
OpenDialog:
	CoUninitialize();


	return info;
}