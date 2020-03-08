#include "UserPointsRenderer.hpp"

namespace MapControl
{
	UserPointsRenderer::UserPointsRenderer() noexcept
	{
	}

	UserPointsRenderer::UserPointsRenderer(
		UserPointsLayer*	pUserPointsLayer,
		HWND*				mvHwnd,
		RectInt*			mvUiWindow,
		RectWM*				mvWmWindow,
		double*				mvScaleLat,
		double*				mvScaleLon,
		unsigned*			mvScaleLevel
		) noexcept :
		_pUserPointsLayer(pUserPointsLayer),
		BaseRenderer(
			mvHwnd,
			mvUiWindow,
			mvWmWindow,
			mvScaleLat,
			mvScaleLon,
			mvScaleLevel
		)
	{
	}



	HBITMAP UserPointsRenderer::GetUserPointsLayerBitmap(const std::vector<unsigned>& transparentBitmapPixels) noexcept
	{
		// Create transparent bitmap.
		HBITMAP hUpLayerBitmap = CreateBitmap(
			_mvUiWindow->w,
			_mvUiWindow->h,
			bmpDefPlanes,
			bmpDefBitCount,
			&transparentBitmapPixels[0]
		);

		// Create HDC for transparent bitmap.
		HDC upLayerHdc = CreateCompatibleDC(NULL);
		HBITMAP oldUpLayerHdcBitmap = static_cast<HBITMAP>(SelectObject(upLayerHdc, hUpLayerBitmap));

		_DrawUserPoints(upLayerHdc);

		// Free HDC.
		SelectObject(upLayerHdc, oldUpLayerHdcBitmap);
		DeleteObject(upLayerHdc);

		return hUpLayerBitmap;
	}

	void UserPointsRenderer::_DrawUserPoints(HDC hdc) noexcept
	{
		// Get user ponts
		const std::vector<PointWM> userPoints = _pUserPointsLayer->GetVisibleUserPoints(*_mvWmWindow);

		// Initialize drawing resources.
		HBRUSH hUserPointBrush = CreateSolidBrush(qUserPointColor);
		HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc, hUserPointBrush);
		HPEN hUserPointPen = CreatePen(PS_SOLID, userPointPenW, qUserPointColor);
		HPEN hPenOld = (HPEN)SelectObject(hdc, hUserPointPen);

		// Draw user points.
		for (const PointWM& userPoint : userPoints)
		{
			PointInt scaledUserPoint = _ScalePoint(userPoint);
			_DrawPoint(hdc, scaledUserPoint);
		}

		// Utialize drawing resources.
		SelectObject(hdc, hPenOld);
		DeleteObject(hUserPointPen);
		SelectObject(hdc, hBrushOld);
		DeleteObject(hUserPointBrush);
	}

	void UserPointsRenderer::_DrawPoint(HDC hdc, const PointInt& pointUInt) noexcept
	{

		Ellipse(
			hdc,
			pointUInt.x - userPointRadius,
			pointUInt.y - userPointRadius,
			pointUInt.x + userPointRadius,
			pointUInt.y + userPointRadius
		);
	}
}