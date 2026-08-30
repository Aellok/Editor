#include "QuadTree.h"
#include "Application\Engine.h"
#define MAX_ELEMENTS 1024
#define MAX_TREE_NODES 64
#define SPLIT_ELEMENT_THRESHOLD 16
MemoryArena ArenaElements;
MemoryArena ArenaNodes;

struct QuadTreeNode
{
	QuadTreeNode* QTNodes;
	AABB Bounds;
	u32 ElementCount;
	Object2D** Elements;
	void Initialize(Vector Min, Vector Max);
	bool ContainsPoint(Vector Pos);
	void Add(Object2D* Object);
};
bool QuadTreeNode::ContainsPoint(Vector Pos) 
{
	if (Pos.m128_f32[0] > Bounds.Min.m128_f32[0] && Pos.m128_f32[0] < Bounds.Max.m128_f32[0])
	{
		if (Pos.m128_f32[1] > Bounds.Min.m128_f32[1] && Pos.m128_f32[1] < Bounds.Max.m128_f32[1])
		{
			return true;
		}
	}
	return false;
}
//Base->QTNodes = (QuadTreeNode**)Nodes.Allocate(sizeof(QuadTreeNode*) * 4);
void QuadTreeNode::Add(Object2D* Object)
{
	if (ElementCount > SPLIT_ELEMENT_THRESHOLD)
	{
		QTNodes = (QuadTreeNode*)ArenaNodes.Allocate(sizeof(QuadTreeNode) * 4);
		Vector Min[4] = { Bounds.Min, // TopLeft
						  { Bounds.Center.m128_f32[0], Bounds.Min.m128_f32[1] }, // Top Right
						  { Bounds.Min.m128_f32[0], Bounds.Center.m128_f32[1] }, // Bottom Left
						  { Bounds.Center.m128_f32[0],Bounds.Center.m128_f32[1] }
		};
		Vector Max[4] = { Bounds.Center, //Top Left
						  {Bounds.Max.m128_f32[0],Bounds.Center.m128_f32[1]}, // Top Right
						  {Bounds.Center.m128_f32[0],Bounds.Max.m128_f32[1]}, // BottomLeft
						  {Bounds.Max.m128_f32[0],Bounds.Max.m128_f32[1]}  // Bottom Right
		};

		for (u32 i = 0; i < 4; i++)
		{
			QTNodes->Initialize(Min[i], Max[i]);
		}
		for (u32 i = 0; i < ElementCount;i++ )
		{
			//u32 Index = 2 * (Object->Center.y > Bounds.Center.m128_f32[1]) + (x < Bounds.Center.m128_f32[0]);

		}
	}

}
void QuadTreeNode::Initialize(Vector Min, Vector Max)
{
	Bounds.Center = DirectX::XMVectorScale(DirectX::XMVectorAdd(Min, Max),0.5);
	Bounds.Min = Min;
	Bounds.Max = Max;
	ElementCount = 0;
	Elements = (Object2D**)ArenaElements.Allocate(sizeof(Object2D*) * SPLIT_ELEMENT_THRESHOLD);
	QTNodes = 0;
}
void QuadTree::Initialize(Vector Min,Vector Max)
{
	if (!ArenaElements.Data && !ArenaNodes.Data)
	{
		ArenaElements.Create(sizeof(Object2D*) * MAX_ELEMENTS);
		ArenaNodes.Create(sizeof(QuadTreeNode) * MAX_TREE_NODES);
	}
	
	Base = (QuadTreeNode*)ArenaNodes.Allocate(sizeof(QuadTreeNode));
	Base->Initialize( Min, Max);
}

//u32 Index = 2 * (y > Bounds.Center.m128_f32[1]) + (x < Bounds.Center.m128_f32[0]);