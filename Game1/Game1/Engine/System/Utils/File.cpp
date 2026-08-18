#include "File.h"
#include <memory>
#include "Windows.h"
#include "../String.h"
bool File::Open(const s8* FilePath,const s8* Mode)
{
	memcpy(Path, FilePath, strlen(FilePath) + 1);
	u32 i = fopen_s(&fp, Path, Mode);
	fseek(fp, 0, SEEK_SET);
	Data = 0;
	return !fp;
}
void File::Write(void* data, u32 size)
{
	fwrite(data, size, 1, fp);
}
void File::Read(void* Buffer, u32 Size)
{
	if (readPtr)
	{
		memcpy(Buffer, readPtr, Size);
		readPtr += Size;
	}
	else
	{
		fread(Buffer, Size, 1, fp);
	}
}
bool File::Load(const s8* FilePath)
{
	memcpy(Path, FilePath, strlen(FilePath) + 1);
	fopen_s(&fp, FilePath, "rb");

	if (!fp)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	FileSize = ftell(fp) + 1;
	fseek(fp, 0, SEEK_SET);

	Data = (u8*)calloc(1,FileSize);
	if (Data)
	{
		u32 ReadBytes = fread(Data, 1, FileSize - 1, fp);
		Data[ReadBytes] = '\0';
		readPtr = Data;
		return true;
	}
	return false;
}

void File::Close()
{
	fclose(fp);
	if (Data)
	{
		free(Data);
	}

}
void FileSystem::AddExtWhitelist(s8* FileExt)
{
	strcpy_s(FileExtWhiteList[FileExtWhitListCount++], 32, FileExt);
}
void FileSystem::AddExtWhitelist(const wchar_t* FileExt)
{
	wcscpy_s(FileExtWhiteListWide[FileExtWhitListCount++], wcslen(FileExt) * 2, FileExt);
}
bool FileSystem::IsFileExtWhiteListed(s8* FileExt)
{
	for (u32 i = 0; i < FileExtWhitListCount; i++)
	{
		if (!memcmp(FileExt, FileExtWhiteList[i], strlen(FileExtWhiteList[i])))
		{
			return true;
		}
	}
}
bool FileSystem::IsFileExtWhiteListed(wchar_t* FileExt)
{
	wchar_t* FileExtPtr = 0;
	for (u32 i = 0; i < FileExtWhitListCount; i++)
	{
		if (wcslen(FileExt) < wcslen(FileExtWhiteListWide[i]))
		{
			continue;
		}

		FileExtPtr = (FileExt + wcslen(FileExt)) - wcslen(FileExtWhiteListWide[i]);

		if (!memcmp(FileExtPtr, FileExtWhiteListWide[i], wcslen(FileExtWhiteListWide[i]) * 2))
		{
			return true;
		}
	}
	return false;
}
void FileSystem::LoadDirectory(s8* Directory)
{
	sprintf_s(WorkingString, "%s/%s", Directory, "*");
	wchar_t* FilePath = CharToWChar(WorkingString);
	WIN32_FIND_DATA  extraData;
	HANDLE FirstFile = FindFirstFileW(FilePath, &extraData);
	if (FirstFile)
	{
		printf("First file found: %ls\n", FilePath);
		free(FilePath);
	}

	while (FindNextFileW(FirstFile, &extraData))
	{
		if (IsFileExtWhiteListed(extraData.cFileName))
		{
			sprintf_s(WorkingString, "%s\\%ls", Directory, extraData.cFileName);
			FileList[FileCount].Load(WorkingString);
			u64 a = wcslen(extraData.cFileName);
			wcstombs_s(&a, FileList[FileCount++].Name, extraData.cFileName, 256);
			printf("First file found: %ls\n", extraData.cFileName);
		}
	}
}

void FileSystem::CloseDirectory()
{
	for (u32 i = 0; i < FileCount; i++)
	{
		FileList[i].Close();
	}
}
int GetLastCharIndex(const char* FilePath, char c)
{
	for (u32 i = strlen(FilePath); i > 0; i--)
	{
		if (FilePath[i] == c)
		{
			return i;
		}
	}
}
void GetFileExtension(const char* FilePath, char* Buffer,u32 BufferLength)
{
	u32 Length = strlen(FilePath);
	u32 i = GetLastCharIndex(FilePath, '.') + 1; // +1 skips the .
	if (Length - i > BufferLength)
	{
		printf("Error: GetFileExtension() Buffer to small for file extension");
		return;
	}
	memcpy(Buffer, &FilePath[i], Length - i);
}