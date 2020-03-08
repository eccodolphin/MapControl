#pragma once

#include "TileStructures.hpp"

namespace MapControl
{
	class ITileCache
	{
	public:
		virtual TileSlot* GetPDirtySlot() = 0;
		virtual void AddPCleanSlot(TileSlot* pCleanSlot) = 0;
	};
}