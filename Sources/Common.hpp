#pragma once

#include <chrono>
#include <exception>

// Web Mercator parametrs.

const double wmMinLat = -85.051129;
const double wmMaxLat = 85.051129;
const double wmMinLon = -180.0;
const double wmMaxLon = 180.0;

const double wmFullWDgs = wmMaxLon - wmMinLon;
const double wmFullHDgs = wmMaxLat - wmMinLat;

const unsigned wmMinScaleLevel = 0;
const unsigned wmMaxScaleLevel = 19;



// Tiles parametrs.

const unsigned wmTileWPix = 256;
const unsigned wmTileHPix = 256;



// Bitmap drawing parametrs.

const unsigned bmpDefPlanes = 1;
const unsigned bmpDefBitCount = 32;

const unsigned pixelComponentsQuantity = 4;



// Helper functions.

int SafePow(int number, unsigned pow) noexcept;
std::chrono::milliseconds GetCurrentTimestamp() noexcept;
