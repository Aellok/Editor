#pragma once
#include "System/Types.h"
#include "System/Vector.h"
#include <stdio.h>
#include <string>
struct File
{
	FILE* fp = NULL;
	s8 Path[512] = { 0 };
	u32 FileSize = 0;
	u8* Data = NULL;
	u8* readPtr = NULL;
	s8 Name[256] = {0};
	bool Load(const s8* FilePath);
	void Close();
	bool Open(const s8* FilePath,const s8* Mode);
	void Write(void* data, u32 size);
	void Read(void* Buffer,u32 Size);
};
struct FileSystem
{
	s8 WorkingString[512];
	s8* Directory;

	u32 FileCount = 0;
	File FileList[512];

	u32 FileExtWhitListCount = 0;
	s8 FileExtWhiteList[32][8];
	wchar_t FileExtWhiteListWide[32][8];

	bool IsFileExtWhiteListed(s8* FileExt);
	bool IsFileExtWhiteListed(wchar_t* FileExt);
	void AddExtWhitelist(s8* FileExt);
	void AddExtWhitelist(const wchar_t* FileExt);
	void LoadDirectory(s8* Directory);
	void CloseDirectory();
};

void BackSlashToForwardSlash(char* FilePath);

int GetLastCharIndex(const char* FilePath, char c);
void GetFileExtension(const char* FilePath, char* Buffer, u32 Size);
char* GetEngineRelativePath(char* FilePath);