#pragma once
#include<stdio.h>
#include<string>
#include<vector>
#include"BooleanNode.h"
#ifndef BOOLEANPARSER_H
class BooleanParser
{
public:
	static BooleanNode* ReadEquation(const std::string& ss)
	{
		BooleanNode* BN = Parse(Pivot(ss));
		Reorder(BN);
		return BN;
	}

	static BooleanNode* ReadRHS(const std::string& ss)
	{
		BooleanNode* BN = Parse(ss);
		Reorder(BN);
		return BN;
	}

private:
	// parse function
	static BooleanNode* Parse(const std::string& ss)
	{
		BooleanNode* BN = new BooleanNode();
		int parentorder = 0;
		std::string passthrough;
		for (const char& c : ss)
		{
			if (parentorder == 0)
			{
				if (c == ' ')
					continue; 
				if (c == '~'||c == '*' || c == '+')
				{
					BN->setContent(c);
					continue;
				}
				if (c == '(')
				{
					parentorder++;
					passthrough.clear();
					continue;
				}
				if (c == ')')
					continue;
				BooleanNode* BNc = new BooleanNode(c);
				BN->addChild(BNc);
			}
			else
			{
				if (c == ')')
				{
					parentorder--;
					if (parentorder == 0)
					{
						BooleanNode* Bnc = new BooleanNode();
						Bnc = Parse(passthrough);
						BN->addChild(Bnc);
					}
					else
						passthrough = passthrough + c;
					continue;
				}
				if (c == '(')
					parentorder++;
				passthrough = passthrough + c;
			}
		}
		return BN;
	}

	// reorder sequence order
	static void Reorder(BooleanNode* BN)
	{
		if (BN->getChildrenSize() == 1)
			return Reorder(BN->getChildren().at(0));
		std::vector<BooleanNode*> temp = BN->getChildren();
		if (temp.empty())
			return;
		for each (BooleanNode* t in temp)
			Reorder(t);
		BN->clearChild();
		BN->setNT(2);
		if (temp.size() % 2 == 0)
			BN->setNT(0);
		while (!temp.empty())
		{
			//add white
			for (size_t i = 0; i < temp.size(); i++)
			{
				if (temp.at(i)->getNT() == 0)
				{
					BN->addChild(temp[i]);
					BN->setNT(0);
					temp.erase(temp.begin() + i);
					i--;
				}
			}
			//add blend as wb
			for (size_t i = 0; i < temp.size(); i++)
			{
				if (temp.at(i)->getNT() == 1)
				{
					BN->addChild(temp[i]);
					BN->setNT(1);
					temp.erase(temp.begin() + i);
					break;
				}

				if (temp.at(i)->getNT() == 3)
				{
					temp.at(i)->reverse();
					BN->addChild(temp[i]);
					BN->setNT(1);
					temp.erase(temp.begin() + i);
					break;
				}
			}

			//add black
			for (size_t i = 0; i < temp.size(); i++)
			{
				if (temp.at(i)->getNT() == 2)
				{
					BN->addChild(temp[i]);
					if (BN->getNT() == 0)
						BN->setNT(1);
					temp.erase(temp.begin() + i);
					i--;
				}
			}
			//add blend as bw
			for (size_t i = 0; i < temp.size(); i++)
			{
				if (temp.at(i)->getNT() == 3)
				{
					BN->addChild(temp[i]);
					temp.erase(temp.begin() + i);
					if (BN->getNT() == 2)
						BN->setNT(3);
					else
						BN->setNT(0);
					break;
				}

				if (temp.at(i)->getNT() == 1)
				{
					temp.at(i)->reverse();
					BN->addChild(temp[i]);
					temp.erase(temp.begin() + i);
					if (BN->getNT() == 2)
						BN->setNT(3);
					else
						BN->setNT(0);
					break;
				}
			}

		}
	}

	//get RHs of the equation
	static std::string Pivot(const std::string& ss)
	{
		std::string cc = "";
		bool  start = false;
		for (const char& c : ss)
		{
			if (start)
				 cc += c;
			if (c == '=')
				start = true;
		}
		return cc;
	}
};
#endif