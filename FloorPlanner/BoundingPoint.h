#pragma once
#include <stdio.h>
#include <vector>
#include <list>
#include <iterator>

class Point
{
public:
	friend bool operator<=(const Point& P1, const Point& P2)
	{
		if (P1.x <= P2.x && P1.y <= P2.y)
			return true;
		return false;
	}

	double x;
	double y;
};

class BoundingPoint
{
public:
	void SetId(const size_t fid)
	{
		id = fid;
	}
	BoundingPoint(){}
	friend std::vector<BoundingPoint> operator*(const BoundingPoint & BP1, const BoundingPoint & BP2)
	{

	}

	friend std::vector<BoundingPoint> operator+(const BoundingPoint & BP1, const BoundingPoint & BP2)
	{

	}

	friend bool operator<=(const BoundingPoint& P1, const BoundingPoint& P2)
	{
		if (P1.single)
		{
			if (P2.single)
			{
				if (P1.vertex[0] <= P2.vertex[0])
					return true;
				return false;
			}
			if (P1.vertex[0].x <= P2.vertex[1].x && P1.vertex[0].y <= P2.vertex[1].y)
				return true;
			return false;
		}
		if (P2.single)
		{
			if (P1.vertex[1]<= P2.vertex[0] && P1.vertex[2] <= P2.vertex[0])
				return true;
			return false;
		}
		if (P1.vertex[1] <= P2.vertex[1] && P1.vertex[2] <= P2.vertex[2])
			return true;
		return false;
	}

private:
	std::vector<Point> vertex;//single point, 3 points for curve( i.e. (x-a)(y-b) = s, 0 a, b; 1, min x,y, 2, max, x,y )
	bool single;// true one point, false curve
	size_t id;
	size_t source1;
	size_t source2;
};

class BoundingCurve
{
public:
	BoundingCurve(){}

	BoundingCurve(double l, double u, bool fixed)
	{

	}

	friend BoundingCurve operator*(const BoundingCurve & BC1, const BoundingCurve & BC2)
	{
		BoundingCurve BQ;
		std::vector<BoundingPoint> temp;
		for each (auto t in BC2.points)
			for each (auto tp in BC1.points)
				temp = t * tp;
		for each (auto p in temp)
			BQ.addpoints(p);
		BQ.assignid();
		return BQ;
	}

	friend BoundingCurve operator+(const BoundingCurve & BC1, const BoundingCurve & BC2)
	{
		BoundingCurve BQ;
		std::vector<BoundingPoint> temp;
		for each (auto t in BC2.points)
			for each (auto tp in BC1.points)
				temp = t + tp;
		for each (auto p in temp)
			BQ.addpoints(p);
		BQ.assignid();
		return BQ;
	}

private:
	void addpoints(const BoundingPoint & BP)
	{
		if (points.empty())
		{
			points.push_back(BP);
			return;
		}
		bool inserted = false;
		for (std::list<BoundingPoint>::iterator p = points.begin; p != points.end; p++)
		{
			if (*p <= BP)
				return;
			if (BP <= *p)
			{
				points.erase(p);
				if (!inserted)
				{
					points.push_back(BP);
					inserted = true;
				}
			}
		}
	}
	void assignid()
	{
		for (std::list<BoundingPoint>::iterator p = points.begin; p != points.end; p++)
			p->SetId(p - points.begin);
	}
	std::list<BoundingPoint> points;
};