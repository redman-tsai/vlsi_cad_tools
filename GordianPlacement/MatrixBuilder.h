#pragma once
#include<Eigen/Dense>
#include<iostream>
#include"Netlist.h"
#include"Region.h"

using namespace Eigen;
class CoefficientMatrices
{
	using module = std::vector<Module>::iterator;
	using Mpin = std::pair<module, Point>;
public:
	MatrixXd C;
	VectorXd dx;
	VectorXd dy;
	CoefficientMatrices(){}

	CoefficientMatrices(int n) :C(n, n), dx(n), dy(n)
	{
		C.setZero();
		dx.setZero();
		dy.setZero();
	}

	void Resize(int n)
	{
		C.resize(n, n);
		dx.resize(n);
		dy.resize(n);
		C.setZero();
		dx.setZero();
		dy.setZero();
	}

	void Build (const NetList& NL)
	{
		size_t n;
		double e;
		for each (Net N in NL.netlist)
		{
			n = N.ModuleChain.size();
			e = 2.0 / n;
			for each (Mpin M in N.ModuleChain)
			{
				if (!M.first->fixed)
				{
					unsigned index_r = NL.conversion.at(M.first->Index);
					C(index_r, index_r) = C(index_r, index_r) + e*n;
					dx(index_r) = dx(index_r) + M.second.x*e*n;
					dy(index_r) = dy(index_r) + M.second.y*e*n;
					for each(Mpin L in N.ModuleChain)
					{
						if (!L.first->fixed)
						{
							unsigned index_c = NL.conversion.at(L.first->Index);
							C(index_r, index_c) = C(index_r, index_c) - e;
							dx(index_r) = dx(index_r) - L.second.x*e;
							dy(index_r) = dy(index_r) - L.second.y*e;
						}
						else
						{
							dx(index_r) = dx(index_r) - (L.second.x + L.first->Central.x)*e;
							dy(index_r) = dy(index_r) - (L.second.y + L.first->Central.y)*e;
						}
					}
				}
			}
		}
	}
};

class ConstraintMatrices
{
public:
	using module = std::vector<Module>::iterator;
	MatrixXd A;
	VectorXd ux, uy;
	std::vector<unsigned> conv;
	ConstraintMatrices(int r, int c) :A(r, c),  ux(r), uy(r)
	{
		A.setZero();
		ux.setZero();
		uy.setZero();
	}

	void Build(std::vector<Region*>& R, const NetList& NL)
	{
		double mm;
		unsigned mi;
		double totalarea;
		size_t n = R.size();
		for(size_t i = 0; i<n; i++)
		{
			mm = 0.0;
			mi = 0;
			totalarea = 0.0;
			for each (module m in R[i]->modules)
				if (!m->fixed)
					totalarea += (m->Length.x)*(m->Length.y);
			for each (module m in R[i]->modules)
			{
				if (!m->fixed)
				{
					A(i, NL.conversion.at(m->Index)) = (m->Length.x)*(m->Length.y) / totalarea;
					if ((m->Length.x)*(m->Length.y) > mm)
					{
						mi = NL.conversion.at(m->Index);
						mm = (m->Length.x)*(m->Length.y);
					}
				}
				/*else
				{
					ux(i) = ux(i) - (m->Central.x)*(m->Length.x)*(m->Length.y) / totalarea;
					uy(i) = uy(i) - (m->Central.y)*(m->Length.x)*(m->Length.y) / totalarea;
				}*/
			}
			ux(i) += R[i]->Central().x;
			uy(i) += R[i]->Central().y;
			conv.push_back(mi);
		}
		order();
		Permute();
	}

	void order()
	{
		std::vector<unsigned> convc;
		std::vector<bool> convb(A.cols(), true);
		for (size_t i = 0; i < conv.size(); i++)
		{
			convb[conv[i]] = false;
			convc.push_back(conv[i]);
		}
		for (size_t i = 0; i < convb.size(); i++)
		{
			if (convb[i])
				convc.push_back(i);
		}
		conv = convc;
	}

	void Permute()
	{
		MatrixXd Ac = A;
		for (size_t i = 0; i < conv.size(); i++)
		{
			A.col(i) = Ac.col(conv[i]);
		}
		//std::cout << "\nbefore perfure\n" << Ac.block(0, 0, A.rows(), 16) << std::endl;
		//std::cout << "\nafter perfure\n" << A.block(0, 0, A.rows(), 16) << std::endl;
	}
};

class MatrixSolver
{
public:
	std::vector<double> solx;
	std::vector<double> soly;
	MatrixSolver(){}
	void Solve(CoefficientMatrices CoM, ConstraintMatrices CtM)
	{
		solx = solve(CoM.C, CtM.A, CoM.dx, CtM.ux, CtM.conv);
		soly = solve(CoM.C, CtM.A, CoM.dy, CtM.uy, CtM.conv);
	}

	std::vector<double> solve(const MatrixXd& C, const MatrixXd& A, const VectorXd& d, const VectorXd& u, const std::vector<unsigned>& conv)
	{
		MatrixXd Cx = C;
		VectorXd dx = d;
		for (size_t i = 0; i < conv.size(); i++)
		{
			dx(i) = d(conv[i]);
			for (size_t j = 0; j < conv.size(); j++)
			{
				Cx(i, j) = C(conv[i], conv[j]);
			}
		}

		int rows = A.rows();
		int cols = A.cols();
		MatrixXd DI = A.block(0, 0, rows, rows);
		for (int i = 0; i < rows; i++)
		{
			double temp = DI(i, i);
			DI(i, i) = 1 / temp;
		}
		
		MatrixXd B = A.block(0, rows,rows, cols - rows);
		MatrixXd Z(cols, cols - rows);
		Z.block(0, 0, rows, cols - rows) = -DI*B;
		Z.block(rows, 0, cols - rows, cols - rows).setIdentity();
		VectorXd x0(cols);
		x0.setZero();
		x0.head(rows) = DI*u;
		VectorXd c = Cx*x0;
		c = c + dx;
		c = Z.transpose()*c;
		MatrixXd coff = Z.transpose()*Cx;
		coff = coff*Z;
		VectorXd xi = coff.llt().solve(-c);
		VectorXd px = Z*xi;
		px = px + x0;
		std::vector<double> fx(px.data(), px.data() + px.size());
		std::vector<double> fxc = fx;
		for (size_t i = 0; i < conv.size(); i++)
		{
			fx[i] = fxc[conv[i]];
		}
		return fx;
	}
	


};