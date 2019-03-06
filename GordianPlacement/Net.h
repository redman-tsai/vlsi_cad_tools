#pragma once
#include<vector>
#include<utility>
#include "Point.h"
class Module
{
public:
	Point Central;
	Point Length;
	unsigned Index;
	bool fixed;
};


class Net
{
	using module = std::vector<Module>::iterator;
	using Mpin = std::pair<module, Point>;
public:
	Point Central;
	std::vector<Mpin> ModuleChain;
	size_t movable;

	Net()
	{
		movable = 0;
	}

	void AddPin(module m, double px, double py)
	{
		Mpin M;
		M.first = m;
		M.second = Point(px, py);
		ModuleChain.push_back(M);
		if (!m->fixed)
			movable++;
	}

	double GetLength()
	{
		double l = 0.0;
		Point temp;
		for each(Mpin t in ModuleChain)
		{
			temp = t.first->Central+t.second-Central;
			l = l + temp.Norm();
		}
		return l/2;
	}

	void UpdateCentral()
	{
		Central.x = 0.0;
		Central.y = 0.0;
		for each(Mpin t in ModuleChain)
			Central = Central+ t.first->Central;
		Central = Central / ModuleChain.size();
	}
};



