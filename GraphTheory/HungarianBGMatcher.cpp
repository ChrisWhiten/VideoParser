/*
 *   Copyright (c) 2007 John Weaver
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#include "HungarianBGMatcher.h"

#include <iostream>

#define Z_NORMAL 0
#define Z_STAR 1
#define Z_PRIME 2

using namespace vpl;

typedef HungarianBGMatcher Munkres;

bool Munkres::find_uncovered_in_matrix(const double& item, 
									   unsigned& row, unsigned& col) 
{
	for ( row = 0 ; row < matrix.rows() ; row++ )
		if ( !row_mask[row] )
			for ( col = 0 ; col < matrix.columns() ; col++ )
				if ( !col_mask[col] )
					if ( matrix(row,col) == item )
						return true;

	return false;
}

bool Munkres::pair_in_list(const std::pair<int,int> &needle, 
						   const std::list<std::pair<int,int> > &haystack) 
{
	for ( std::list<std::pair<int,int> >::const_iterator i = haystack.begin(); 
		i != haystack.end() ; i++ ) 
	{
		if ( needle == *i )
			return true;
	}
	
	return false;
}

int Munkres::step1(void) 
{
	for ( unsigned row = 0 ; row < matrix.rows() ; row++ )
		for ( unsigned col = 0 ; col < matrix.columns() ; col++ )
			if ( matrix(row,col) == 0 ) {
				bool isstarred = false;
				for ( unsigned nrow = 0 ; nrow < matrix.rows() ; nrow++ )
					if ( mask_matrix(nrow,col) == Z_STAR )
						isstarred = true;

				if ( !isstarred ) {
					for ( unsigned ncol = 0 ; ncol < matrix.columns() ; ncol++ )
						if ( mask_matrix(row,ncol) == Z_STAR )
							isstarred = true;
				}
							
				if ( !isstarred ) {
					mask_matrix(row,col) = Z_STAR;
				}
			}

	return 2;
}

int Munkres::step2(void) 
{
	unsigned covercount = 0;

	for ( unsigned row = 0 ; row < matrix.rows() ; row++ )
		for ( unsigned col = 0 ; col < matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_STAR ) {
				col_mask[col] = true;
				covercount++;
			}
			
	unsigned k = matrix.min_size();

	if ( covercount >= k ) {
#ifdef MUNKRES_DEBUG
		std::cout << "Final cover count: " << covercount << std::endl;
#endif
		return 0;
	}

#ifdef MUNKRES_DEBUG
	std::cout << "Munkres matrix has " << covercount << " of " << k << " Columns covered:" << std::endl;
	for ( int row = 0 ; row < matrix.rows() ; row++ ) {
		for ( int col = 0 ; col < matrix.columns() ; col++ ) {
			std::cout.width(8);
			std::cout << matrix(row,col) << ",";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif


	return 3;
}

/*!
	Main Zero Search

   1. Find an uncovered Z in the distance matrix and prime it. If no such zero exists, go to Step 5
   2. If No Z* exists in the row of the Z', go to Step 4.
   3. If a Z* exists, cover this row and uncover the column of the Z*. Return to Step 3.1 to find a new Z
*/
int Munkres::step3(void) 
{
	if ( find_uncovered_in_matrix(0, saverow, savecol) ) {
		mask_matrix(saverow,savecol) = Z_PRIME; // prime it.
	} else {
		return 5;
	}

	for ( unsigned ncol = 0 ; ncol < matrix.columns() ; ncol++ )
		if ( mask_matrix(saverow,ncol) == Z_STAR ) {
			row_mask[saverow] = true; //cover this row and
			col_mask[ncol] = false; // uncover the column containing the starred zero
			return 3; // repeat
		}

	return 4; // no starred zero in the row containing this primed zero
}

/*!
	Increment Set of Starred Zeros

   1. Construct the ``alternating sequence'' of primed and starred zeros:

         Z0 : Unpaired Z' from Step 4.2 
         Z1 : The Z* in the column of Z0
         Z[2N] : The Z' in the row of Z[2N-1], if such a zero exists 
         Z[2N+1] : The Z* in the column of Z[2N]

      The sequence eventually terminates with an unpaired Z' = Z[2N] for some N.
*/
int Munkres::step4(void) 
{
	std::list<std::pair<int,int> > seq;
	// use saverow, savecol from step 3.
	std::pair<int,int> z0(saverow, savecol);
	std::pair<int,int> z1(-1,-1);
	std::pair<int,int> z2n(-1,-1);
	seq.insert(seq.end(), z0);
	unsigned row, col = savecol;

	bool madepair;
	do {
		madepair = false;
		for ( row = 0 ; row < matrix.rows() ; row++ )
			if ( mask_matrix(row,col) == Z_STAR ) {
				z1.first = row;
				z1.second = col;
				if ( pair_in_list(z1, seq) )
					continue;
				
				madepair = true;
				seq.insert(seq.end(), z1);
				break;
			}

		if ( !madepair )
			break;

		madepair = false;

		for ( col = 0 ; col < matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_PRIME ) {
				z2n.first = row;
				z2n.second = col;
				if ( pair_in_list(z2n, seq) )
					continue;
				madepair = true;
				seq.insert(seq.end(), z2n);
				break;
			}
	} while ( madepair );

	for ( std::list<std::pair<int,int> >::iterator i = seq.begin() ;
		  i != seq.end() ;
		  i++ ) {
		// 2. Unstar each starred zero of the sequence.
		if ( mask_matrix(i->first,i->second) == Z_STAR )
			mask_matrix(i->first,i->second) = Z_NORMAL;

		// 3. Star each primed zero of the sequence,
		// thus increasing the number of starred zeros by one.
		if ( mask_matrix(i->first,i->second) == Z_PRIME )
			mask_matrix(i->first,i->second) = Z_STAR;
	}

	// 4. Erase all primes, uncover all columns and rows, 
	for ( unsigned row = 0 ; row < mask_matrix.rows() ; row++ )
		for ( unsigned col = 0 ; col < mask_matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_PRIME )
				mask_matrix(row,col) = Z_NORMAL;
	
	for ( unsigned i = 0 ; i < matrix.rows() ; i++ ) {
		row_mask[i] = false;
	}

	for ( unsigned i = 0 ; i < matrix.columns() ; i++ ) {
		col_mask[i] = false;
	}

	// and return to Step 2. 
	return 2;
}

