#pragma once
#include<vector>
#include<utility>
#include<set>
#include<map>
#include "Point.h"
#include "Net.h"
class NetList
{
public:
	NetList(){}

	void setsize(size_t msize, size_t nsize)
	{
		modulelist.resize(msize);
		netlist.resize(nsize);
	}

	void InitModule(unsigned index, double x, double y, double cx = 0.0, double cy = 0.0, bool fx = false)
	{
		modulelist[index].Index = index;
		modulelist[index].Central = Point(cx, cy);
		modulelist[index].Length = Point(x, y);
		modulelist[index].fixed = fx;
	}

	void SetModule(unsigned index, double cx, double cy)
	{
		modulelist[index].Central = Point(cx, cy);
	}

	void InitNet(unsigned index, unsigned module, double px, double py)
	{
		netlist[index].AddPin(modulelist.begin() + module, px, py);
	}


	double GetLength()
	{
		double l = 0.0;
		for (size_t i = 0; i < netlist.size(); i++)
		{
			netlist[i].UpdateCentral();
			l = l + netlist[i].GetLength();
		}
		return l;
	}

	void GenCon()
	{
		unsigned id = 0;
		for each(Module M in modulelist)
		{
			if (!M.fixed)
			{
				conversion.insert({ M.Index, id });
				id++;
			}
		}
	}

	std::vector<Module> modulelist;
	std::vector<Net> netlist;
	std::map<unsigned, unsigned> conversion;
	int moveable;
};

