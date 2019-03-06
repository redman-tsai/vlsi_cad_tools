#pragma once
#include<vector>
#include<utility>
#include <algorithm>
#include<set>
#include "Point.h"
#include "Net.h"

class Region
{
	using module = std::vector<Module>::iterator;
public:

	Point ld;
	Point ru;
	bool vertical;
	bool alive;
	std::set<module> modules;

	Region(){}

	Region(double width, double height)
	{
		ld = Point();
		ru = Point(width, height);
	}

	void Add(module m)
	{
		modules.insert(m);
	}

	Point Central()
	{
		return (ld + ru) / 2;
	}

	Point Dim()
	{
		return (ru - ld);
	}

	double pivot()
	{
		std::vector<double> p;
		for each (module m in modules)
		{
			if (vertical)
				p.push_back((m->Central).x);
			else
				p.push_back((m->Central).y);
		}
		
		if (p.size() % 2 == 0)
		{
			std::nth_element(p.begin(), p.begin() + p.size() / 2 , p.end());
			return p[p.size() / 2 ];
		}
		std::nth_element(p.begin(), p.begin() + (p.size()+1) / 2 , p.end());
		return p[(p.size() + 1) / 2];
	}

	Region* Cut()
	{
		Point s, e;
		if (vertical)
		{
			s.x = (ld.x + ru.x) / 2;
			s.y = ld.y;
			e.x = s.x;
			e.y = ru.y;
		}
		else
		{
			s.y = (ld.y + ru.y) / 2;
			s.x = ld.x;
			e.y = s.y;
			e.x = ru.x;
		}
		Region* sub = new Region();
		sub->ld = s;
		sub->ru = ru;
		sub->vertical = !vertical;
		sub->alive = false;
		ru = e;
		double prev = pivot();
		if (vertical)
		{
			for each (module m in modules)
			{
				if ((m->Central.x < prev))
				{
					sub->Add(m);
					if (!m->fixed)
						sub->alive = true;
				}
			}
		}
					
		if (!vertical)
		{
			for each (module m in modules)
			{
				if ((m->Central.y < prev))
				{
					sub->Add(m);
					if (!m->fixed)
						sub->alive = true;
				}
			}
		}
		for each (module m in sub->modules)
			modules.erase(m);
		vertical = !vertical;
		alive = false;
		for each (module m in modules)
		{
			if (!m->fixed)
			{
				alive = true;
				break;
			}
		}
		return sub;
	}
};