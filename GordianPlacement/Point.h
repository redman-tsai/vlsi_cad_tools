#pragma once
class Point
{
public:
	Point()
	{
		x = 0.0;
		y = 0.0;
	}

	Point(double vx, double vy)
	{
		x = vx;
		y = vy;
	}

	void set(double vx = 0.0, double vy = 0.0)
	{
		x = vx;
		y = vy;
	}

	double Dist(const Point& B)
	{
		double dx = x - B.x;
		double dy = y - B.y;
		return sqrt(dx*dx + dy*dy);
	}

	double Norm()
	{
		return (x*x + y*y);
	}

	Point& operator=(const Point& B)
	{
		x = B.x;
		y = B.y;
		return *this;
	}

	bool operator<(const Point& B)
	{
		if (x < B.x && y < B.y)
			return true;
		return false;
	}

	bool operator>(const Point& B)
	{
		if (x > B.x && y > B.y)
			return true;
		return false;
	}

	bool operator<=(const Point& B)
	{
		if (x <= B.x && y <= B.y)
			return true;
		return false;
	}

	bool operator>=(const Point& B)
	{
		if (x >= B.x && y >= B.y)
			return true;
		return false;
	}

	Point& operator+(const Point& B)
	{
		Point* temp = new Point();
		temp->x = x + B.x;
		temp->y = y + B.y;
		return *temp;
	}

	Point& operator-(const Point& B)
	{
		Point* temp = new Point();
		temp->x = x - B.x;
		temp->y = y - B.y;
		return *temp;
	}

	Point& operator*(double B)
	{
		Point* temp = new Point();
		temp->x = x*B;
		temp->y = y*B;
		return *temp;
	}

	Point& operator*(int B)
	{
		Point* temp = new Point();
		temp->x = x*B;
		temp->y = y*B;
		return *temp;
	}

	Point& operator*(size_t B)
	{
		Point* temp = new Point();
		temp->x = x*B;
		temp->y = y*B;
		return *temp;
	}


	Point& operator/(double B)
	{
		Point* temp = new Point();
		temp->x = x / B;
		temp->y = y / B;
		return *temp;
	}

	Point& operator/(int B)
	{
		Point* temp = new Point();
		temp->x = x / B;
		temp->y = y / B;
		return *temp;
	}

	Point& operator/(size_t B)
	{
		Point* temp = new Point();
		temp->x = x / B;
		temp->y = y / B;
		return *temp;
	}

	double x;
	double y;

};