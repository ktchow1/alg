#include<iostream>
#include<string>
#include<vector>

namespace interview {
template<typename T, int SZY, int SZX> struct matrix
{
	enum direction { L2R, DOWN, R2L, UP };

	matrix(const T& initial_value) : ptr(new T[SZY * SZX])
	{
		T temp = initial_value;
		for(int y=0; y!=SZY; ++y)
		{
			for(int x=0; x!=SZX; ++x)
			{
				ptr[y*SZX+x] = temp;
				++temp;
			}
		}		
	}

	void spiral_traverse(std::vector<T>& ans)
	{
		int offset_y = 0;
		int offset_x = 0;
		int opsize_y = SZY;
		int opsize_x = SZX;

		while(opsize_y > 1 && opsize_x > 1)
		{
			line_traverse(offset_y,            offset_x,            opsize_x-1, L2R,  ans);
			line_traverse(offset_y,            offset_x+opsize_x-1, opsize_y-1, DOWN, ans);
			line_traverse(offset_y+opsize_y-1, offset_x+opsize_x-1, opsize_x-1, R2L,  ans);
			line_traverse(offset_y+opsize_y-1, offset_x,            opsize_y-1, UP,   ans);
			opsize_y -= 2;
			opsize_x -= 2;
			++offset_y;
			++offset_x;
		}		
		if (opsize_y == 1)
		{
			line_traverse(offset_y, offset_x, opsize_x, L2R, ans);
		}
		else if (opsize_x == 1)
		{
			line_traverse(offset_y, offset_x, opsize_y, DOWN, ans);
		}
	}

	void line_traverse(int y, int x, int len, direction dir, std::vector<T>& ans)
	{		
		for(int n=0; n!=len; ++n)
		{
			ans.push_back(ptr[y*SZX+x]);
			if      (dir == L2R)  ++x;
			else if (dir == DOWN) ++y;
			else if (dir == R2L)  --x;
			else                  --y;
		}
	}

	void debug() const
	{
		for(int y=0; y!=SZY; ++y)
		{			
			for(int x=0; x!=SZX; ++x) std::cout << ptr[y*SZX+x] << " ";
			std::cout << "\n";
		}
	}
	
	T* ptr;
};

void test_citadel_matrix_spiral()
{
	// Test all combinations : 1x1, 2x2, 3x3, 1x100 ...
	matrix<int, 8, 7> m(0);
	m.debug();

	std::vector<int> vec;
	m.spiral_traverse(vec);

	std::cout << "\n\n";
	for(auto& x : vec) std::cout << x << " ";
	std::cout << "\n\n";
}
}
