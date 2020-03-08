#pragma once

#include <Windows.h>
#include <windowsx.h>

#include <cmath>
#include <exception>

#include "BaseWindow.hpp"
#include "LayerPanelWindow.hpp"

#include "MapView.hpp"

namespace MapControl
{
	const unsigned ID_LAYER_PANEL_ITEM = 9001;



	class WindowCreationFailed : public std::exception 
	{
	};



	class MapViewWindow : public BaseWindow<MapViewWindow>
	{
	public:
		MapViewWindow(MapModel& mapModel) noexcept;
		void GiveHwndToMapView() noexcept;
	private:
		void _InitMenu() noexcept;
		void _InitLayerPanelWindow();


	public:
		LPCWSTR ClassName() const noexcept;
		
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void HandleKeyDown(const MSG& msg) noexcept;


	private:
		MapView _mapView;

		LayerPanelWindow _layerPanelWindow;

		LPCWSTR _className = L"Map View Window Class";
	};
}