#include "Geometry.hpp"

namespace MapControl
{
	bool SafeEqual(const double& d1, const double& d2)
	{
		return std::abs(d1 - d2) < verySmallValue;
	}

	bool SafeLessOrEqual(const double& d1, const double& d2)
	{
		if (d1 < d2 || SafeEqual(d1, d2))
		{
			return true;
		}
		return false;
	}

	bool SafeMoreOrEqual(const double& d1, const double& d2)
	{
		if (d1 > d2 || SafeEqual(d1, d2))
		{
			return true;
		}
		return false;
	}



	bool IsEqual(const PointWM& wmPoint1, const PointWM& wmPoint2)
	{
		return (
			SafeEqual(wmPoint1.lat, wmPoint2.lat) &&
			SafeEqual(wmPoint1.lon, wmPoint2.lon)
			);
	}



	void Swap(PointWM& wmPoint1, PointWM& wmPoint2)
	{
		PointWM wmPointTemp = wmPoint1;
		wmPoint1 = wmPoint2;
		wmPoint2 = wmPointTemp;
	}



	bool IsVertical(const SectWM& wmSection)
	{
		return SafeEqual(wmSection.p1.lon, wmSection.p2.lon);
	}

	bool IsVertical(const LineWM& wmLine)
	{
		return SafeEqual(wmLine.b, 0.0);
	}

	bool IsHorizontal(const SectWM& wmSection)
	{
		return SafeEqual(wmSection.p1.lat, wmSection.p2.lat);
	}

	bool IsHorizontal(const LineWM& wmLine)
	{
		return SafeEqual(wmLine.a, 0.0);
	}



	double Slope(const SectWM& wmSection)
	{
		return (wmSection.p2.lat - wmSection.p1.lat) / (wmSection.p2.lon - wmSection.p1.lon);
	}

	LineWM SectionToLine(const SectWM& wmSection)
	{
		double a = wmSection.p1.lat - wmSection.p2.lat;
		double b = wmSection.p2.lon - wmSection.p1.lon;
		double c = (
			wmSection.p2.lat * wmSection.p1.lon -
			wmSection.p1.lat * wmSection.p2.lon
			);
		return LineWM{ a, b, c };
	}

	std::vector<SectWM> RectToSects(const RectWM& wmRect)
	{
		std::vector<SectWM> wmRectSects;

		wmRectSects.push_back(SectWM{
			PointWM{ wmRect.origin.lat, wmRect.origin.lon },
			PointWM{ wmRect.origin.lat, wmRect.extrem.lon }
			});

		wmRectSects.push_back(SectWM{
			PointWM{ wmRect.origin.lat, wmRect.extrem.lon },
			PointWM{ wmRect.extrem.lat, wmRect.extrem.lon }
		});

		wmRectSects.push_back(SectWM{
			PointWM{ wmRect.extrem.lat, wmRect.extrem.lon },
			PointWM{ wmRect.extrem.lat, wmRect.origin.lon }
		});

		wmRectSects.push_back(SectWM{
			PointWM{ wmRect.extrem.lat, wmRect.origin.lon },
			PointWM{ wmRect.origin.lat, wmRect.origin.lon }
		});

		return wmRectSects;
	}



	bool AreParallel(const SectWM& wmSection1, const SectWM& wmSection2)
	{
		return SafeEqual(Slope(wmSection1), Slope(wmSection2));
	}

	bool AreParallel(const LineWM& wmLine1, const LineWM& wmLine2)
	{
		return SafeEqual(wmLine1.a, wmLine2.a) && SafeEqual(wmLine1.b, wmLine2.b);
	}



	bool IsLinePoint(const PointWM& wmPoint, const LineWM& wmLine)
	{
		return SafeEqual(wmLine.a * wmPoint.lon + wmLine.b * wmPoint.lat + wmLine.c, 0.0);
	}

	bool IsSectionPoint(PointWM& wmPoint, SectWM& wmSection)
	{
		if (IsLinePoint(wmPoint, SectionToLine(wmSection)))
		{
			if (IsVertical(wmSection))
			{
				if (wmSection.p1.lat > wmSection.p2.lat)
				{
					Swap(wmSection.p1, wmSection.p2);
				}
				return (
					SafeLessOrEqual(wmSection.p1.lat, wmPoint.lat) &&
					SafeLessOrEqual(wmPoint.lat, wmSection.p2.lat)
					);
			}
			else
			{
				if (wmSection.p1.lon > wmSection.p2.lon)
				{
					Swap(wmSection.p1, wmSection.p2);
				}
				return (
					SafeLessOrEqual(wmSection.p1.lon, wmPoint.lon) &&
					SafeLessOrEqual(wmPoint.lon, wmSection.p2.lon)
					);
			}
		}
		return false;
	}



	PointWM LineInterseñtionPoint(const LineWM& wmLine1, const LineWM& wmLine2)
	{
		if (AreParallel(wmLine1, wmLine2))
		{
			throw ParallelLines();
		}

		PointWM wmLineIntersectionPoint;
		wmLineIntersectionPoint.lat = (
			(wmLine1.a * wmLine2.c - wmLine2.a * wmLine1.c) /
			(wmLine2.a * wmLine1.b - wmLine1.a * wmLine2.b)
			);
		wmLineIntersectionPoint.lon = (
			(wmLine2.b * wmLine1.c - wmLine1.b * wmLine2.c) /
			(wmLine2.a * wmLine1.b - wmLine1.a * wmLine2.b)
			);
		return wmLineIntersectionPoint;
	}

