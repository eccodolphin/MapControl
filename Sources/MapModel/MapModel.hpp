#pragma once

#include "TileLayer.hpp"
#include "LatLonGridLayer.hpp"
#include "UserPointsLayer.hpp"

namespace MapControl
{
	class MapModel
	{
	public:
		MapModel(TileRequester& tileRequester, std::mutex& tileServicesMutex) noexcept;

		TileLayer& GetTileLayer() noexcept;
		LatLonGridLayer& GetLatLonGridLayer() noexcept;
		UserPointsLayer& GetUserPointsLayer() noexcept;

	private:
		TileLayer _tileLayer;
		LatLonGridLayer _latLonGridLayer;
		UserPointsLayer _userPointsLayer;
	};
}