/*!
	New Zero Manufactures

   1. Let h be the smallest uncovered entry in the (modified) distance matrix.
   2. Add h to all covered rows.
   3. Subtract h from all uncovered columns
   4. Return to Step 3, without altering stars, primes, or covers. 
*/
int Munkres::step5(void) 
{
	double h = 0;
	for ( unsigned row = 0 ; row < matrix.rows() ; row++ ) {
		if ( !row_mask[row] ) {
			for ( unsigned col = 0 ; col < matrix.columns() ; col++ ) {
				if ( !col_mask[col] ) {
					if ( (h > matrix(row,col) && matrix(row,col) != 0) || h == 0 ) {
						h = matrix(row,col);
					}
				}
			}
		}
	}

	for ( unsigned row = 0 ; row < matrix.rows() ; row++ )
		for ( unsigned col = 0 ; col < matrix.columns() ; col++ ) {
			if ( row_mask[row] )
				matrix(row,col) += h;

			if ( !col_mask[col] )
				matrix(row,col) -= h;
		}

	return 3;
}

/*!
	Linear assignment problem solution
	[modifies matrix in-place.]
	matrix(row,col): row major format assumed.

	Assignments are remaining 0 values on 'matrix'
	(extra 0 values are replaced with -1)

	The correspondence results are stored in the variables
	row2colMap and col2rowMap s.t.

		row2colMap(r) = c means that col c is assigned to row r
		col2rowMap(c) = r means that row r is assigned to column c

	@param initMaps if true the row/col maps are init to invalid values,
	                which help check the mapping by calling CheckMapping()
*/
double Munkres::SolveMinCost(const DoubleMatrix& m) 
{
#ifdef MUNKRES_DEBUG
	std::cout << "Munkres input matrix:" << std::endl;

	for ( int row = 0 ; row < m.rows() ; row++ ) 
	{
		for ( int col = 0 ; col < m.columns() ; col++ ) {
			std::cout.width(8);
			std::cout << m(row,col) << ",";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif

	bool notdone = true;
	int step = 1;

	// Copy m, because it need to be modified
	matrix = m;

	// Z_STAR == 1 == starred, Z_PRIME == 2 == primed
	mask_matrix.set_size(matrix.rows(), matrix.columns());
	mask_matrix.fill(0);

	row_mask = new bool[matrix.rows()];
	col_mask = new bool[matrix.columns()];

	for ( unsigned i = 0 ; i < matrix.rows() ; i++ ) {
		row_mask[i] = false;
	}

	for ( unsigned i = 0 ; i < matrix.columns() ; i++ ) {
		col_mask[i] = false;
	}

	while ( notdone ) {
		switch ( step ) {
			case 0:
				notdone = false;
				break;
			case 1:
				step = step1();
				break;
			case 2:
				step = step2();
				break;
			case 3:
				step = step3();
				break;
			case 4:
				step = step4();
				break;
			case 5:
				step = step5();
				break;
		}
	}

	delete[] row_mask;
	delete[] col_mask;

	// Store results
	/*
	for ( unsigned row = 0 ; row < matrix.rows() ; row++ )
		for ( unsigned col = 0 ; col < matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_STAR )
				matrix(row,col) = 0;
			else
				matrix(row,col) = -1;
				
	#ifdef MUNKRES_DEBUG
		std::cout << "Munkres output matrix:" << std::endl;
		for ( int row = 0 ; row < matrix.rows() ; row++ ) {
			for ( int col = 0 ; col < matrix.columns() ; col++ ) {
				std::cout.width(1);
				std::cout << matrix(row,col) << ",";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	#endif		
	*/

	double matchCost = 0;

	for (unsigned row = 0 ; row < matrix.rows() ; ++row)
	{
		for (unsigned col = 0 ; col < matrix.columns() ; ++col)
		{
			if (mask_matrix(row,col) == Z_STAR)
			{
				matchCost += m[row][col]; // use given 'm' not 'matrix'!
			}
		}
	}

	return matchCost;
}

/*! 
	Returns the correspondences found by the SolveMinCost() function.
		
	Stores results in the row and col maps s.t.
	row2colMap(r) = c means that col c is assigned to row r
	col2rowMap(c) = r means that row r is assigned to column c
*/
void HungarianBGMatcher::GetCorrespondences(UIntVector& row2colMap, 
	UIntVector& col2rowMap)
{
	// Init vectors to "known" invalid values
	row2colMap.set_size(matrix.rows());
	row2colMap.fill(UINT_MAX);
		
	col2rowMap.set_size(matrix.cols());
	col2rowMap.fill(UINT_MAX);
		
	for (unsigned row = 0 ; row < matrix.rows() ; ++row)
	{
		for (unsigned col = 0 ; col < matrix.columns() ; ++col)
		{
			if (mask_matrix(row,col) == Z_STAR)
			{
				row2colMap[row] = col;
				col2rowMap[col] = row;
			}
		}
	}
}