	PointWM SectsInterseñtPoint(SectWM& wmSection1, SectWM& wmSection2)
	{
		try
		{
			PointWM wmLineIntersectionPoint = LineInterseñtionPoint(
				SectionToLine(wmSection1), SectionToLine(wmSection2)
				);
			
			if (IsSectionPoint(wmLineIntersectionPoint, wmSection1) &&
				IsSectionPoint(wmLineIntersectionPoint, wmSection2)
				)
			{
				return wmLineIntersectionPoint;
			}
			throw NoSectionIntersectionPoint();
		}
		catch (ParallelLines)
		{
			throw ParallelSections();
		}
	}
	

	
	bool IsVisible(const PointWM& wmPoint, const RectWM& wmRect)
	{
		return (
			SafeLessOrEqual(wmRect.origin.lat, wmPoint.lat)	&& 
			SafeLessOrEqual(wmRect.origin.lon, wmPoint.lon)	&& 
			SafeLessOrEqual(wmPoint.lat, wmRect.extrem.lat) &&
			SafeLessOrEqual(wmPoint.lon, wmRect.extrem.lon)
			);
	}

	bool IsVisible(SectWM& wmSection, const RectWM& wmRect)
	{
		if (IsFullyVisible(wmSection, wmRect))
		{
			return true;
		}
		if (IsFullyInvisible(wmSection, wmRect))
		{
			return false;
		}

		if (IsVisible(wmSection.p1, wmRect) || IsVisible(wmSection.p2, wmRect))
		{
			return true;
		}

		std::vector<SectWM> wmRectSects = RectToSects(wmRect);
		for (SectWM& wmRectSect : wmRectSects)
		{
			try
			{
				SectsInterseñtPoint(wmRectSect, wmSection);
				return true;
			}
			catch (ParallelSections)
			{

			}
			catch (NoSectionIntersectionPoint)
			{

			}
		}
		return false;
	}

	bool IsFullyVisible(const SectWM& wmSection, const RectWM& wmRect)
	{
		return IsVisible(wmSection.p1, wmRect) && IsVisible(wmSection.p2, wmRect);
	}

	bool IsFullyInvisible(const SectWM& wmSection, const RectWM& wmRect)
	{
		return (
			wmSection.p1.lat < wmRect.origin.lat && wmSection.p2.lat < wmRect.origin.lat ||
			wmSection.p1.lon < wmRect.origin.lon && wmSection.p2.lon < wmRect.origin.lon ||
			wmSection.p1.lat > wmRect.extrem.lat && wmSection.p2.lat > wmRect.extrem.lat ||
			wmSection.p1.lon > wmRect.extrem.lon && wmSection.p2.lon > wmRect.extrem.lon
			);
	}



	SectWM GetVisiblePeace(SectWM& wmSect, const RectWM& wmRect)
	{
		if (!IsVisible(wmSect, wmRect))
		{
			throw InvisibleSection();
		}
		if (IsFullyVisible(wmSect, wmRect))
		{
			return wmSect;
		}

		std::vector<SectWM> wmRectSects = RectToSects(wmRect);

		if (IsVisible(wmSect.p1, wmRect) || IsVisible(wmSect.p2, wmRect))
		{
			PointWM wmIntersectPoint;
			for (SectWM& wmRectSect : wmRectSects)
			{
				try
				{
					wmIntersectPoint = SectsInterseñtPoint(wmRectSect, wmSect);
					break;
				}
				catch (ParallelSections) 
				{
				}
				catch (NoSectionIntersectionPoint) 
				{
				}
			}

			if (IsVisible(wmSect.p1, wmRect))
			{
				return SectWM{ wmSect.p1, wmIntersectPoint };
			}
			else
			{
				return SectWM{ wmSect.p2, wmIntersectPoint };
			}
		}

		std::deque<PointWM> wmIntersectPoints;
		for (SectWM& wmRectSect : wmRectSects)
		{
			try
			{
				wmIntersectPoints.push_back(SectsInterseñtPoint(wmRectSect, wmSect));
			}
			catch (ParallelSections) 
			{
			}

			catch (NoSectionIntersectionPoint)
			{
			}
		}

		if (wmIntersectPoints.size() == 1)
		{
			PointWM wmTouchPoint = wmIntersectPoints.front();
			return SectWM{ wmTouchPoint, wmTouchPoint };
		}

		// Works in all remaining cases, side-side, sidel-corner and corner-corner penetration.
		PointWM wmIntersectPoint1 = wmIntersectPoints.front();
		wmIntersectPoints.pop_front();
		while (IsEqual(wmIntersectPoints.front(), wmIntersectPoint1))
		{
			wmIntersectPoints.pop_front();
		}
		PointWM wmIntersectPoint2 = wmIntersectPoints.front();
		return SectWM{ wmIntersectPoint1, wmIntersectPoint2 };
	}



	bool IsValid(PointWM wmPoint)
	{
		if (wmPoint.lat < wmMinLat || wmMaxLat < wmPoint.lat)
		{
			return false;
		}
		if (wmPoint.lon < wmMinLon || wmMaxLon < wmPoint.lon)
		{
			return false;
		}
		return true;
	}
}
