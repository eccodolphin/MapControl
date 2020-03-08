#include "MapViewWindow.hpp"

namespace MapControl
{

	MapViewWindow::MapViewWindow(MapModel& mapModel) noexcept :
		_mapView(mapModel)
	{
	}

	void MapViewWindow::GiveHwndToMapView() noexcept
	{
		_mapView.SetHwnd(m_hwnd);
	}

	void MapViewWindow::_InitMenu() noexcept
	{
		HMENU hMenu = CreateMenu();
		AppendMenu(hMenu, MF_STRING, ID_LAYER_PANEL_ITEM, L"Layer panel");
		SetMenu(m_hwnd, hMenu);
	}

	void MapViewWindow::_InitLayerPanelWindow()
	{
		unsigned lpwDefW = 220;
		unsigned lpwDefH = 132;

		DWORD lpwStyle = (
			WS_OVERLAPPEDWINDOW &
			(~WS_THICKFRAME)	&
			(~WS_MINIMIZEBOX)	&
			(~WS_MAXIMIZEBOX)
			);

		_layerPanelWindow = LayerPanelWindow(&_mapView, m_hwnd);
		if (!_layerPanelWindow.Create(
			L"Layer Panel",
			lpwStyle,
			lpwDefW, lpwDefH, 0, 0
			))
		{
			throw WindowCreationFailed();
		}
	}



	LPCWSTR MapViewWindow::ClassName() const noexcept
	{
		return _className;
	}



	LRESULT MapViewWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static unsigned wNewW, wNewH;
		static LPMINMAXINFO lpMMI;

		static const int mouseWheelUnitsPerAction = 120;
		static int crsPrevX, crsPrevY, crsCurX, crsCurY;
		static int mouseWheelDelta, mouseWheelActions;

		switch (uMsg)
		{
		case WM_CREATE:
			_InitMenu();
			_InitLayerPanelWindow();
			return 0;

		case WM_PAINT:
			_mapView.Redraw();
			return 0;

		case WM_LBUTTONDBLCLK:
			_mapView.AddUserPoint(PointInt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) });
			InvalidateRect(m_hwnd, NULL, TRUE);
			return 0;

		case WM_SIZE:
			wNewW = (unsigned)LOWORD(lParam);
			wNewH = (unsigned)HIWORD(lParam);
			if (wNewW != 0 && wNewH != 0) // Check of minimizing window case.
			{
				_mapView.ResizeWindow(wNewW, wNewH);
				InvalidateRect(m_hwnd, NULL, TRUE);
			}
			return 0;

		// Limit minimal window w/h.
		case WM_GETMINMAXINFO:
			lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = mvMinW;
			lpMMI->ptMinTrackSize.y = mvMinH;
			return 0;

		case WM_MOUSEMOVE:
			crsCurX = GET_X_LPARAM(lParam);
			crsCurY = GET_Y_LPARAM(lParam);
			if (wParam & MK_LBUTTON)
			{
				if (crsCurX != crsPrevX)
				{
					if (crsCurX < crsPrevX)
					{
						_mapView.MoveRight();
					}
					else
					{
						_mapView.MoveLeft();
					}
				}
				if (crsCurY != crsPrevY)
				{
					if (crsCurY < crsPrevY)
					{
						_mapView.MoveDown();
					}
					else
					{
						_mapView.MoveUp();
					}
				}
				InvalidateRect(m_hwnd, NULL, TRUE);
			}
			crsPrevX = crsCurX;
			crsPrevY = crsCurY;
			return 0;

		case WM_MOUSEWHEEL:
			mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouseWheelActions = std::abs(mouseWheelDelta / mouseWheelUnitsPerAction);
			// Scroll toward, zoom in.
			if (mouseWheelDelta > 0)
			{
				for (int i = 0; i < mouseWheelActions; ++i)
				{
					_mapView.ZoomIn();
				}
			}
			// Scroll backward, zoom out.
			else
			{
				for (int i = 0; i < mouseWheelActions; ++i)
				{
					_mapView.ZoomOut();
				}
			}
			InvalidateRect(m_hwnd, NULL, TRUE);
			return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			// Layer panel menu item pressed.
			case ID_LAYER_PANEL_ITEM:
				ShowWindow(_layerPanelWindow.Window(), SW_SHOW);
				return 0;
			}

		case WM_CLOSE:
			DestroyWindow(_layerPanelWindow.Window());
			return DefWindowProc(m_hwnd, uMsg, wParam, lParam);

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		}

		return TRUE;
	}

	void MapViewWindow::HandleKeyDown(const MSG& msg) noexcept
	{
		const char key = MapVirtualKey(static_cast<UINT>(msg.wParam), MAPVK_VK_TO_CHAR);
		switch (key)
		{
		case 'P':
			_mapView.ZoomIn();
			InvalidateRect(m_hwnd, NULL, TRUE);
			break;
		case 'M':
			_mapView.ZoomOut();
			InvalidateRect(m_hwnd, NULL, TRUE);
			break;
		case 'W':
			_mapView.MoveUp();
			InvalidateRect(m_hwnd, NULL, TRUE);
			break;
		case 'A':
			_mapView.MoveLeft();
			InvalidateRect(m_hwnd, NULL, TRUE);
			break;
		case 'S':
			_mapView.MoveDown();
			InvalidateRect(m_hwnd, NULL, TRUE);
			break;
		case 'D':
			_mapView.MoveRight();
			InvalidateRect(m_hwnd, NULL, TRUE);
			break;
		}
	}
}