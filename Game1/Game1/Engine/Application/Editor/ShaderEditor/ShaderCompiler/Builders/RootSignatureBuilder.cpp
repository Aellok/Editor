#include "RootSignatureBuilder.h"
#include "RootSignatureTypes.h"
struct VSRSParamInfo
{
	//D3D12_ROOT_DESCRIPTOR
	DynamicArray Descriptors;
	//D3D12_ROOT_PARAMETER_TYPE
	DynamicArray ParamTypes;
	//u32
	DynamicArray BufferSizes;
};
struct PSRSParamInfo
{
	//D3D12_STATIC_SAMPLER_DESC (2032 max)
	DynamicArray StaticSampler;
	//D3D12_ROOT_DESCRIPTOR_TABLE
	DynamicArray DescriptorTable;
	//D3D12_ROOT_PARAMETER_TYPE
	DynamicArray ParamTypes;
};
void ProcessVSRSParamInfo(VSParserResult vsInfo, RootSignature& rootSig)
{
	//CBVs
	for (u32 i = 0; i < vsInfo.cbvs.Count; i++)
	{
		if (rootSig.RootParams.elementCount > 64) // MaxParams
		{
			printf("Error: Max param count reached\n");
		}

		D3D12_ROOT_DESCRIPTOR Desc;
		Desc.RegisterSpace = 0; // Assume it doesnt have this set in the shader.
		Desc.ShaderRegister = vsInfo.cbvs.Slot[i];
		
		D3D12_ROOT_PARAMETER_TYPE ParamType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	
		rootSig.BufferSizes.Add(&vsInfo.cbvs.Size[i]);

		D3D12_ROOT_PARAMETER rootParam;
		rootParam.ShaderVisibility = (D3D12_SHADER_VISIBILITY)D3D12_SHADER_VISIBILITY_VERTEX;
		rootParam.Descriptor = Desc;
		rootParam.ParameterType = ParamType;
		rootSig.RootParams.Add(&rootParam);

		RootSignatureTypes type = eRS_CBV;
		rootSig.RootParamTypes.Add(&type);
	}
	//UAVS
}
//RootParam = D3D12_ROOT_PARAMETER
void ProcessPSRSParamInfo(PSParserResult psInfo, RootSignature& rootSig)
{
	rootSig.DescriptorRanges.Init(8, sizeof(D3D12_DESCRIPTOR_RANGE));
	D3D12_DESCRIPTOR_RANGE* Ranges = DYNAMIC_ARR_GET_CAST_DATA(D3D12_DESCRIPTOR_RANGE, rootSig.DescriptorRanges);
	u32* Types = DYNAMIC_ARR_GET_CAST_DATA(u32, psInfo.registers.Types);
	u32* Slots = DYNAMIC_ARR_GET_CAST_DATA(u32, psInfo.registers.Slots);
	DataString* VarNames = DYNAMIC_ARR_GET_CAST_DATA(DataString, psInfo.registers.VarNames);
	DataString* DataTypes = DYNAMIC_ARR_GET_CAST_DATA(DataString, psInfo.registers.DataTypes);
	//D3D12_ROOT_PARAMETER* Params = DYNAMIC_ARR_GET_CAST_DATA(D3D12_ROOT_PARAMETER, RootParam);
	for (u32 i = 0; i < psInfo.registers.Types.elementCount; i++)
	{
		if (rootSig.RootParams.elementCount > 64) // MaxParams
		{
			printf("Error: Max param count reached\n");
		}

		switch(Types[i])
		{
			case eTEXTURE:
			{
				D3D12_DESCRIPTOR_RANGE DescTableRange;
				DescTableRange.BaseShaderRegister = Slots[i];
				DescTableRange.NumDescriptors = 1;
				DescTableRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				DescTableRange.RegisterSpace = 0;
				DescTableRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				rootSig.DescriptorRanges.Add(&DescTableRange);
				D3D12_ROOT_DESCRIPTOR_TABLE descTable;
				descTable.NumDescriptorRanges = 1;
				descTable.pDescriptorRanges = &Ranges[rootSig.DescriptorRanges.elementCount - 1];

				D3D12_ROOT_PARAMETER rootParam;
				rootParam.DescriptorTable = descTable;
				rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

				rootSig.RootParams.Add(&rootParam);
				RootSignatureTypes type = eRS_TEXTURE;
				rootSig.RootParamTypes.Add(&type);

				break;
			}
			case eSAMPLER:
			{
				//TODO: Expose this to the ui.
				D3D12_STATIC_SAMPLER_DESC StaticSampler;
				StaticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
				StaticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				StaticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				StaticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
				StaticSampler.MipLODBias = 0;
				StaticSampler.MaxAnisotropy = 0;
				StaticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
				StaticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
				StaticSampler.MinLOD = 0.0f;
				StaticSampler.MaxLOD = D3D12_FLOAT32_MAX;
				StaticSampler.ShaderRegister = 0;
				StaticSampler.RegisterSpace = 0;
				StaticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

				rootSig.Samplers.Add(&StaticSampler);
				break;
			}
		}
	}

}
void SerializeRootRootSig(RootSignature& rootSig)
{
	rootSig.Desc.NumParameters = rootSig.RootParams.elementCount;
	rootSig.Desc.pParameters = (D3D12_ROOT_PARAMETER*)rootSig.RootParams.data;
	rootSig.Desc.NumStaticSamplers = rootSig.Samplers.elementCount;
	rootSig.Desc.pStaticSamplers = (D3D12_STATIC_SAMPLER_DESC*)rootSig.Samplers.data;
	rootSig.Desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	HRESULT hr = D3D12SerializeRootSignature(&rootSig.Desc, D3D_ROOT_SIGNATURE_VERSION_1,&rootSig.CompiledBlob,nullptr);
	if (hr != S_OK)
	{
		printf("Error: BuildRootSig: D3D12SerializeRootSignature Failed to create a root signiture.\n");
	}
}
RootSignature BuildRootSignature(VSParserResult vsInfo, PSParserResult psInfo)
{
	RootSignature res;
	RootSignature* res2 = &res;

	res.RootParams.Init(8, sizeof(D3D12_ROOT_PARAMETER));
	res.RootParamTypes.Init(8, sizeof(u32));
	res.Samplers.Init(8, sizeof(D3D12_STATIC_SAMPLER_DESC));
	res.BufferSizes.Init(8, sizeof(u32));

	ProcessVSRSParamInfo(vsInfo,res);
	ProcessPSRSParamInfo(psInfo,res);
	SerializeRootRootSig(res);
	return res;
}