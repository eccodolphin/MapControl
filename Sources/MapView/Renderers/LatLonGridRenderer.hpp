#pragma once

#include "BaseRenderer.hpp"
#include "LatLonGridLayer.hpp"

namespace MapControl
{
	class LatLonGridRenderer : public BaseRenderer
	{
	public:
		LatLonGridRenderer() noexcept;
		LatLonGridRenderer(
			LatLonGridLayer*	pLatLonGridLayer,
			HWND*				mvHwnd,
			RectInt*			mvUiWindow,
			RectWM*				mvWmWindow,
			double*				mvScaleLat,
			double*				mvScaleLon,
			unsigned*			mvScaleLevel
		) noexcept;

	public:
		HBITMAP GetLatLonGridLayerBitmap(const std::vector<unsigned>& transparentBitmapPixels) noexcept;
	private:
		void _DrawSect(HDC hdc, const SectInt& lineUInt) noexcept;
		void _DrawLatLonGrid(HDC hdc) noexcept;

	private:
		LatLonGridLayer* _pLatLonGridLayer;
	};
}