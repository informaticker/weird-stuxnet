#include <vector>
#include "NWAlign.hpp"
#include <math.h>
#include <iostream>

/**
 * NWAligner impliments Needleman-Wuncsh global sequence alignment.
 * The alignment system is non-symbolic in this case, and uses 
 * real-valued inputs. The cost for aligning two locations is the
 * distance between them, thus alignments between the same values
 * is free. 
 * 
 * @author John Doucette
 * @date June 11th 2010
 */

double NWAligner::NWAlign(vector<double> &v1, vector<double> &v2){
	
	//this can be made a lot faster by only storing the current and previous rows/columns.
	//Could also declare as static it we aren't planning to multi-thread for similair speed boost.
	double D[v1.size()+1][v2.size()+1];
	
	//fill the DP matrix
	for(int i = v1.size(); i>= 0; i--){
		for(int j = v2.size(); j >= 0; j--){
			//initial conditions
			if(i==(long int)v1.size() && j== (long int)v2.size())
				D[i][j] = 0;
			else if(j== (long int)v2.size())
				D[i][j] = D[i+1][j] + this->gap_cost;
			else if(i== (long int)v2.size())
				D[i][j] = D[i][j+1] + this->gap_cost;

			// compute optimal path to D[i][j]
			else{
				double g1,g2,g3;
				g1 = D[i+1][j+1] + fabsf(v1[i] - v2[j]);
				g2 = D[i+1][j] + gap_cost;
				g3 = D[i][j+1] + gap_cost;
				D[i][j] = g1 < g2 ? (g1 < g3? g1 : g3) : (g2 < g3 ? g2 : g3); //messy, but it works.
			}
		
		}
	}
	
	//uncomment for debug op
	//this->print(&D[0][0], v2, v1, v1.size() + 1, v2.size() + 1);
	
	return D[0][0];

}

double NWAligner::NWAlign(vector<int> &v1, vector<int> &v2){

	//this can be made a lot faster by only storing the current and previous rows/columns.
	//Could also declare as static it we aren't planning to multi-thread for similair speed boost.
	double D[v1.size()+1][v2.size()+1];

	//fill the DP matrix
	for(int i = v1.size(); i>= 0; i--){
		for(int j = v2.size(); j >= 0; j--){
			//initial conditions
			if(i==(long int)v1.size() && j== (long int)v2.size())
				D[i][j] = 0;
			else if(j== (long int)v2.size())
				D[i][j] = D[i+1][j] + this->gap_cost;
			else if(i== (long int)v2.size())
				D[i][j] = D[i][j+1] + this->gap_cost;

			// compute optimal path to D[i][j]
			else{
				double g1,g2,g3;
				g1 = D[i+1][j+1] + fabsf(v1[i] - v2[j]);
				g2 = D[i+1][j] + gap_cost;
				g3 = D[i][j+1] + gap_cost;
				D[i][j] = g1 < g2 ? (g1 < g3? g1 : g3) : (g2 < g3 ? g2 : g3); //messy, but it works.
			}

		}
	}

	//uncomment for debug op
	//this->print(&D[0][0], v2, v1, v1.size() + 1, v2.size() + 1);

	return D[0][0];

}

NWAligner::NWAligner(double gc){
	this->gap_cost = gc;
	if(gap_cost <0)
		cerr << "Warning: Gap cost in sequence alignment is less than zero. All allignments will contain maximum gaps!" << endl; 

}


//Assumes square matrix for input, hence only 1 dim length needed for each.
double NWAligner::align_genomes(double *d1, double *d2, int l1, int l2){
	vector<double> v1;
	for(int i = 0; i < l1; i++)
		for(int j = 0; j < l1; j++)
			v1.push_back(d1[i*l1+j]);
	
	vector<double> v2;
	for(int i = 0; i < l2; i++)
		for(int j = 0; j < l2; j++)
			v2.push_back(d2[i*l2+j]);
	
	return this->NWAlign(v1,v2); //Need a wrapper (rational?) if we want to use this as a fitness measure. e.g. 1/(1+x)
}

//Note: For multi-dimensional phenotypic representations (i.e. > 2), break into 2-d segments, and make multiple calls.
//There is no way to make sense of a gap between sequences, and the algorithm does nothing to check for such a thing.
//Consider writing a wrapper method to deal with the splitting?
double NWAligner::align_behaviors(double b1[], double b2[], int l1, int l2){
	vector<double> v1;
	for(int i = 0; i < l1; i++)
		v1.push_back(b1[i]);

	vector<double> v2;
	for(int i = 0; i < l2; i++)
		v2.push_back(b2[i]);

	return this->NWAlign(v1,v2); // as above, need a wrapper.
}

void NWAligner::print(double *D, vector<double> &v1, vector<double> &v2, int l1, int l2){
	cout << "   ";
	for(int i = 0; i < l1; i++){
		if(v1[i] < 10)
                	cout << " ";
                cout << v1[i] << " ";
	}
	cout << endl;

	for(int i =0;i < l1; i++){
		if(v2[i] < 10)
                	cout << " ";
                cout << v2[i] << " ";

		for(int j = 0; j < l2; j++){
			if(D[i*l1 +j] < 10)
				cout << " ";
			cout << D[i*l1 + j] << " ";
		}
		cout << endl;
	} 

}
