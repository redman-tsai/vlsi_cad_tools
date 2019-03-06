#pragma once
#include<stdio.h>
#include<string>
#include<set>
#include<iterator>
#include<utility>
#include<vector>
#include<map>
#include<iostream>
#include"BooleanNode.h"

class NetlistNode
{
	const int largeint = 10000;
public:
	NetlistNode(const std::string& n) { VOG = true; name = n; }
	NetlistNode(int n, bool p) { pmos = p, number = n; VOG = false; }
	std::string getName()
	{
		if (!VOG)
		{
			if (pmos)
				name = "P" + std::to_string(number);
			else
				name = "N" + std::to_string(number);
		}
		return name;
	}
	int getNumber() { if (VOG) return largeint; return number; }
	int getOrder()
	{
		if (!VOG)
			return number;
		if (name == "VDD")
			return -1;
		if (name == "OUT")
			return -2;
		if (name == "GND")
			return -3;
	}
	void setNumber(int n) { number = n; }
private:
	int number;
	bool pmos;
	bool VOG;
	std::string name;
};

class Netlist
{
	using netlist_nodes = std::set<NetlistNode*>;
	using node = netlist_nodes::iterator;
	using gate = char;
	using path = std::pair<node, node>;
	using netlist = std::map<gate, path>;
	using netorder = std::map<gate, bool>;

public:
	Netlist(){}
	void Read(BooleanNode* BN)
	{
		
		NetlistNode* vdd = new NetlistNode("VDD");
		NetlistNode* gnd = new NetlistNode("GND");
		NetlistNode* out = new NetlistNode("OUT");
		nn.insert(gnd);
		nn.insert(out);
		pn.insert(vdd);
		pn.insert(out);
		AddP(BN, pn.find(vdd), pn.find(out));
		AddN(BN, nn.find(gnd), nn.find(out));
		GetOrder(BN->PolySeq());
	}

	void Print()
	{
		std::cout << "\nPNetlist:\tSource\t\tDrain\n";
		for (std::map<gate, path>::iterator it = pnetlist.begin(); it != pnetlist.end(); ++it)
		{
			std::cout <<(it->first) << '\t'<<'\t' << (*(it->second.first))->getName() << '\t'<<'\t' << (*(it->second.second))->getName();
			std::cout << '\n';
		}
		std::cout << "\nNNetlist:\tSource\t\tDrain\n";
		for (std::map<gate, path>::iterator it = nnetlist.begin(); it != nnetlist.end(); ++it)
		{
			std::cout << (it->first) << '\t'<<'\t' << (*(it->second.first))->getName() << '\t'<<'\t' << (*(it->second.second))->getName();
			std::cout << '\n';
		}
		std::cout << '\n' << std::endl;		
	}

	void PrintOrder()
	{
		for each (int i in porder)
			std::cout << '\n' << i;
		std::cout << '\n' << std::endl;
		for each (int i in norder)
			std::cout << '\n' << i;
		std::cout << '\n' << std::endl;
	}

	std::vector<int> GetPOrder()
	{
		return porder;
	}

	std::vector<int> GetNOrder()
	{
		return norder;
	}

private:
	int POrderedPathS(char c)
	{
		if (po[c])
			return (*(pnetlist[c].first))->getOrder();
		return (*(pnetlist[c].second))->getOrder();
	}

	int POrderedPathE(char c)
	{
		if (po[c])
			return (*(pnetlist[c].second))->getOrder();
		return (*(pnetlist[c].first))->getOrder();
	}

	int NOrderedPathS(char c)
	{
		if (no[c])
			return (*(nnetlist[c].first))->getOrder();
		return (*(nnetlist[c].second))->getOrder();
	}

	int NOrderedPathE(char c)
	{
		if (no[c])
			return (*(nnetlist[c].second))->getOrder();
		return (*(nnetlist[c].first))->getOrder();
	}

