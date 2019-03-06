#pragma once
#include<stdio.h>
#include<string>
#include<list>
#include<vector>
#include <fstream>
struct StickBridge
{
	std::pair<size_t, size_t> path;
	int layer;
	bool ipmos;
};



struct StickRail
{
	size_t pin;
	size_t rail;
	bool ipmos;
};

class StickLayout
{
public:
	StickLayout(const std::vector<char>& BN)
	{
		polyseq = BN;
		intervalcount = BN.size() + 1;
		for each (char b in BN)
		{
			if (b != '%')
			{
				polynames.push_back(b);
				continue;
			}
			intervalcount++;
		}
	}
	std::vector<char> GetSeq() { return polyseq; }
	std::vector<StickBridge> GetBridges() { return bridges; }
	std::vector<StickRail> GetRails() { return rails; }
	std::pair<size_t, size_t> GetORail() { return outrail; }

	void InitiateBridge(const std::vector<int>& porder, const std::vector<int>& norder)
	{
		outrail = { intervalcount - 1, 0 };
		layercount = 0;
		BridgeBuilder(porder, true);
		outrailnm = layercount;
		layercount++;
		totallayer.push_back(10000);
		BridgeBuilder(norder, false);
		FixOutRail();
	}

	size_t getlayercount() { return layercount; }
	size_t outrailnumber() { return outrailnm; }

private:
	void FixOutRail()
	{
		bool fixable = false;
		size_t prev = outrailnm;
		for (size_t i = 0; i < totallayer.size(); i++)
		{
			if (totallayer[i] < outrail.first)
			{
				bool nooverlap = true;
				for each(size_t np in noutrailpins)
				{
					for (size_t t = 0; t < bridges.size(); t++)
					{
						if ((bridges[t].path.first == np) || (bridges[t].path.second == np))
						{
							if (bridges[t].ipmos)
							{
								if (t > i)
								{
									nooverlap = false;
									break;
								}
							}
							else
							{
								nooverlap = false;
								break;
							}
						}
						
					}
					if (!nooverlap)
						break;
				}

				for each(size_t np in poutrailpins)
				{
					for (size_t t = 0; t < bridges.size(); t++)
					{
						if ((bridges[t].path.first == np) || (bridges[t].path.second == np))
						{
							if (!bridges[t].ipmos)
							{
								if (t < i)
								{
									nooverlap = false;
									break;
								}
							}
							else
							{
								nooverlap = false;
								break;
							}
						}

					}
					if (!nooverlap)
						break;
				}
				if (nooverlap)
				{
					fixable = true;
					outrailnm = i;
					if (i > prev)
						outrailnm--;
					break;
				}
			}
		}

		if (fixable)
		{
			auto bridgetemp = bridges;
			bridges.clear();
			for each(auto b in bridgetemp)
			{
				if (b.layer < prev)
					bridges.push_back(b);
				else
				{
					b.layer = b.layer - 1;
					bridges.push_back(b);
				}
			}
			layercount--;
		}
	}
	void BridgeBuilder(const std::vector<int>& order, bool ipm)
	{
		std::vector<size_t> layer;
		std::vector<size_t> layermap;
		size_t os = order.size();
		std::vector<bool> checked(os, false);
		checked[os - 1] = true;
		std::vector<std::vector<size_t>> bgs;
		int temp;
		size_t start;
		for (size_t i = 0; i < order.size(); i++)
		{
			if (order[i] == -1 || order[i] == -3 || order[i] == -2)
			{
				StickRail SR;
				SR.ipmos = ipm;
				SR.pin = i;
				SR.rail = order[i] + 3;
				rails.push_back(SR);
				if (order[i] == -2)
				{
					if (i < outrail.first)
						outrail.first = i;
					if (i > outrail.second)
						outrail.second = i;
					if (ipm)
						poutrailpins.push_back(i);
					else
						noutrailpins.push_back(i);
				}
				continue;
			}
			if (!checked[i])
			{
				temp = order[i];
				bool fd = false;
				std::vector<size_t> bg;
				bg.push_back(i);
				for (size_t j = i + 1; j < order.size(); j++)
				{
					if (order[j] == temp)
					{
						fd = true;
						bg.push_back(j);
						checked[j] = true;
					}
				}
				if (fd)
					bgs.push_back(bg);

			}
		}
		for (size_t i = 0; i < bgs.size(); i++)
		{
			bool lf = false;
			for (size_t j = 0; j < layer.size(); j++)
			{
				if (layer[j] <= bgs[i][0])
				{
					layer[j] = bgs[i][bgs[i].size() - 1];
					layermap.push_back(layercount + j);
					lf = true;
					break;
				}
			}
			if (lf)
				continue;
			layer.push_back(bgs[i][bgs[i].size() - 1]);
			layermap.push_back(layercount + layer.size()-1);		
		}
		for (size_t j = 0; j<bgs.size(); j++)
		{
			for (size_t i = 0; i < bgs[j].size() - 1; i++)
			{
				StickBridge StB;
				StB.path = { bgs[j][i], bgs[j][i+1]};
				StB.layer = layermap[j];
				StB.ipmos = ipm;
				bridges.push_back(StB);
			}
		}
		/*for (size_t i = 0; i < order.size(); i++)
		{
			for (size_t j = i + 1; j < order.size(); j++)
			{
				if (order[j] == order[i])
				{
					StickBridge StB;
					StB.path = { i, j };
					StB.layer = -1;
					StB.ipmos = ipm;
					bool lf = false;
					for (size_t t = 0; t < layer.size(); t++)
					{
						if (layer[t]<=i)
						{
							lf = true;
							StB.layer = layercount + t;
							layer[t] = j;
							bridges.push_back(StB);
							break;
						}
					}
					if (lf)
						break;
					layer.push_back(j);
					StB.layer = layercount + layer.size() - 1;
					bridges.push_back(StB);
					break;
				}
			}
		}*/
		layercount += layer.size();
		for each (size_t l in layer)
			totallayer.push_back(l);
	}
	std::vector<char> polyseq;
	std::vector<char> polynames;
	std::vector<StickBridge> bridges;
	std::vector<StickRail> rails;
	std::pair<size_t, size_t> outrail;
	std::vector<size_t> poutrailpins;
	std::vector<size_t> noutrailpins;
	size_t outrailnm;
	size_t intervalcount;
	size_t layercount;
	std::vector<size_t> totallayer;
};

