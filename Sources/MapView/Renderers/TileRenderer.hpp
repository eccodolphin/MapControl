#pragma once

#include "BaseRenderer.hpp"
#include "TileLayer.hpp"

namespace MapControl
{
	class TileRenderer : public BaseRenderer
	{
	public:
		TileRenderer() noexcept;
		TileRenderer(
			TileLayer*	tileLayer,
			HWND*		mvHwnd,
			RectInt*	mvUiWindow,
			RectWM*		mvWmWindow,
			double*		mvScaleLat,
			double*		mvScaleLon,
			unsigned*	mvScaleLevel
			) noexcept;

	public:
		HBITMAP GetTileLayerBitmap(const std::vector<unsigned>& whiteBitmapPixels) noexcept;
	private:
		void _DrawTiles(HDC tileLayerHdc) noexcept;
		void _DrawTile(HDC tileLayerHdc, const TilePointer& tilePointer) noexcept;
		void _DrawTileBitmap(HDC tileLayerHdc, HBITMAP hTileBitmap, int bmpOriginX, int bmpOriginY) noexcept;

	private:
		TileLayer* _tileLayer;
	};
}