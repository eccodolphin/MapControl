#include "LatLonGridRenderer.hpp"

namespace MapControl
{
	LatLonGridRenderer::LatLonGridRenderer() noexcept
	{
	}

	LatLonGridRenderer::LatLonGridRenderer(
		LatLonGridLayer*	pLatLonGridLayer,
		HWND*				mvHwnd,
		RectInt*			mvUiWindow,
		RectWM*				mvWmWindow,
		double*				mvScaleLat,
		double*				mvScaleLon,
		unsigned*			mvScaleLevel
		) noexcept :
		_pLatLonGridLayer(pLatLonGridLayer),
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



	HBITMAP LatLonGridRenderer::GetLatLonGridLayerBitmap(const std::vector<unsigned>& transparentBitmapPixels) noexcept
	{
		// Create transparent bitmap.
		HBITMAP hLlgLayerBitmap = CreateBitmap(
			_mvUiWindow->w,
			_mvUiWindow->h,
			bmpDefPlanes,
			bmpDefBitCount,
			&transparentBitmapPixels[0]
			);

		// Create HDC for transparent bitmap.
		HDC llgLayerHdc = CreateCompatibleDC(NULL);
		HBITMAP oldLlgLayerHdcBitmap = static_cast<HBITMAP>(SelectObject(llgLayerHdc, hLlgLayerBitmap));

		_DrawLatLonGrid(llgLayerHdc);

		// Free HDC.
		SelectObject(llgLayerHdc, oldLlgLayerHdcBitmap);
		DeleteObject(llgLayerHdc);

		return hLlgLayerBitmap;
	}

	void LatLonGridRenderer::_DrawLatLonGrid(HDC hdc) noexcept
	{
		static const unsigned penW = 1;

		std::vector<SectWM> latLonGrid = _pLatLonGridLayer->GetVisibleLatLonGrid(*_mvWmWindow);

		for (SectWM& llgSect : latLonGrid)
		{
			// Initialize drawing resources.
			HPEN hLlgSectPen = CreatePen(PS_SOLID, penW, llgSect.c);
			HPEN hPenOld = (HPEN)SelectObject(hdc, hLlgSectPen);

			// Draw LLG section.
			SectInt scaledLlgSect = _ScaleSect(GetVisiblePeace(llgSect, *_mvWmWindow));
			_DrawSect(hdc, scaledLlgSect);

			// Utialize drawing resources.
			SelectObject(hdc, hPenOld);
			DeleteObject(hLlgSectPen);
		}
	}

	void LatLonGridRenderer::_DrawSect(HDC hdc, const SectInt& uiSect) noexcept
	{
		MoveToEx(hdc, uiSect.p1.x, uiSect.p1.y, NULL);
		LineTo(hdc, uiSect.p2.x, uiSect.p2.y);
	}
}