class LayoutRect
{
public:

	void SetLayer(int num) { layernumber = num; }

	void SetPos(double l, double d, double r, double u)
	{
		leftbound = l;
		lowerbound = d;
		upperbound = u;
		rightbound = r;
	}

	std::string ToGDS(int lamda)
	{
		int l = leftbound*lamda;
		int u = upperbound*lamda;
		int d = lowerbound*lamda;
		int r = rightbound*lamda;
		std::string rectGDS = "BOUNDARY;\nLAYER ";
		rectGDS = rectGDS + std::to_string(layernumber) + ";\nDATATYPE 0;\nXY 5;\n";
		rectGDS = rectGDS + "\tX: " + std::to_string(l) + ";\t\tY: " + std::to_string(d) + ";\n";
		rectGDS = rectGDS + "\tX: " + std::to_string(r) + ";\t\tY: " + std::to_string(d) + ";\n";
		rectGDS = rectGDS + "\tX: " + std::to_string(r) + ";\t\tY: " + std::to_string(u) + ";\n";
		rectGDS = rectGDS + "\tX: " + std::to_string(l) + ";\t\tY: " + std::to_string(u) + ";\n";
		rectGDS = rectGDS + "\tX: " + std::to_string(l) + ";\t\tY: " + std::to_string(d) + ";\n";
		rectGDS = rectGDS + "ENDEL;\n\n";
		return rectGDS;
	}

private:
	double leftbound;
	double lowerbound;
	double rightbound;
	double upperbound;
	int layernumber;
};

