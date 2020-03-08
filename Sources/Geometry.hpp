#pragma once

#include <Windows.h>

#include <cmath>
#include <cassert>
#include <exception>
#include <vector>
#include <deque>

#include "Common.hpp"

namespace MapControl
{
	// Integer primitives.

	struct PointInt
	{
		int x;
		int y;
	};

	struct SectInt
	{
		PointInt p1;
		PointInt p2;
	};

	struct RectInt
	{
		PointInt origin;
		unsigned w;
		unsigned h;
	};



	// Web Mercator primitives.

	struct PointWM
	{
		double lat;
		double lon;
	};

	struct LineWM
	{
		double a;
		double b;
		double c;
	};

	struct SectWM
	{
		PointWM p1;
		PointWM p2;
		COLORREF c;
	};

	struct RectWM
	{
		PointWM origin;
		PointWM extrem;
	};


	
	// Useful constant for safe compare operations with doubles.
	const double verySmallValue = 1e-6;



	// Exception classes.

	class ParallelLines : public std::exception 
	{
	};

	class ParallelSections : public std::exception 
	{
	};

	class NoSectionIntersectionPoint : public std::exception
	{
	};

	class InvisibleSection : public std::exception 
	{
	};
	
	

	// Safe compare functions for doubles.
	
	bool SafeEqual(const double& d1, const double& d2);
	bool SafeLessOrEqual(const double& d1, const double& d2);
	bool SafeMoreOrEqual(const double& d1, const double& d2);



	// Geometry library for Web Mercator primitives.

	bool IsValid(PointWM wmPoint);
	void Swap(PointWM& wmPoint1, PointWM& wmPoint2);
	bool IsEqual(const PointWM& wmPoint1, const PointWM& wmPoint2);
	bool IsLinePoint(const PointWM& wmPoint, const LineWM& wmLine);
	bool IsSectionPoint(PointWM& wmPoint, SectWM& wmSection);

	double Slope(const SectWM& wmSection);

	bool IsVertical(const SectWM& wmSection);
	bool IsVertical(const LineWM& wmLine);
	bool IsHorizontal(const SectWM& wmSection);
	bool IsHorizontal(const LineWM& wmLine);
	bool AreParallel(const SectWM& wmSection1, const SectWM& wmSection2);
	bool AreParallel(const LineWM& wmLine1, const LineWM& wmLine2);

	LineWM SectionToLine(const SectWM& wmSection);
	std::vector<SectWM> RectToSects(const RectWM& wmRect);

	PointWM LineInterseñtionPoint(const LineWM& wmLine1, const LineWM& wmLine2);
	PointWM SectsInterseñtPoint(SectWM& wmSection1, SectWM& wmSection2);

	bool IsVisible(const PointWM& wmPoint, const RectWM& wmRect);
	bool IsVisible(SectWM& wmSection, const RectWM& wmRect);
	bool IsFullyVisible(const SectWM& wmSection, const RectWM& wmRect);
	bool IsFullyInvisible(const SectWM& wmSection, const RectWM& wmRect);
	SectWM GetVisiblePeace(SectWM& wmSection, const RectWM& wmRect);
}
