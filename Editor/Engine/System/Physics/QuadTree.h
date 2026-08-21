#pragma once
#include "AABB.h"
#include "System\Memory\Arena.h"
#include "System\Renderer\Object2D.h"

struct QuadTreeNode;

struct QuadTree
{
	QuadTreeNode* Base;
	void Initialize( Vector Min, Vector Max);
	void Add(Object2D* Object);
};