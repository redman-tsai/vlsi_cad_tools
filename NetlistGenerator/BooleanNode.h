#pragma once
#include<stdio.h>
#include<string>
#include<list>
#include<vector>
class BooleanNode
{
public:
	BooleanNode() { travelorder = 0; }
	BooleanNode(char c) { content = c; nodetype = 2; travelorder = 0; }
	void addChild(BooleanNode* c) { children.push_back(c); }
	void RemoveChild(size_t c) { children.erase(children.begin() + c); }
	void clearChild() { children.clear(); }
	void setContent(char c) { content = c; }
	void setNT(int c) { nodetype = c; }
	size_t getChildrenSize() { return children.size(); }
	std::vector<BooleanNode*> getChildren() { return children; }
	char getContent() { return content; }
	int getNT() { return nodetype; }
	int getOrder() { return travelorder; }
	void reverse() 
	{ 
		if (nodetype == 1 || nodetype == 3) 
			nodetype = 4 - nodetype; 
		travelorder = 1 - travelorder; 
		for each(BooleanNode* bn in children) 
			bn->reverse(); 
	}
	std::string BooleanExpression()
	{
		std::string c = "";
		if (children.size() == 1)
		{
			c = "~" + children[0]->BooleanExpression();
			return c;
		}
		if (children.empty())
		{
			c += content;
			return c;
		}
		c += '(';
		if (travelorder == 0)
		{
			for (size_t t = 0; t < children.size() - 1; t++)
				c += children[t]->BooleanExpression() + content;
			c += children[children.size() - 1]->BooleanExpression();
		}
		else
		{
			for (size_t t = children.size() - 1; t >0; t--)
				c += children[t]->BooleanExpression() + content;
			c += children[0]->BooleanExpression();
		}
		c += ')';
		return c;
	}

	std::string PolySequence()
	{
		return topoly();
	}

	std::vector<char> PolySeq()
	{
		std::string temp = topoly();
		std::vector<char> p;
		for each (char c in temp)
			p.push_back(c);
		return p;
	}

private:
	std::string topoly()
	{
		std::string p = "";
		if (children.size() == 1)
			return children[0]->topoly();
		if (children.empty())
		{
			p += content;
			return p;
		}
		bool gap = false;
		for (size_t i = 0; i < children.size(); i++)
		{
			size_t t = i*(1 - travelorder) + (children.size() - 1 - i)*travelorder;
			if (!gap)
			{
				p = p + children[t]->topoly();
				if (children[t]->nodetype == 0)
					gap = true;
			}
			else
			{
				p = p + "%" + children[t]->topoly();
				gap = false;
				if (children[t]->nodetype == 0 || children[t]->nodetype == 3)
					gap = true;
			}
		}
		return p;
	}

	std::vector<BooleanNode*> children;
	int travelorder;// children order, top down or bottom up, when a node is flipped, only this need to be changed in stead of its whole children.
	int nodetype;//0 all white, 1 whiteblack, 2 black, 3 blcakwhite. this is used to determine sequence, no real pseudo node is inserted, only node type is changed.
	char content;// operator or variable
};