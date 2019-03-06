#pragma once
#include<fstream>
#include<string>
#include"Netlist.h"
#include"Region.h"

class FileReader
{
public:
	Region* RG;
	double W;
	double H;
	void Read(const std::string& filename, NetList& NL)
	{
		std::ifstream file(filename);
		std::string str;
		while (std::getline(file, str))
			Process(str, NL);
		NL.GenCon();
		for (size_t i = 0; i < NL.modulelist.size(); i++)
		{
			//if(!(NL.modulelist.begin() + i)->fixed)
				RG->Add(NL.modulelist.begin() + i);
		}
		RG->alive = true;
		RG->vertical = true;
	}

	void Process(const std::string& prcs, NetList& NL)
	{
		if (SetNet(prcs))
			return;
		if (SetModule(prcs, NL))
			return;
		if (GetConf("Num_Cells", prcs, cn))
			return;
		if (GetConf("Num_Mcells", prcs, mn))
			return;
		if (GetConf("Num_Nets", prcs, nn))
		{
			NL.setsize(cn, nn);
			NL.moveable = mn;
			return;
		}
		if (GetRConf("W", prcs, W))
			return;
		if (GetRConf("H", prcs, H))
		{
			RG = new Region(W, H);
			return;
		}
		

	}

	bool GetConf(const std::string& token, const std::string& tar, int& conf)
	{
		if (tar.find(token) == tar.npos)
			return false;
		std::size_t pos = tar.find("=");
		conf = std::stoi(tar.substr(pos + 1));
		return true;
	}

	bool GetRConf(const std::string& token, const std::string& tar, double& conf)
	{
		if (tar.find(token) == tar.npos)
			return false;
		std::size_t pos = tar.find("=");
		conf = std::stod(tar.substr(pos + 1));
		return true;
	}

	bool SetNet(const std::string& tar)
	{
		if (tar.find("N(") == tar.npos)
			return false;
		std::size_t s = tar.find("(")+1;
		std::size_t e = tar.find(")")-1;
		curr_n = std::stoi(tar.substr(s, e))-1;
		return true;
	}

	bool SetModule(const std::string& tar, NetList& NL)
	{
		if (tar.find("C(") == tar.npos)
			return false;
		double w, h, px, py, cx, cy;
		std::size_t s = tar.find("(") + 1;
		std::size_t e = tar.find_first_of(")") - 1;
		unsigned index = std::stoi(tar.substr(s, e));
		std::size_t coma;
		std::string remain = tar.substr(e + 2);
		s = remain.find_first_of("(") + 1;
		e = remain.find_first_of(")") - 1;
		coma = remain.find_first_of(",");
		w = std::stod(remain.substr(s, coma - 1));
		h = std::stod(remain.substr(coma + 1, e));
		remain = remain.substr(e + 2);
		s = remain.find_first_of("(") + 1;
		e = remain.find_first_of(")") - 1;
		coma = remain.find_first_of(",");
		px = std::stod(remain.substr(s, coma - 1));
		py = std::stod(remain.substr(coma + 1, e));
		if (remain.find("F(") == remain.npos)
		{
			NL.InitModule(index, w, h);
			NL.InitNet(curr_n, index, px, py);
			return true;
		}
		remain = remain.substr(e + 3);
		s = remain.find_first_of("(") + 1;
		e = remain.find_first_of(")") - 1;
		coma = remain.find_first_of(",");
		cx = std::stod(remain.substr(s, coma - 1));
		cy = std::stod(remain.substr(coma + 1, e));
		NL.InitModule(index, w, h, cx, cy, true);
		NL.InitNet(curr_n, index, px, py);
		return true;
	}


private:
	unsigned curr_n;
	int cn;
	int nn;
	int mn;

};