#include "Waterfrog.h"

Waterfrog::Waterfrog()
{
	initSprite("waterfrog");

	m_oppositeLiquid = TileType::lava;
}