class Contact
{
public:
	void SetPos(double l, double u, bool p)
	{
		if (p)
		{
			LayoutRect LR1, LR2;
			LR1.SetLayer(25);
			LR1.SetPos(l + 1, u + 1.5, l + 3, u + 3.5);
			LR2.SetLayer(25);
			LR2.SetPos(l + 1, u + 6.5, l + 3, u + 8.5);
			ccs.push_back(LR1);
			ccs.push_back(LR2);
		}
		else
		{
			LayoutRect LR;
			LR.SetLayer(25);
			LR.SetPos(l + 1, u + 1.5, l + 3, u + 3.5);
			ccs.push_back(LR);
		}
	}

	std::string ToGDS(int lamda)
	{
		std::string gds;
		for each (auto c in ccs)
			gds += c.ToGDS(lamda);
		return gds;
	}

private:
	std::vector<LayoutRect> ccs;
};

class RailContact
{
public:
	RailContact()
	{
		cc.SetLayer(25);
		active.SetLayer(43);
	}

	void SetPos(double l, double d)
	{
		cc.SetPos(l + 1, d + 1, l + 3, d + 3);
		active.SetPos(l, d, l + 4, d + 4);
	}

	std::string ToGDS(int lamda)
	{
		return cc.ToGDS(lamda) + active.ToGDS(lamda);
	}
private:
	LayoutRect cc;
	LayoutRect active;
};

class NMOS
{
public:
	NMOS()
	{
		active.SetLayer(43);
		nselect.SetLayer(45);
	}
	void SetPos(double l, double d, double r, double u)
	{
		nselect.SetPos(l, d, r, u);
		active.SetPos(l + 2, d + 2, r - 2, u - 2);
	}
	
	std::string ToGDS(int lamda)
	{
		return active.ToGDS(lamda) + nselect.ToGDS(lamda);
	}
private:
	LayoutRect active;
	LayoutRect nselect;
};

class PMOS
{
public:
	PMOS()
	{
		active.SetLayer(43);
		pselect.SetLayer(44);
		nwell.SetLayer(42);
	}

	void SetPos(double l, double d, double r, double u)
	{
		nwell.SetPos(l, d, r, u);
		pselect.SetPos(l + 4, d + 4, r - 4, u - 16);
		active.SetPos(l + 6, d + 6, r - 6, u - 18);
	}

	std::string ToGDS(int lamda)
	{
		return active.ToGDS(lamda) + pselect.ToGDS(lamda) + nwell.ToGDS(lamda);
	}

private:
	LayoutRect active;
	LayoutRect pselect;
	LayoutRect nwell;
};

class PowerRail
{
public:
	PowerRail()
	{
		VDD.SetLayer(49);
		GND.SetLayer(49);
		vselect.SetLayer(45);
		gselect.SetLayer(44);
	}


	void SetPos(double l, double w, double h)
	{
		GND.SetPos(l, 1, l + w, 7);
		gselect.SetPos(l + 4, 0, l + w - 4, 8);
		VDD.SetPos(l, h + 44, l + w, h + 50);
		vselect.SetPos(l + 4, h + 43, l + w - 4, h + 51);
	}

	std::string ToGDS(int lamda)
	{
		std::string gds;
		gds =  VDD.ToGDS(lamda) + GND.ToGDS(lamda)+vselect.ToGDS(lamda) + gselect.ToGDS(lamda);
		return gds;
	}

private:
	LayoutRect VDD;
	LayoutRect vselect;
	LayoutRect GND;
	LayoutRect gselect;
};

class CMOS
{
public:
	void SetPos(double l, double h, double w)
	{
		double d = 12;
		double u = d + 9;
		AN.SetPos(l + 4, d, l + w - 4, u);
		d = u + h;
		u = d + 34;
		AP.SetPos(l, d, l + w, u);
		PR.SetPos(l, w, h);
	}

	std::string ToGDS(int lamda)
	{
		return (AN.ToGDS(lamda) + AP.ToGDS(lamda) + PR.ToGDS(lamda));
	}

private:
	NMOS AN;
	PMOS AP;
	PowerRail PR;
};

class Poly
{
public:
	Poly()
	{
		poly.SetLayer(46);
	}

