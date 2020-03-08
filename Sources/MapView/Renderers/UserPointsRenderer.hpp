#pragma once

#include "BaseRenderer.hpp"
#include "UserPointsLayer.hpp"

namespace MapControl
{
	// User point drawing parametrs.

	const unsigned userPointPenW = 1;
	const COLORREF qUserPointColor = RGB(255, 255, 0); // Yellow.
	const int userPointRadius = 10; // Pixels.



	class UserPointsRenderer : public BaseRenderer
	{
	public:
		UserPointsRenderer() noexcept;
		UserPointsRenderer(
			UserPointsLayer*	pUserPointsLayer,
			HWND*				mvHwnd,
			RectInt*			mvUiWindow,
			RectWM*				mvWmWindow,
			double*				mvScaleLat,
			double*				mvScaleLon,
			unsigned*			mvScaleLevel
		) noexcept;

	public:
		HBITMAP GetUserPointsLayerBitmap(const std::vector<unsigned>& transparentBitmapPixels) noexcept;
	private:
		void _DrawUserPoints(HDC wHdc) noexcept;
		void _DrawPoint(HDC wHdc, const PointInt& pointUInt) noexcept;

	private:
		UserPointsLayer* _pUserPointsLayer;
	};
}