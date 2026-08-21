#include "String2D.h"
#include "System\Renderer\Object2D.h"
#include "Application\Engine.h"
void String2D::Initialize(DirectX12* DX12,MemoryArena* Arena, const char* String)
{
	Visible = false;
}
void String2D::Draw()
{
	Visible = true;
}