	void SetPos(double l, double h)
	{
		poly.SetPos(l, 12, l+2, 39+h);
	}

	std::string ToGDS(int lamda)
	{
		return poly.ToGDS(lamda);
	}

private:
	LayoutRect poly;
};

class LayoutBridge
{
public:
	LayoutBridge()
	{
		board.SetLayer(51);
		lhold.SetLayer(49);
		rhold.SetLayer(49);
		lvia.SetLayer(50);
		rvia.SetLayer(50);
	}
	void SetPos(const StickBridge& StB, const std::vector<double>& ppos, const std::vector<double>& lpos, double pbound)
	{
		double bl = ppos[StB.path.first];
		double br = ppos[StB.path.second]+4;
		double bd = lpos[StB.layer];
		double bu = bd + 4;
		board.SetPos(bl, bd, br, bu);
		lvia.SetPos(bl + 1, bd + 1, bl + 3, bu - 1);
		rvia.SetPos(br - 3, bd + 1, br - 1, bu - 1);
		if (StB.ipmos)
		{
			double hu = pbound;
			lhold.SetPos(bl, bd, bl + 4, hu);
			lcc.SetPos(bl, hu - 10, StB.ipmos);
			rhold.SetPos(br - 4, bd, br, hu);
			rcc.SetPos(br-4, hu - 10, StB.ipmos);
		}
		else
		{
			double hu = 14;
			lhold.SetPos(bl, hu, bl + 4, bd+4);
			lcc.SetPos(bl, hu, StB.ipmos);
			rhold.SetPos(br - 4, hu, br, bd+4);
			rcc.SetPos(br - 4, hu, StB.ipmos);
		}
	}

	std::string ToGDS(int lamda)
	{
		return board.ToGDS(lamda) + lhold.ToGDS(lamda) + rhold.ToGDS(lamda) + lvia.ToGDS(lamda) + rvia.ToGDS(lamda)+lcc.ToGDS(lamda)+rcc.ToGDS(lamda);
	}


private:
	LayoutRect board;
	LayoutRect lhold;
	LayoutRect rhold;
	LayoutRect lvia;
	LayoutRect rvia;
	Contact lcc;
	Contact rcc;
};

class LayoutRail
{
public:
	void SetPos(std::vector<StickRail>& SR, std::vector<double> ppos, double h, double o)
	{
		double d, u, ccu;
		LayoutRect LR;
		Contact Lcc;
		for each (auto sr in SR)
		{
			double l = ppos[sr.pin];
			double r = l + 4;
			if (sr.rail == 2)
			{
				d = h + 27;
				u = d + 17;
			}
			if (sr.rail == 0)
			{
				d = 7;
				u = 19;
			}
			if (sr.rail == 1)
			{
				if (sr.ipmos)
				{
					d = h + 37;
					u = o;
				}
				else
				{
					d = o + 4;
					u = 14;
				}
				LayoutRect LR2;
				LR2.SetPos(l + 1, o + 1, l + 3, o + 3);
				LR2.SetLayer(50);
				Pins.push_back(LR2);
			}
			if (!sr.ipmos)
				Lcc.SetPos(l, 14, false);
			else
				Lcc.SetPos(l, h + 27, true);
			ccs.push_back(Lcc);
			LR.SetPos(l, d, r, u);
			LR.SetLayer(49);
			Conns.push_back(LR);
		}
	}
	std::string ToGDS(int lamda)
	{
		std::string gds;
		for each (auto c in Conns)
			gds += c.ToGDS(lamda);
		for each (auto p in Pins)
			gds += p.ToGDS(lamda);
		for each (auto c in ccs)
			gds += c.ToGDS(lamda);
		return gds;
	}
private:
	std::vector<LayoutRect> Conns;
	std::vector<LayoutRect> Pins;
	std::vector<Contact> ccs;
};



class Layout
{
public:
	void SetLamda(int l) { lamda = l; }
	
