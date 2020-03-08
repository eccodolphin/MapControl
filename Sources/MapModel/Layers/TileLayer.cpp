#include "TileLayer.hpp"

namespace MapControl
{
	TileLayer::TileLayer(TileRequester& tileRequester, std::mutex& tileServicesMutex) noexcept :
		_tileCache(tileRequester), _tileServicesMutex(tileServicesMutex)
	{
		_InitTilePlaceholder();
	}

	void TileLayer::_InitTilePlaceholder() noexcept
	{
		unsigned size = wmTileWPix * wmTileHPix * pixelComponentsQuantity;
		unsigned char component = 0xFF;

		_tilePlaceholder.resize(size, component);
	}



	std::vector<TilePointer> TileLayer::GetVisibleTiles(RectWM wmWindow, unsigned lod, Status& status) noexcept
	{
		// Set up x/y limits.
		static const int minTileXY = 0;
		int maxTileXY = SafePow(2, lod) - 1;

		// Tile w/h in degrees.
		double tileWDgs = wmFullWDgs / static_cast<double>(maxTileXY + 1);
		double tileHDgs = wmFullHDgs / static_cast<double>(maxTileXY + 1);

		// Calculate x-y boundaries of visible tiles.
		unsigned leftBorderXTls = static_cast<unsigned>(
			max(static_cast<int>((wmWindow.origin.lon - wmMinLon) / tileWDgs), minTileXY)
			);
		unsigned rightBorderXTls = static_cast<unsigned>(
			min(static_cast<int>((wmWindow.extrem.lon - wmMinLon) / tileWDgs), maxTileXY)
			);
		unsigned upBorderYTls = static_cast<unsigned>(
			max(static_cast<int>((wmWindow.origin.lat - wmMinLat) / tileHDgs), minTileXY)
			);
		unsigned downBorderYTls = static_cast<unsigned>(
			min(static_cast<int>((wmWindow.extrem.lat - wmMinLat) / tileHDgs), maxTileXY)
			);

		// Grab visible tiles.
		std::vector<TilePointer> visibleTiles;
		status = Status::Clean;
		std::unique_lock<std::mutex> tileServicesLock(_tileServicesMutex);
		for (unsigned x = leftBorderXTls; x <= rightBorderXTls; ++x)
		{
			for (unsigned y = upBorderYTls; y <= downBorderYTls; ++y)
			{
				TileKey visibleTileKey = TileKey{ lod, x, y };
				std::vector<unsigned char> const* visibleTileImage = _tileCache.GetTileImage(visibleTileKey);
				if (visibleTileImage)
				{
					visibleTiles.push_back(TilePointer{ visibleTileKey, visibleTileImage } );
				}
				else
				{
					visibleTiles.push_back(TilePointer{ visibleTileKey, &_tilePlaceholder });
					status = Status::Dirty;
				}
			}
		}
		return visibleTiles;
	}
}