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

const char* input_file = "input.flp";
const char* output_file = "sample.out";

// TO represent infinity in double 
double INF = 1.0e100;

// Sturucture to represent BTree
struct btree
{
	btree* left;
	btree* right;
	int node_no;
	vector<pair<int,int> > node;
	btree(){left=NULL; right=NULL;}
};

// This function creates a random binary tree from the vector of given nodes
btree* create_random_btree(vector<vector<pair<int,int> > > &all_list){
	int l = all_list.size();
	if(l==0)
		return NULL;
	if(l==1)
	{
		btree* root=new btree;
		root->node = all_list[0];
		return root;
	}
	random_shuffle(all_list.begin(),all_list.end());
	int r = rand()%l;
	btree* root=new btree;
	root->node = all_list[r];
	vector<vector<pair<int,int> > > le ,ri;
	for (int i = 0; i < l; ++i)
	{
		if(i<r) le.push_back(all_list[i]);
		if(i>r) ri.push_back(all_list[i]);
	}
	root->left=create_random_btree(le); 
	root->right=create_random_btree(ri);
	return root; 
}

// Function for vertical_node_sizing
vector<pair<int,int> > vertical_node_sizing(vector<pair<int,int> > &a,vector<pair<int,int> > &b){
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
vector<pair<int,int> > horizontal_node_sizing(vector<pair<int,int> > &a,vector<pair<int,int> > &b){
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

// Helper functions for calc_area function , does the calcualtion recursively
vector<pair<int,int> > calc_area(btree* root , vector<pair<int,int> > H){
	if(root == NULL)
		return H;
	if(root->left !=NULL){
		H = vertical_node_sizing(H,root->left->node);
		H = calc_area(root->left , H);
	}
	//cout<<H.size()<<" & "<<endl; // DEBUG
	if(root->right != NULL){
		H = horizontal_node_sizing(H,root->right->node);
		H = calc_area(root->right , H);
	}
	//cout<<H.size()<<" : "<<endl; // DEBUG
	return H;
}

// The function to calculate the area , given a random Btree
double calc_area(btree* root){
	if (root == NULL)
		return (double)INT_MAX;

	vector<pair<int,int> > H =calc_area(root , root->node);
	double min_area = INF;
	for (int i = 0; i < H.size(); ++i)
	{
		double area = (double)H[0].first * (double)H[0].second;
		if(area<min_area)
			min_area = area;
	}
	return min_area;

}

// Main function which does the simulated annealing
btree* simulated_annealing(vector<vector<pair<int,int> > > &all_list){
	double t = 1e50; // Initial Temperature
	double t_min = 1e-150; // Minimum temparature, Modify it for threshold
	
	btree* final_node = NULL;
	double old_area = INF;

	while(t>t_min){
		btree* root= create_random_btree(all_list);
		double new_area = calc_area(root);
		double c = new_area-old_area;
		if(c<0 || exp(-c/t) > 0.002){
			old_area = new_area;
			final_node = root;
		}

		t = t*0.9995;
		//cout<<t << " : " << old_area << " current was :"<<new_area<<endl;
		//cout<<t<<" : "<<t_min<<" "<<0.999995<<endl;
	}
	return final_node;

}

// Sorts pairs of x,y such that xi < xj and yi > yj if i<j. And removes those violating rules
void mysort(vector<pair<int,int> > &a){
	if(a.size()==0)
		return;
	vector<pair<int,int> > ret;
	sort(a.begin(),a.end());
	ret.push_back(a[0]);
	for (int i = 1,j=1; i < a.size(); ++i)
	{
		if(a[i].second > ret[j-1].second)
		{
			ret.push_back(a[i]);
			++j;
		}
	}
	a = ret;
}

vector<vector<pair<int,int> > > flp_input(){
	string s;
	char c;
	int n,temp;
	ifstream infile; 
   	infile.open(input_file); 
	infile>>s>>s>>s>>n;
	vector<vector<pair<int,int> > > all_list(n);
	c = '\0';
	while(c!=':')
		infile>>c;

	for (int i = 0; i < n; ++i)
	{
		vector<pair<int,int> > temp;
		pair<int,int> p;
		c='\0';
		while(c!=':'){
			if(c=='<'){
				infile>>p.first>>p.second;
				temp.push_back(p);
			}
			c=infile.get();
			if(c==-1){
				break;
			}
		}
		mysort(temp);
		all_list[i]=temp;
	}
	return all_list;
	infile.close();
}

// Main function
int main(int argc, char const *argv[])
{
	
	ofstream outfile;
	outfile.open(output_file);
	vector<vector<pair<int,int> > > all_list = flp_input();
	
	// To use the random numbers
	srand(unsigned(std::time(0)));
	
	btree* root = simulated_annealing(all_list);
	double least_area = calc_area(root);
	outfile<<"Total Area: "<<least_area<<endl;
	outfile.close();

	return 0;
}