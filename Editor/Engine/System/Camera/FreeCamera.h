#pragma once
#include "System/MouseManager.h"

void FreeCamera_LButtonDown(void* Parent, Mouse mouse);
void FreeCamera_LButtonUp(void* Parent, Mouse mouse);
void FreeCamera_RButtonDown(void* Parent, Mouse mouse);
void FreeCamera_RButtonUp(void* Parent, Mouse mouse);
void FreeCamera_MButtonDown(void* Parent, Mouse mouse);
void FreeCamera_MButtonUp(void* Parent, Mouse mouse);
void FreeCamera_Move(void* Parent, Mouse mouse);

void FreeCamera_KeyDown(void* Parent, u64 KeyCode);
void FreeCamera_KeyUp(void* Parent, u64 KeyCode);
