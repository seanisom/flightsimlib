#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

namespace flightsimlib::geo
{

class WebMercator
{
private:
	static constexpr double EarthRadius = 6378137;
	static constexpr double MinLatitude = -85.05112878;
	static constexpr double MaxLatitude = 85.05112878;
	static constexpr double MinLongitude = -180;
	static constexpr double MaxLongitude = 180;
	static constexpr double pi = 3.1415926535897;

public:
	static double Clip(double n, double minValue, double maxValue)
	{
		return std::min(std::max(n, minValue), maxValue);
	}

	static unsigned int MapSize(int levelOfDetail)
	{
		return (unsigned int) 256 << levelOfDetail;
	}

	static double GroundResolution(double latitude, int levelOfDetail)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		return cos(latitude * pi / 180) * 2 * pi * EarthRadius / MapSize(levelOfDetail);
	}

	static double MapScale(double latitude, int levelOfDetail, int screenDpi)
	{
		return GroundResolution(latitude, levelOfDetail) * screenDpi / 0.0254;
	}

	static void LatLongToPixelXY(double latitude, double longitude, int levelOfDetail, int& pixelX,
		int& pixelY)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		longitude = Clip(longitude, MinLongitude, MaxLongitude);

		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * pi / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * pi);

		unsigned int mapSize = MapSize(levelOfDetail);
		pixelX = (int)Clip(x * mapSize + 0.5, 0, mapSize - 1);
		pixelY = (int)Clip(y * mapSize + 0.5, 0, mapSize - 1);
	}

	static void PixelXYToLatLong(int pixelX, int pixelY, int levelOfDetail, double& latitude,
		double& longitude)
	{
		double mapSize = MapSize(levelOfDetail);
		double x = (Clip(pixelX, 0, mapSize - 1) / mapSize) - 0.5;
		double y = 0.5 - (Clip(pixelY, 0, mapSize - 1) / mapSize);

		latitude = 90 - 360 * atan(exp(-y * 2 * pi)) / pi;
		longitude = 360 * x;
	}

	static void PixelXYToTileXY(int pixelX, int pixelY, int& tileX, int& tileY)
	{
		tileX = pixelX / 256;
		tileY = pixelY / 256;
	}

	static void TileXYToPixelXY(int tileX, int tileY, int& pixelX, int& pixelY)
	{
		pixelX = tileX * 256;
		pixelY = tileY * 256;
	}
 
	static std::string TileXYToQuadKey(int tileX, int tileY, int levelOfDetail)
	{
		std::stringstream quadKey;
		for (int i = levelOfDetail; i > 0; i--)
		{
			char digit = '0';
			int mask = 1 << (i - 1);
			if (tileX & mask)
			{
				digit++;
			}
			if (tileY & mask)
			{
				digit++;
				digit++;
			}
			quadKey << digit;
		}
		return quadKey.str();
	}

	static void QuadKeyToTileXY(std::string quadKey, unsigned int& tileX, unsigned int& tileY, int& levelOfDetail)
	{
		tileX = tileY = 0;
		levelOfDetail = static_cast<int>(quadKey.size());
		for (int i = levelOfDetail; i > 0; i--)
		{
			const auto mask = 1 << (i - 1);
			switch (quadKey[levelOfDetail - i])
			{
			case '0':
				break;

			case '1':
				tileX |= mask;
				break;

			case '2':
				tileY |= mask;
				break;

			case '3':
				tileX |= mask;
				tileY |= mask;
				break;

			default:
				levelOfDetail = 0;
				return;
			}
		}
	}
};

}