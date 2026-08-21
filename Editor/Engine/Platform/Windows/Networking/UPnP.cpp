#include "UPnP.h"
#include <upnp.h>
#include <stdio.h>
#include <natupnp.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "user32.lib")

IUPnPNAT* UPnPNAT = NULL;
IStaticPortMappingCollection* PortMappings;

void UPnP::Init()
{
	EntryCount = 0;
	CoInitialize(NULL);
	CoCreateInstance(CLSID_UPnPNAT, NULL, CLSCTX_INPROC_SERVER, IID_IUPnPNAT, (void**)&UPnPNAT);
	UPnPNAT->get_StaticPortMappingCollection(&PortMappings);
}
bool UPnP::Remove(UPnPEntry entry)
{
	HRESULT hr = PortMappings->Remove(entry.Port, entry.Protocol);
	return SUCCEEDED(hr);
}
bool UPnP::AddEntry(const char* InternalIp, const char* ExternalIp, const char* Desc, const char* Protocol, long Port)
{
	size_t num_chars_converted{};
	mbstowcs_s(&num_chars_converted, Entries[EntryCount].InternalIp, InternalIp, 128);
	mbstowcs_s(&num_chars_converted, Entries[EntryCount].ExternalIp, ExternalIp, 128);
	mbstowcs_s(&num_chars_converted, Entries[EntryCount].Desc, Desc, 128);
	mbstowcs_s(&num_chars_converted, Entries[EntryCount].Protocol, Protocol, 128);
	Entries[EntryCount].Port = Port;

	IStaticPortMapping* PortMap;
	HRESULT hr = PortMappings->Add(Entries[EntryCount].Port, Entries[EntryCount].Protocol, Entries[EntryCount].Port, Entries[EntryCount].InternalIp, VARIANT_TRUE, Entries[EntryCount].Desc, &PortMap);
	if (FAILED(hr))
	{
		return false;
	}
	EntryCount++;
	PortMap->Release();
	return true;
}
void UPnP::PrintMappings()
{
	long IPort = 0;
	long EPort = 0;
	BSTR ExternalIP = NULL;
	BSTR Description = NULL;
	BSTR Proto = NULL;
	BSTR Client = NULL;
	VARIANT_BOOL Enabled = false;

	IUnknown* PortEnumInterface;
	IEnumVARIANT* PortEntries = NULL;
	VARIANT CurrentDevice;
	VariantInit(&CurrentDevice);
	
	PortMappings->get__NewEnum(&PortEnumInterface);

	PortEnumInterface->QueryInterface(IID_IEnumVARIANT, (void**)&PortEntries);
	
	PortEntries->Reset();
	
	while(PortEntries->Next(1,&CurrentDevice,NULL) == S_OK)
	{
		IStaticPortMapping* Mapping = NULL;
		IDispatch* Device = V_DISPATCH(&CurrentDevice);

		if (SUCCEEDED(Device->QueryInterface(&Mapping)))
		{
			Mapping->get_InternalPort(&IPort);
			Mapping->get_ExternalPort(&EPort);
			Mapping->get_ExternalIPAddress(&ExternalIP);
			Mapping->get_Description(&Description);
			Mapping->get_Enabled(&Enabled);
			Mapping->get_Protocol(&Proto);
			Mapping->get_InternalClient(&Client);

			printf("Client              : %ls\n", Client);
			printf("Enabled             : %s\n", Enabled == -1 ? "true" : "false");
			printf("Protocol            : %ls\n", Proto);
			printf("Description         : %ls\n", Description);
			printf("Internal port       : %d\n", IPort);
			printf("External port       : %d\n", EPort);
			printf("External IP Address : %ls\n", ExternalIP);

			Mapping->Release();
		}
		Device->Release();
	}
}