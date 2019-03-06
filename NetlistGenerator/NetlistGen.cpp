#include<stdio.h>
#include "BooleanParser.h"
#include "Netlist.h"
#include "AbstractLayout.h"
void Process(const std::string& input, const std::string& name)
{
	BooleanNode* BN = BooleanParser::ReadEquation(input);
	std::cout << "\n\nInput Boolean expression is:\n";
	std::cout << input;
	std::cout << "\n\nBoolean expression with the poly sequence order is:\n";
	std::cout << BN->BooleanExpression();
	std::cout << "\n\nPoly Sequence is:\n";
	std::cout << BN->PolySequence() << std::endl;
	Netlist* NL = new Netlist();
	StickLayout SL(BN->PolySeq());
	Layout LO;
	NL->Read(BN);
	NL->Print();
	SL.InitiateBridge(NL->GetPOrder(), NL->GetNOrder());
	LO.Read(SL);
	LO.ToFile(name);
}

void main()
{
	std::string input, name;

	input = "OUT=~((((A*B)+(C*D))*E)+(F*G*H)+I)";
	name = "ASIC1";
	Process(input, name);

	input = "OUT=~(K+((((A+B)*C*(D+E+F))+(G*H))*(I+J))+L)";
	name = "ASIC2";
	Process(input, name);

	input = "OUT=~((((((G*(F+(A*B*J)))+C)*H)+I)*D)+E)";
	name = "ASIC3";
	Process(input, name);

	input = "OUT=~((J+K)*(F+((D*E)+C)+((G+H)*I))*(A+B))";
	name = "ASIC4";
	Process(input, name);

	input = "OUT=~(((C+D)*E)+(A*B*F)+(G*H))";
	name = "ASIC5";
	Process(input, name);
	
	system("pause");
}