	void GetOrder(std::vector<char> order)
	{
		bool pivot = true;
		for each(char c in order)
		{
			if (c != '%')
			{
				if (!pivot)
				{
					porder.push_back(POrderedPathE(c));
					norder.push_back(NOrderedPathE(c));
				}
				else
				{
					porder.push_back(POrderedPathS(c));
					porder.push_back(POrderedPathE(c));
					norder.push_back(NOrderedPathS(c));
					norder.push_back(NOrderedPathE(c));
					pivot = false;
				}
			}
			else
				pivot = true;
		}
	}
	void AddP(BooleanNode* BN, node& ps, node& pd, bool reverse = false)
	{
		bool rev = !reverse;
		if (BN->getChildrenSize() == 1)
			return AddP(BN->getChildren().at(0), ps, pd);
		if (BN->getContent() == '*')
		{
			if ((BN->getChildrenSize() % 2 != 1) && ((BN->getNT()==0 && BN->getOrder() == 0)||BN->getNT()==1))
				rev = !rev;
			for (size_t i = 0; i < BN->getChildrenSize(); i++)
			{
				rev = !rev;
				size_t j = i*(1 - BN->getOrder()) + (BN->getChildrenSize() - 1 - i)*BN->getOrder();
				BooleanNode* bn = BN->getChildren().at(j);
				if (bn->getChildrenSize() == 0)
				{
					char NG = bn->getContent();
					AddPath(NG, ps, pd, true, rev);
				}
				else
					AddP(bn, ps, pd, rev);
			}
		}
		if (BN->getContent() == '+')
		{
			rev = !rev;
			node nps, npd;
			nps = ps;
			size_t j;
			for (size_t i = 0; i < BN->getChildrenSize() - 1; i++)
			{
				if (!reverse)
					j = i*(1 - BN->getOrder()) + (BN->getChildrenSize() - 1 - i)*BN->getOrder();
				else
					j = i*BN->getOrder() + (BN->getChildrenSize() - 1 - i)*(1 - BN->getOrder());
				BooleanNode* bn = BN->getChildren().at(j);
				int npdn = 2 * (pn.size() - 2)+1;
				if (npdn >(*pd)->getNumber())
				{
					int temp = (*pd)->getNumber();
					(*pd)->setNumber(npdn);
					npdn = temp;
				}
				NetlistNode* NPD = new NetlistNode(npdn, true);
				pn.insert(NPD);
				npd = pn.find(NPD);
				if (bn->getChildrenSize() == 0)
				{
					char NG = bn->getContent();
					AddPath(NG, nps, npd, true, rev);
					nps = npd;
					continue;
				}
				AddP(bn, nps, npd, rev);
				if ((*npd)->getNumber() > (*pd)->getNumber())
				{
					int temp = (*npd)->getNumber();
					(*npd)->setNumber((*pd)->getNumber());
					(*pd)->setNumber(temp);
				}
				nps = npd;
			}
			npd = pd;
			if (!reverse)
				j = (BN->getChildrenSize() - 1)*(1 - BN->getOrder());
			else
				j = (BN->getChildrenSize() - 1)*BN->getOrder();
			BooleanNode* bn = BN->getChildren().at(j);
			if (bn->getChildrenSize() == 0)
			{
				char NG = bn->getContent();
				AddPath(NG, nps, npd, true, rev);
			}
			else
				AddP(bn, nps, npd, rev);
		}
	}
	void AddN(BooleanNode* BN, node& ns, node& nd, bool reverse = false)
	{
		bool rev = !reverse;
		if (BN->getChildrenSize() == 1)
			return AddN(BN->getChildren().at(0), ns, nd);
		if (BN->getContent() == '+')
		{
			if ((BN->getChildrenSize() % 2 != 1) && ((BN->getNT() == 0 && BN->getOrder() == 0) || BN->getNT() == 1))
				rev = !rev;
			for (size_t i = 0; i < BN->getChildrenSize(); i++)
			{
				rev = !rev;
				size_t j = i*(1 - BN->getOrder()) + (BN->getChildrenSize() - 1 - i)*BN->getOrder();
				BooleanNode* bn = BN->getChildren().at(j);
				if (bn->getChildrenSize() == 0)
				{
					char NG = bn->getContent();
					AddPath(NG, ns, nd, false, rev);
				}
				else
					AddN(bn, ns, nd, rev);
			}
		}
		if (BN->getContent() == '*')
		{
			rev = !rev;
			node nns, nnd;
			nns = ns;
			size_t j;
			for (size_t i = 0; i < BN->getChildrenSize() - 1; i++)
			{
				if (!reverse)
					j = i*(1 - BN->getOrder()) + (BN->getChildrenSize() - 1 - i)*BN->getOrder();
				else
					j = i*BN->getOrder() + (BN->getChildrenSize() - 1 - i)*(1 - BN->getOrder());
				BooleanNode* bn = BN->getChildren().at(j);
				int nndn = 2 * (nn.size() - 2);
				if (nndn >(*nd)->getNumber())
				{
					int temp = (*nd)->getNumber();
					(*nd)->setNumber(nndn);
					nndn = temp;
				}
				NetlistNode* NND = new NetlistNode(nndn, false);
				nn.insert(NND);
				nnd = nn.find(NND);
				if (bn->getChildrenSize() == 0)
				{
					char NG = bn->getContent();
					AddPath(NG, nns, nnd, false, rev);
					nns = nnd;
					continue;
				}
				AddN(bn, nns, nnd, rev);
				if ((*nnd)->getNumber() > (*nd)->getNumber())
				{
					int temp = (*nnd)->getNumber();
					(*nnd)->setNumber((*nd)->getNumber());
					(*nd)->setNumber(temp);
				}
				nns = nnd;
			}
			nnd = nd;
			if (!reverse)
				j = (BN->getChildrenSize() - 1)*(1 - BN->getOrder());
			else
				j = (BN->getChildrenSize() - 1)*BN->getOrder();
			BooleanNode* bn = BN->getChildren().at(j);
			if (bn->getChildrenSize() == 0)
			{
				char NG = bn->getContent();
				AddPath(NG, nns, nnd, false, rev);
			}
			else
				AddN(bn, nns, nnd, rev);
		}
	}

	void AddPath(gate g, node& s, node& d, bool p, bool order)
	{
		path pth = { s, d };
		if (p)
		{
			pnetlist.insert({ g, pth });
			po.insert({ g, !order });
		}
		else
		{
			nnetlist.insert({ g, pth });
			no.insert({ g, !order });
		}
	}
	
	netlist pnetlist;
	netlist nnetlist;
	netlist_nodes nn;
	netlist_nodes pn;
	netorder no;
	netorder po;
	std::vector<int> porder;
	std::vector<int> norder;
};