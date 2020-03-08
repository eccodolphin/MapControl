#include "TileRequester.hpp"

namespace MapControl
{
	TileRequester::TileRequester(std::mutex& tileServicesMutex) :
		_serverService(),
		_serverSocket(_serverService),
		_tileServicesMutex(tileServicesMutex)
	{
		try
		{
			_EstablishServerConnection();
		}
		catch (ConnectionFailed e)
		{
			throw e;
		}
	}

	void TileRequester::SetPTileCache(ITileCache* pTileCache)
	{
		_pTileCache = pTileCache;
	}

	void TileRequester::_EstablishServerConnection()
	{
		using namespace boost::asio::ip;

		// Itterate through endpoints until conection isn't established.

		tcp::resolver resolver(_serverService);
		tcp::resolver::query query(tileServerName, tileServerProtocol);

		tcp::resolver::iterator endpointIterator;
		try
		{
			endpointIterator = resolver.resolve(query); // Throws when no connection.
		}
		catch (...)
		{
			throw ConnectionFailed();
		}

		tcp::resolver::iterator end;
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpointIterator != end)
		{
			_serverSocket.close();
			_serverSocket.connect(*endpointIterator, error);
			_serverEndpoint = *endpointIterator++;
		}

		if (error)
		{
			throw ConnectionFailed();
		}
	}


	
	// Requesting section.

	void TileRequester::PushTileRequest(const TileKey& tileKey)
	{
		if (_requestsSet.find(tileKey) == _requestsSet.end())
		{
			_requestsSet.insert(tileKey);
			_requestsDeque.push_front(tileKey);

			if (_requestsDeque.size() > requestsDequeLimit)
			{
				_requestsSet.erase(_requestsDeque.back());
				_requestsDeque.pop_back();
			}
		}
	}

	void TileRequester::ProcessTileRequest()
	{
		std::unique_lock<std::mutex> tileServicesLock(_tileServicesMutex);

		if (!_serverSocket.is_open())
		{
			throw SocketNotOpened();
		}
		if (_requestsDeque.empty())
		{
			return;
		}

		TileKey tileKey = _requestsDeque.front();
		_requestsDeque.pop_front();
		_requestsSet.erase(tileKey);

		// Initialize buffers.
		boost::asio::streambuf responseBuf;
		std::stringstream imageBuf;

		// Get tile image from server.
		_RefreshConnection();
		_MakeRequest(TileKeyToRequestString(tileKey));
		_ProcessResponseStatus(responseBuf);
		_ProcessResponseHeaders(responseBuf, imageBuf);
		_ReadResponseData(responseBuf, imageBuf);

		std::vector<unsigned char> tileImage = StringToVector(imageBuf.str());

		TileSlot* pTileSlot = _pTileCache->GetPDirtySlot();
		pTileSlot->status = TileStatus::InProgress;

		// Update slot.
		pTileSlot->image		= DecodePng(tileImage);
		pTileSlot->key			= tileKey;
		pTileSlot->status		= TileStatus::Clean;
		pTileSlot->timestamp	= std::chrono::milliseconds(0);

		_pTileCache->AddPCleanSlot(pTileSlot);
	}

	void TileRequester::_RefreshConnection()
	{
		static boost::system::error_code error;

		_serverSocket.close();
		_serverSocket.connect(_serverEndpoint, error);

		if (error)
		{
			throw ConnectionFailed();
		}
	}

	void TileRequester::_MakeRequest(std::string requestString)
	{
		boost::asio::streambuf request;
		std::ostream requestStream(&request);

		// Form tile request.
		requestStream << "GET " << requestString << " HTTP/1.0" << "\r\n";
		requestStream << "Host: " << tileServerName << "\r\n";
		requestStream << "Accept: " << "*/*" << "\r\n";
		requestStream << "Connection: " << "close" << "\r\n";
		requestStream << "\r\n";

		boost::asio::write(_serverSocket, request);
	}

	void TileRequester::_ProcessResponseStatus(boost::asio::streambuf& responseBuf)
	{
		static std::string httpVersion;
		static unsigned statusCode;
		static std::string statusMessage;

		// Read response info from socket.
		boost::asio::read_until(_serverSocket, responseBuf, "\r\n");
		std::istream responseStream(&responseBuf);
		responseStream >> httpVersion;
		responseStream >> statusCode;
		std::getline(responseStream, statusMessage);

		// Check response info.
		std::string httpVersionPrefix = httpVersion.substr(0, 5);
		if (httpVersionPrefix != validHttpVersionPrefix)
		{
			throw InvalidHttpPrefix();
		}
		if (statusCode != successStatusCode)
		{
			throw FailStatusCode();
		}
	}

	// Read headers without processing.
	void TileRequester::_ProcessResponseHeaders(
		boost::asio::streambuf& responseBuf,
		std::stringstream& imageDataBuf
		)
	{
		boost::asio::read_until(_serverSocket, responseBuf, "\r\n\r\n");
		std::istream responseBufStream(&responseBuf);

		std::string header;
		while (std::getline(responseBufStream, header) && header != "\r")
		{
		}

		// Write allready readed tile image data.
		if (responseBuf.size() > 0)
		{
			imageDataBuf << &responseBuf;
		}
	}

	// Read tile image data.
	void TileRequester::_ReadResponseData(
		boost::asio::streambuf& responseBuf,
		std::stringstream& imageDataBuf
		)
	{
		using namespace boost::asio;
		boost::system::error_code error;
		while (read(_serverSocket, responseBuf, transfer_at_least(1), error))
		{
			imageDataBuf << &responseBuf;
		}
	}



	// Requesting control section.
	
	void TileRequester::StartRequesting() noexcept
	{
		_requesting = true;
	}

	void TileRequester::StopRequesting() noexcept
	{
		_requesting = false;
	}

	bool TileRequester::IsRequesting() noexcept
	{
		return _requesting;
	}



	// Destroying section.
	
	TileRequester::~TileRequester()
	{
		_serverSocket.close();
	}


	// Helpers section.

	bool IsValid(const TileKey& tileKey)
	{
		if (tileKey.lod < wmMinScaleLevel || wmMaxScaleLevel < tileKey.lod)
		{
			return false;
		}

		static const unsigned minXY = 0;
		const unsigned maxXY = SafePow(2, tileKey.lod) - 1;
		if (tileKey.x < minXY || tileKey.y < minXY || tileKey.x > maxXY || tileKey.y > maxXY)
		{
			return false;
		}

		return true;
	}

	std::string TileKeyToRequestString(const MapControl::TileKey& tileKey)
	{
		if (!IsValid(tileKey))
		{
			throw InvalidTileKey();
		}

		std::string requestString;

		requestString += "/hot";
		requestString += "/" + std::to_string(tileKey.lod);
		requestString += "/" + std::to_string(tileKey.x);
		requestString += "/" + std::to_string(tileKey.y);
		requestString += ".png";

		return requestString;
	}

	std::vector<unsigned char> StringToVector(const std::string& s)
	{
		std::vector<unsigned char> v;
		for (const unsigned char& c : s)
		{
			v.push_back(c);
		}
		return v;
	}

	std::vector<unsigned char> DecodePng(const std::vector<unsigned char>& png)
	{
		std::vector<unsigned char> pixels;
		unsigned w, h;
		unsigned error = lodepng::decode(pixels, w, h, png);
		if (error)
		{
			throw InvalidPng();
		}
		return pixels;
	}
}
