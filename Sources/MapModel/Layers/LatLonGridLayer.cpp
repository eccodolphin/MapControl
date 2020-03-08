#include "LatLonGridLayer.hpp"

namespace MapControl
{
	LatLonGridLayer::LatLonGridLayer() noexcept
	{
		_InitLatLonGrid();
	}

	void LatLonGridLayer::_InitLatLonGrid() noexcept
	{
		// Initialize meridians.
		double lonSpawnStep = 30.0;
		for (double lon = wmMinLon; SafeLessOrEqual(lon, wmMaxLon); lon += lonSpawnStep)
		{
			_latLonGrid.push_back(SectWM{
				PointWM{ wmMinLat, lon },
				PointWM{ wmMaxLat, lon }
			});
		}

		// Initialize parallels.
		double latSpawnStep = lonSpawnStep * (wmFullHDgs / wmFullWDgs);
		for (double lat = wmMinLat; SafeLessOrEqual(lat, wmMaxLat); lat += latSpawnStep)
		{
			_latLonGrid.push_back(SectWM{
				PointWM{ lat, wmMinLon },
				PointWM{ lat, wmMaxLon }
			});
		}

		// Colorize lat/lon grid.
		unsigned greyBright = 125;
		COLORREF greyColor = RGB(greyBright, greyBright, greyBright);
		for (SectWM& latLonSect : _latLonGrid)
		{
			latLonSect.c = greyColor;
		}
	}



	std::vector<SectWM> LatLonGridLayer::GetVisibleLatLonGrid(RectWM wmWindow) noexcept
	{
		std::vector<SectWM> visibleLatLonGrid;
		for (SectWM& latLonGridSection : _latLonGrid)
		{
			if (IsVisible(latLonGridSection, wmWindow))
			{
				visibleLatLonGrid.push_back(latLonGridSection);
			}
		}
		return visibleLatLonGrid;
	}
}