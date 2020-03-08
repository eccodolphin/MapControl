#pragma once

#include <mutex>
#include <exception>

#include "BaseLayer.hpp"

#include "TileStructures.hpp"
#include "TileCache.hpp"
#include "TileRequester.hpp"

namespace MapControl
{
	class TileLayer : public BaseLayer
	{
	public:
		enum class Status
		{
			Clean,
			Dirty
		};

	public:
		TileLayer(TileRequester& tileRequester, std::mutex& tileServicesMutex) noexcept;
	private:
		void _InitTilePlaceholder() noexcept;

	public:
		std::vector<TilePointer> GetVisibleTiles(RectWM wmWindow, unsigned lod, Status& status) noexcept;

	private:
		TileCache _tileCache;
		std::mutex& _tileServicesMutex;
		std::vector<unsigned char> _tilePlaceholder; // White bitmap.
	};
}