#pragma once
#include "FileReader.h"
#include "MatrixBuilder.h"
#include "BMP.h"

class GORDIAN
{
public:
	double w, h;
	GORDIAN() { kj = 3; }
	void SetK(int l) { kj = l; }
	void Start(const std::string& file)
	{
		Init(file);
		int k = 0;
		std::string base = file.substr(0, file.find(".") - 1);
		while (NL.modulelist.size() >= kj*regions.size())
		{
			Drawer DR;
			DR.Set(w, h);
			if (regions.size() == NL.moveable)
			{
				break;
			}
			Solve();
			for each (Module m in NL.modulelist)
				DR.DrawBox(m);
			DR.Save(base + std::to_string(k) + "box.bmp");
			for each (Net n in NL.netlist)
				DR.DrawNet(n);
			DR.Save(base + std::to_string(k) + "full.bmp");
			k++;
		}
		Drawer DR;
		DR.Plot(loi, maxl);
		DR.Save(base + "iter.bmp");
	}
private:
	void Init(const std::string& file)
	{
		FR.Read(file, NL);
		w = FR.W;
		h = FR.H;
		regions.push_back(FR.RG);
		CoM.Resize(NL.moveable);
		CoM.Build(NL);
		//std::cout << CoM.C << std::endl;
		maxl = 0.0;
	}

	void Solve()
	{
		ConstraintMatrices CrM(regions.size(), NL.moveable);
		CrM.Build(regions, NL);
		//std::cout <<'\n'<< CrM.A.block(0,0, regions.size(), 16) << std::endl;
		MatrixSolver MS;
		MS.Solve(CoM, CrM);
		std::vector<double> solx = MS.solx;
		std::vector<double> soly = MS.soly;
		for each (auto p in NL.conversion)
			NL.SetModule(p.first, solx[p.second], soly[p.second]);
		std::vector<Region*> regions2;
		std::vector<Region*> regionsf;
		for each (auto r in regions)
		{
			if (r->modules.size() > 1)
			{
				regions2.push_back(r->Cut());
			}
			if (r->alive)
				regionsf.push_back(r);
		}
		for each (auto r in regions2)
		{
			if (r->alive)
				regionsf.push_back(r);
		}
		regions = regionsf;
		double t = NL.GetLength();
		loi.push_back(t);
		if (t > maxl)
			maxl = t;
	}

	int kj;
	FileReader FR;
	NetList NL;
	CoefficientMatrices CoM;
	std::vector<Region*> regions;
	std::vector<double> loi;
	double maxl;
};