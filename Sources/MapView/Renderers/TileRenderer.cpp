#include "TileRenderer.hpp"

namespace MapControl
{
	TileRenderer::TileRenderer() noexcept
	{
	}

	TileRenderer::TileRenderer(
		TileLayer*	tileLayer,
		HWND*		mvHwnd,
		RectInt*	mvUiWindow,
		RectWM*		mvWmWindow,
		double*		mvScaleLat,
		double*		mvScaleLon,
		unsigned*	mvScaleLevel
		) noexcept :
		_tileLayer(tileLayer),
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



	HBITMAP TileRenderer::GetTileLayerBitmap(const std::vector<unsigned>& transparentBitmapPixels) noexcept
	{
		// Create transparent bitmap.
		HBITMAP tileLayerBitmap = CreateBitmap(
			_mvUiWindow->w,
			_mvUiWindow->h,
			bmpDefPlanes,
			bmpDefBitCount,
			&transparentBitmapPixels[0]
			);

		// Create HDC for transparent bitmap.
		HDC tileLayerHdc = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(tileLayerHdc, tileLayerBitmap));

		_DrawTiles(tileLayerHdc);

		// Free HDC.
		SelectObject(tileLayerHdc, oldBitmap);
		DeleteObject(tileLayerHdc);

		return tileLayerBitmap;
	}

	void TileRenderer::_DrawTiles(HDC tileLayerHdc) noexcept
	{
		// Get tiles.
		TileLayer::Status tileLayerStatus;
		std::vector<TilePointer> tilePointers = _tileLayer->GetVisibleTiles(
			*_mvWmWindow,
			*_mvScaleLevel,
			tileLayerStatus
			);

		// Draw tiles.
		for (const TilePointer& tilePointer : tilePointers)
		{
			_DrawTile(tileLayerHdc, tilePointer);
		}

		// Generate redraw message if necessary.
		if (tileLayerStatus == TileLayer::Status::Dirty)
		{
			InvalidateRect(*_mvHwnd, NULL, TRUE);
		}
	}

	void TileRenderer::_DrawTile(HDC tileLayerHdc, const TilePointer& tilePointer) noexcept
	{
		// Get tile image data.
		const std::vector<unsigned char>& tileImage = *tilePointer.image_ptr;
		const VOID* tileImagePixelsPtr = &tileImage[0];
		HBITMAP hTileBitmap = CreateBitmap(wmTileWPix, wmTileHPix, 1, 32, tileImagePixelsPtr);

		// Calculate tile w/h in degrees.
		int wmWHTls = static_cast<int>(std::pow(2, *_mvScaleLevel));
		double tileWDgs = wmFullWDgs / static_cast<double>(wmWHTls);
		double tileHDgs = wmFullHDgs / static_cast<double>(wmWHTls);

		// Calculate tile origin.
		double tileUnscaledOriginLon = wmMinLon + tilePointer.key.x * tileWDgs;
		double tileUnscaledOriginLat = wmMinLat + tilePointer.key.y * tileHDgs;
		int tileScaledOriginX = static_cast<int>(
			(tileUnscaledOriginLon - _mvWmWindow->origin.lon) * (*_mvScaleLon)
			);
		int tileScaledOriginY = static_cast<int>(
			(tileUnscaledOriginLat - _mvWmWindow->origin.lat) * (*_mvScaleLat)
			);

		_DrawTileBitmap(tileLayerHdc, hTileBitmap, tileScaledOriginX, tileScaledOriginY);

		DeleteObject(hTileBitmap);
	}

	void TileRenderer::_DrawTileBitmap(HDC tileLayerHdc, HBITMAP hTileBitmap, int tbmpOriginX, int tbmpOriginY) noexcept
	{
		BITMAP tileBitmapInfo;
		GetObject(hTileBitmap, sizeof(BITMAP), &tileBitmapInfo);

		HDC bitmapHdc = CreateCompatibleDC(tileLayerHdc);
		HGDIOBJ oldBitmapHdcBitmap = SelectObject(bitmapHdc, hTileBitmap);

		BitBlt(
			tileLayerHdc, tbmpOriginX, tbmpOriginY, tileBitmapInfo.bmWidth, tileBitmapInfo.bmHeight,
			bitmapHdc, 0, 0, 
			SRCCOPY
			);

		SelectObject(bitmapHdc, oldBitmapHdcBitmap);
		DeleteDC(bitmapHdc);
	}
}