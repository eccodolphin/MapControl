#pragma once

#define _WIN32_WINDOWS 0x0601
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include <exception>
#include <mutex>
#include <deque>

#include "boost/asio.hpp"
#include "lodepng.h"

#include "TileStructures.hpp"
#include "ITileCache.hpp"

namespace MapControl
{
	class TileRequester
	{
	// Constants section.
	private:
		const std::string tileServerName = "a.tile.openstreetmap.fr";
		const std::string tileServerProtocol = "http";
		const std::string validHttpVersionPrefix = "HTTP/";
		const unsigned successStatusCode = 200;

		const unsigned requestsDequeLimit = 10;


	// Creation section.
	public:
		TileRequester(std::mutex& tileServicesMutex);
		void SetPTileCache(ITileCache* pTileCache);
	private:
		void _EstablishServerConnection();

	// Requesting section.
	public:
		void PushTileRequest(const TileKey& tileKey);
		void ProcessTileRequest();
	private:
		void _RefreshConnection();
		void _MakeRequest(std::string requestString);
		void _ProcessResponseStatus(boost::asio::streambuf& responseBuf);
		void _ProcessResponseHeaders(boost::asio::streambuf& responseBuf, std::stringstream& imageBuf);
		void _ReadResponseData(boost::asio::streambuf& responseBuf, std::stringstream& imageBuf);

	// Requesting control section.
	public:
		void StartRequesting() noexcept;
		void StopRequesting() noexcept;
		bool IsRequesting() noexcept;

	// Destroying section.
	public:
		~TileRequester();

	// State cection.
	private:
		std::deque<TileKey> _requestsDeque;
		std::set<TileKey>	_requestsSet;

		bool _requesting;

		std::mutex&						_tileServicesMutex;
		boost::asio::io_service			_serverService;
		boost::asio::ip::tcp::socket	_serverSocket;
		boost::asio::ip::tcp::endpoint	_serverEndpoint;

		ITileCache* _pTileCache;
	};



	// Exceptions section.
	class SocketNotOpened : public std::exception
	{
	};
	class InvalidPng : public std::exception
	{
	};
	class ConnectionFailed : public std::exception
	{
	};
	class InvalidHttpPrefix : public std::exception
	{
	};
	class FailStatusCode : public std::exception
	{
	};
	class FailedToReadTileImageData : public std::exception
	{
	};
	class InvalidTileKey : public std::exception
	{
	};

	// Helpers section.
	bool IsValid(const TileKey& tileKey);
	std::string TileKeyToRequestString(const TileKey& tileKey);
	std::vector<unsigned char> StringToVector(const std::string& s);
	std::vector<unsigned char> DecodePng(const std::vector<unsigned char>& png);
}
