#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cstdio>

using namespace std;

const char* InputFileName = "input.flp";
const char* OutputFileName = "sample.out";

// TO represent infinity in double 
double INFINITY_VAL = 1.0e100;

// Sturucture to represent Binary Tree
struct Binary_Tree
{
	Binary_Tree* LeftChild;
	Binary_Tree* RightChild;
	int ArrayVal_no;
	vector<pair<int,int> > ArrayVal;
	Binary_Tree(){LeftChild=NULL; RightChild=NULL;}
};

Binary_Tree* GenerateBinaryTree(vector<vector<pair<int,int> > > &AllNodes){
	int l = AllNodes.size();
	if(l==0)
		return NULL;
	if(l==1)
	{
		Binary_Tree* root=new Binary_Tree;
		root->ArrayVal = AllNodes[0];
		return root;
	}
	random_shuffle(AllNodes.begin(),AllNodes.end());
	int r = rand()%l;
	Binary_Tree* root=new Binary_Tree;
	root->ArrayVal = AllNodes[r];
	vector<vector<pair<int,int> > > left_vector ,right_vector;
	for (int i = 0; i < l; ++i)
	{
		if(i<r) left_vector.push_back(AllNodes[i]);
		if(i>r) right_vector.push_back(AllNodes[i]);
	}
	root->LeftChild=GenerateBinaryTree(left_vector); 
	root->RightChild=GenerateBinaryTree(right_vector);
	return root; 
}

// Function for VerticalDivision
vector<pair<int,int> > VerticalDivision(vector<pair<int,int> > &a,vector<pair<int,int> > &b){
	int s = a.size(),t = b.size();
	int i = 0 , j = 0 ;
	vector<pair<int,int> > H;
	while(i<s && j<t){
		pair<int,int> temp;
		temp.first = a[i].first + b[j].first;
		temp.second = max(a[i].second , b[j].second);
		H.push_back(temp);
		if(temp.second == a[i].second)
			i++;
		else
			j++;
	}
	return H;
}

// Function for horizontal node sizing
vector<pair<int,int> > HorizontalDivision(vector<pair<int,int> > &a,vector<pair<int,int> > &b){
	int s = a.size(),t = b.size();
	int i = s-1 , j = t-1;
	vector<pair<int,int> > H;
	while(i>=0 && j>=0){
		pair<int,int> temp;
		temp.second = a[i].second + b[j].second;
		temp.first = max(a[i].first , b[j].first);
		H.push_back(temp);
		if(temp.first == a[i].first)
			i--;
		else
			j--;
	}
	return H;
}

// Helper functions for CalculateArea function , does the calcualtion recursively
vector<pair<int,int> > CalculateArea(Binary_Tree* root , vector<pair<int,int> > H){
	if(root == NULL)
		return H;
	if(root->LeftChild !=NULL){
		H = VerticalDivision(H,root->LeftChild->ArrayVal);
		H = CalculateArea(root->LeftChild , H);
	}
	//cout<<H.size()<<" & "<<endl; // DEBUG
	if(root->RightChild != NULL){
		H = HorizontalDivision(H,root->RightChild->ArrayVal);
		H = CalculateArea(root->RightChild , H);
	}
	//cout<<H.size()<<" : "<<endl; // DEBUG
	return H;
}

// The function to calculate the area , given a random Binary_Tree
double CalculateArea(Binary_Tree* root){
	if (root == NULL)
		return INFINITY_VAL;

	vector<pair<int,int> > H =CalculateArea(root , root->ArrayVal);
	double MinimumArea = INFINITY_VAL;
	for (int i = 0; i < H.size(); ++i)
	{
		double area = (double)H[0].first * (double)H[0].second;
		if(area<MinimumArea)
			MinimumArea = area;
	}
	return MinimumArea;

}

// Main function which does the simulated annealing
Binary_Tree* SimulatedAnnealing(vector<vector<pair<int,int> > > &AllNodes){
	double Tinit = 1e50; // Initial Temperature
	double TMin = 1e-50; // Minimum temparature, Modify it for threshold
	
	Binary_Tree* ResultantNode = NULL;
	double CurrentArea = INFINITY_VAL;

	while(Tinit>TMin){
		Binary_Tree* root= GenerateBinaryTree(AllNodes);
		double CalculatedArea = CalculateArea(root);
		double Difference = CalculatedArea-CurrentArea;
		if(Difference<0 || exp(-Difference/Tinit) > 0.002){
			CurrentArea = CalculatedArea;
			ResultantNode = root;
		}

		Tinit = Tinit*0.95;
	}
	return ResultantNode;

}

// Sorts pairs of x,y such that xi < xj and yi > yj if i<j. And removes those violating rules
void CustomizedSort(vector<pair<int,int> > &a){
	if(a.size()==0)
		return;
	vector<pair<int,int> > ReturnVector;
	sort(a.begin(),a.end());
	ReturnVector.push_back(a[0]);
	for (int i = 1,j=1; i < a.size(); ++i)
	{
		if(a[i].second > ReturnVector[j-1].second)
		{
			ReturnVector.push_back(a[i]);
			++j;
		}
	}
	a = ReturnVector;
}

vector<vector<pair<int,int> > > ManualFileInput(){
	string s;
	char c;
	int n,temp;
	ifstream InputStream; 
   	InputStream.open(InputFileName); 
	InputStream>>s>>s>>s>>n;
	vector<vector<pair<int,int> > > AllNodes(n);
	c = 'X';
	while(c!=':')
		InputStream>>c;

	for (int i = 0; i < n; ++i)
	{
		vector<pair<int,int> > temp;
		pair<int,int> p;
		c='\0';
		while(c!=':'){
			if(c=='<'){
				InputStream>>p.first>>p.second;
				temp.push_back(p);
			}
			c=InputStream.get();
			if(c==-1){
				break;
			}
		}
		CustomizedSort(temp);
		AllNodes[i]=temp;
	}
	return AllNodes;
	InputStream.close();
}

// Main function
int main(int argc, char const *argv[])
{
	
	ofstream OutputStream;
	OutputStream.open(OutputFileName);
	vector<vector<pair<int,int> > > AllNodes = ManualFileInput();
	
	// To use the random numbers
	srand(unsigned(std::time(0)));
	
	Binary_Tree* root = SimulatedAnnealing(AllNodes);
	double least_area = CalculateArea(root);
	OutputStream<<"Total Area: "<<least_area<<endl;
	OutputStream.close();

	return 0;
}