#include "System/Types.h"
#include "System/Window.h"
#include "System/Renderer/Font/FontRenderer.h"
#include "Platform/Windows/Renderer/DirectX12/DX12Object2D.h"
#include "Application/Engine.h"
#include "Engine\Platform\Windows\Renderer\DirectX12\ScreenCapture.h"
#include "System\Physics\AABB2D.h"
#include "Platform\Windows\Renderer\DirectX12\DX12Compute.h"
#include "Application\Editor\Editor.h"
#include "System\Renderer\Font\String2D.h"
#include "Application\UI\Button.h"
#include "System\Renderer\Viewport.h"
#include "Platform\Windows\Utils\MonitorScale.h"

void global_MouseMove(void* parent,Mouse mouse)
{
	Vector* Pos = (Vector*)parent;
	Pos->m128_f32[0] = min( mouse.x ,2560);
	Pos->m128_f32[1] = min( mouse.y ,1440);
}
void ApplicationMain()
{
	EngineSpec spec = {0};
	spec.fullscreen = false;
	spec.windowName = "Tower Defence";
	spec.width = GetSystemMetrics(SM_CXSCREEN);
	spec.height = GetSystemMetrics(SM_CYSCREEN);

	GEngine.Init(spec);

	MouseCallbacks callbacks = { 0 };
	Vector MousePos;
	callbacks.Parent = &MousePos;
	callbacks.OnMove = global_MouseMove;
	GEngine.pWindow->mouseManager.Register(&callbacks);

	Editor editor;
	editor.Init();
	//load and wait to 
	GEngine.ExecuteCommandList();

	while (GEngine.pWindow->Running())
	{
		GEngine.ProcessInputs();
		GEngine.StartFrame();

		editor.Update();
		// Draws the Shader Editor. ready for the GEngine ObjectManager Draw call
		editor.Draw(); 

		GEngine.pRendererInterface->StartRender3D({ 0,0,0,0 });
		GEngine.pRendererInterface->StartRender2D();

		GEngine.pObjManager2D->Draw(&GEngine.pRendererInterface->MainCommandQueue);
		GEngine.EndFrame(&GEngine.pRendererInterface->MainCommandQueue);
	}
}