	std::string Header(const std::string& name)
	{
		std::string time = "{05-9-25 15:53:12}";
		std::string head = "HEADER 5;\nBGNLIB;\n\tLASTMOD\t";
		head += time + ";\n\tLASTACC ";
		head += time + ";\nLIBNAME cse664.DB;\nUNITS;\n\tUSERUNITS 0.001;\n\tPHYSUNITS 9.999999999999999E-10;\n\n";
		head += "BGNSTR;\n\tCREATION " + time + ";\n\tLASTMOD " + time + ";\nSTRNAME "+name+";\n";
		return head+"\n";
	}

	void ToFile(const std::string& file)
	{
		std::string tof = ToGDS(file);
		std::ofstream out(file+".txt");
		out << tof;
		out.close();
	}
	std::string ToGDS(const std::string& name)
	{
		std::string tofile = Header(name);
		for each (auto c in cmos)
			tofile += c.ToGDS(lamda);
		for each (auto p in polys)
			tofile += p.ToGDS(lamda);
		for each(auto b in bridges)
			tofile += b.ToGDS(lamda);
		for each(auto c in RCs)
			tofile += c.ToGDS(lamda);
		tofile += output.ToGDS(lamda);
		tofile += rails.ToGDS(lamda);
	    tofile += "\n\nENDSTR;";
		tofile += "\n\nENDLIB;";
		return tofile;

	}

	void Read(StickLayout& SL)
	{
		Initialize(SL.GetSeq(), SL.getlayercount());
		double pbound = 37 + SL.getlayercount() * 8 - 4;
		SetBridges(SL.GetBridges(), pbound);
		SetOutput(SL.outrailnumber(), SL.GetORail());
		SetRails(SL.GetRails(), (SL.getlayercount() * 8 - 4), lpos[SL.outrailnumber()]);
		SetPowerCCs((SL.getlayercount() * 8 - 4));
	}
	

private:
	void Initialize(const std::vector<char>& seq, const size_t layercount)
	{
		double h = layercount * 8 - 4;
		double l = 0;
		double lp = 6;
		Ppos.push_back(lp);
		double pp = 11;
		Poly py;
		double w = 10;
		double layp = h+19;
		for (size_t i = 0; i < layercount; i++)
		{
			lpos.push_back(layp);
			layp = layp - 8;
		}
		for each (char c in seq)
		{
			if (c != '%')
			{
				w = w + 8;
				lp = lp + 8;
				Ppos.push_back(lp);
				py.SetPos(pp, h);
				polys.push_back(py);
				pp = pp + 8;
			}
			else
			{
				w = w + 6;
				CMOS cm;
				cm.SetPos(l, h, w);
				cmos.push_back(cm);
				l = l + w;
				w = 10;
				pp = pp + 16;
				lp = lp + 16;
				Ppos.push_back(lp);
			}
		}
		w = w + 6;
		CMOS cm;
		cm.SetPos(l, h, w);
		cmos.push_back(cm);
	}

	void SetBridges(const std::vector<StickBridge>& StB, double pbound)
	{
		for each(auto B in StB)
		{
			LayoutBridge LB;
			LB.SetPos(B, Ppos, lpos, pbound);
			bridges.push_back(LB);
		}
	}

	void SetRails(std::vector<StickRail>& SR, double h, double o)
	{
		rails.SetPos(SR, Ppos, h, o);
	}

	void SetOutput(size_t o, std::pair<size_t, size_t> path)
	{
		double l = Ppos[path.first];
		double r = Ppos[path.second]+4;
		double d = lpos[o];
		double u = d + 4;
		output.SetLayer(51);
		output.SetPos(l, d, r, u);
	}

	void SetPowerCCs(double h)
	{
		RailContact VRC, GRC;
		for each (double p in Ppos)
		{
			VRC.SetPos(p, 2);
			GRC.SetPos(p, h + 45);
			RCs.push_back(VRC);
			RCs.push_back(GRC);
		}
	}

	int lamda = 300;
	std::vector<CMOS> cmos;
	std::vector<Poly> polys;
	std::vector<LayoutBridge> bridges;
	LayoutRail rails;
	LayoutRect output;
	std::vector<double> Ppos;
	std::vector<double> lpos;
	std::vector<RailContact> RCs;
};

