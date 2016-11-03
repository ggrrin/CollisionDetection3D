#pragma once
#include <glm/glm.hpp>
#include <exception>
#include <vector>

namespace CollisionDetection3D
{
	using namespace std;
	static float GlobalEpsilon = 0.01f; //0.0001;

	template< typename T>
	class GSElimination
	{
	public:
		GSElimination(T& m, float ep = GlobalEpsilon) : mat(m), epsilon(ep), Rows(mat.length())
		{
			if (Rows < 1)
				throw std::logic_error("empty matrix!");
			Columns = mat[0].length();
		}

		int Rows;
		int Columns;
		T& mat;

		float epsilon;

		template<typename ResultVec, typename ComplementVec>
		vector<ResultVec> Result()
		{
			vector<ResultVec> results; 
			auto trans = transpose(mat);

			int height = 0;
			for (int column = 0; column < trans.length(); column++)
			{
				auto& col = trans[column];
				if (height >= col.length() || abs(col[height]) < epsilon)//jsme mimo matici nebo v nebazickem sloupec
				{
					ResultVec x(-col, ComplementVec(0));//minus prevod na 2. stranu
					if (column < trans.length() -1)
						x[height] = 1;//nebazickysloupec
					results.push_back(x);
				}
				else//pivot				
					height++;
			}

			return move(results);
		}

		

		int Elimine()
		{
			bool oneSolution = true;
			int height = 0;
			for (int column = 0; column < Columns; column++)
			{
				if (abs(mat[height][column]) < epsilon)//if (fields[height, column] == 0)
					if (!FindMaxPivot(column, height))
					{
						if (column < Columns)
							oneSolution = false;
						continue;//pokud neni v sloupci neuulová hodnota přeskoč ho      
					}

				for (int row = 0; row < Rows; row++)
					if (row != height)
						if (abs(mat[row][column]) > epsilon)//if (fields[row, column] != 0) 
						{
							MultiplyAndAddRowToAnother(height, row, -mat[row][column] / mat[height][column]);
							//mat[row][column] = 0;//remove inaccuracy where we know we can// probably does not have much effect
						}

				MultiplyRow(height, 1 / mat[height][column]);

				height++;
				if (height >= Rows) break;
			}
			return height;
		}


		/// <summary>
		/// Methods find max value pivot and switch rows in order to make RREF.
		/// </summary>
		/// <param name="column">Search in this specific column.</param>
		/// <param name="height">Searcho from this row to end.</param>
		/// <returns>Returns false wheather nonzero does not exist.</returns>
		/*bool FindMaxPivot(int column, int height)
		{
		return FindMaxPivot(column, height, new MatrixND(Rows, Columns, false));
		}*/


		/// <summary>
		/// Najde nejvetsi poivota, aby se alespon castecne eliminovaly chyby foating point aritmentky
		/// </summary>
		/// <param name="column"></param>
		/// <param name="height"></param>
		/// <param name="matrix"></param>
		/// <returns></returns>
		bool FindMaxPivot(int column, int height/*, MatrixND matrix*/)//wtf??
		{
			int max = height;

			for (int row = height; row < Rows; row++)
				if (abs(mat[row][column]) > epsilon)//if (fields[row, column] != 0)                
					if (abs(mat[max][column]) < abs(mat[row][column]))
						max = abs(row);

			if (abs(mat[max][column]) < epsilon)
				return false;
			else
			{
				SwitchRows(height, max);
				//matrix.SwitchRows(height, max); //wtf??
				return true;
			}
		}

		void MultiplyAndAddRowToAnother(int row1, int row2, float scalar)
		{
			for (int i = 0; i < Columns; i++)
				mat[row2][i] += scalar * mat[row1][i];
		}

		/// <summary>
		/// Multiple specific row by scalar.
		/// </summary>
		/// <param name="row">Specific row.</param>
		/// <param name="scalar">Specific scalar.</param>
		void MultiplyRow(int row, float scalar)
		{
			for (int i = 0; i < Columns; i++)
			{
				mat[row][i] *= scalar;
			}
		}

		void SwitchRows(int row1, int row2)
		{
			if (row1 == row2)
				return;

			auto tempRow = mat[row2];
			mat[row2] = mat[row1];
			mat[row1] = tempRow;


			/*for (int i = 0; i < Columns; i++)
			{
			float temp = mat[row2][i];
			mat[row2][i] = mat[row1][i];
			mat[row1][i] = temp;
			}*/
		}

	};

}