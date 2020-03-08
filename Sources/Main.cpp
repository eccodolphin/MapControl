#define _WIN32_WINDOWS 0x0601
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>

#include <mutex>
#include <thread>
#include <exception>
#include <chrono>

#include "TileRequester.hpp"
#include "MapModel.hpp"
#include "MapViewWindow.hpp"

using namespace MapControl;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmsLine, int nCmdShow)
{
	// Initialize map model.
	std::mutex tileServicesMutex;
	TileRequester tileRequester(tileServicesMutex);
	MapModel mapModel(tileRequester, tileServicesMutex);

	try
	{
		// Initialize map view/controller.
		MapViewWindow mapViewWindow(mapModel);
		if (!mapViewWindow.Create(L"Map View", WS_OVERLAPPEDWINDOW, mvDefW, mvDefH, 0, 0))
		{
			return 0;
		}
		mapViewWindow.GiveHwndToMapView();
		ShowWindow(mapViewWindow.Window(), nCmdShow);


		// Initialize tile requesting thread.
		auto requestingProcess = [&tileRequester, &tileServicesMutex]()
		{
			while (tileRequester.IsRequesting())
			{
				tileRequester.ProcessTileRequest();
				//std::this_thread::yield();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		};
		auto requestingThread = std::async(requestingProcess);


		// Initialize window message loop.
		MSG msg = {};
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_KEYDOWN)
			{
				mapViewWindow.HandleKeyDown(msg);
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//std::this_thread::yield();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		tileRequester.StopRequesting();
		return 0;
	}
	catch (std::exception e)
	{
		tileRequester.StopRequesting();
		return 1;
	}
}
