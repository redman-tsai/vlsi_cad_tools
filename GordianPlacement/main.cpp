#include <iostream>
#include "GORDIAN.h"

int main()
{
	std::string file = "./netlist_20.txt";
	GORDIAN GD;
	GD.SetK(1);
	GD.Start(file);

	system("pause");
}