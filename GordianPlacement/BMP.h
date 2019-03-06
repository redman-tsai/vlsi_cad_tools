#pragma once
#include <string>
#include "EasyBMP.h"
#include "EasyBMP_Geometry.h"
#include "EasyBMP_Font.h"
#include "Netlist.h"
class Drawer
{
public:
	BMP Image;
	void Set(double W, double H)
	{
		w = W;
		h = H;
		Image.SetSize((w*1.2)*mag, h*1.2*mag);
		RGBApixel fc;
		fc.Red = 0;
		fc.Green = 0;
		fc.Blue = 255;
		int l, u, r, d;
		l = shift*w*mag + mag * 0;
		u = shift*h*mag + mag * 0;
		r = shift*w*mag + mag * w;
		d = shift*h*mag + mag * h;
		DrawFastLine(Image, l, d, l, u, fc);
		DrawFastLine(Image, l, u, r, u, fc);
		DrawFastLine(Image, r, u, r, d, fc);
		DrawFastLine(Image, r, d, l, d, fc);
	}

	void DrawBox(Module M)
	{
		RGBApixel fc;
		fc.Red = 255;
		fc.Green = 0;
		fc.Blue = 0;
		int l, u, r, d;
	    l = shift*w*mag+  mag * (M.Central - M.Length / 2).x;
		u = shift*h*mag + mag * (M.Central + M.Length / 2).y;
		r = shift*w*mag + mag * (M.Central + M.Length / 2).x;
		d = shift*h*mag + mag * (M.Central - M.Length / 2).y;
		DrawFastLine(Image, l, d, l, u, fc);
		DrawFastLine(Image, l, u, r, u, fc);
		DrawFastLine(Image, r, u, r, d, fc);
		DrawFastLine(Image, r, d, l, d, fc);
		std::string num = std::to_string(M.Index);
		char* tst = (char*)num.c_str();
		PrintString(Image, tst, l, d, 50, fc);
	}

	void DrawNet(Net N)
	{
		RGBApixel fc;
		fc.Red = 0;
		fc.Green = 255;
		fc.Blue = 0;
		int l, u, r, d;
		l = shift*w*mag + mag * (N.Central.x);
		d = shift*h*mag + mag * (N.Central.y);
		for each (auto m in N.ModuleChain)
		{
			r = shift*w*mag + mag * (m.first->Central.x + m.second.x);
			u = shift*h*mag + mag * (m.first->Central.y + m.second.y);
			DrawFastLine(Image, l, d, r, u, fc);
		}
	}

	void Save(const std::string& filename)
	{
		Image.WriteToFile(filename.c_str());
	}

	void Plot(const std::vector<double>& fig, double max)
	{
		Image.SetSize(8000, 8000);
		int interval = 7000 / (fig.size()+1);
		int sx = 500;
		int sy = 500;
		int ey = 7500;
		int ex = 7500;
		RGBApixel fc;
		fc.Red = 255;
		fc.Green = 0;
		fc.Blue = 0;
		DrawFastLine(Image, sx, 8000-sy, sx, 8000-ey, fc);
		DrawFastLine(Image, sx, 8000-sy, ex, 8000-sy, fc);
		ex = 500;
		fc.Red = 0;
		fc.Blue = 255;
		for each (double f in fig)
		{
			ex += interval;
			ey = 500 + f * 7000 / max;
			DrawFastLine(Image, sx, 8000 - sy, ex, 8000 - ey, fc);
			std::string num = std::to_string(f);
			char* tst = (char*)num.c_str();
			PrintString(Image, tst, ex, 8000 - ey, 50, fc);
			sx = ex;
			sy = ey;
		}


	}
private:
	double w;
	double h;
	int mag = 100;
	double shift = 0.1;
};


