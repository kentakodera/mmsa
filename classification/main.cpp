#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;


class NormalDistribution {
	public:
		vector<pair<double,double>> get_results(int avg_x,int dispersion_x,int avg_y,int dispersion_y,int loop_count){
			random_device seed_gen_x;
			random_device seed_gen_y;
			default_random_engine engine_x(seed_gen_x());
			default_random_engine engine_y(seed_gen_y());
			normal_distribution<> dist_x(avg_x,dispersion_x);
			normal_distribution<> dist_y(avg_y,dispersion_y);
			vector<pair<double,double>> results;
			for(int i=0;i<loop_count;i++){
				double x = dist_x(engine_x);
				double y = dist_y(engine_y);
				results.push_back(make_pair(x,y));
			}
			return results;
		}

		
};



int main(){
	NormalDistribution n_d;
	vector<pair<double,double>> results_class1 = n_d.get_results(30,5,30,10,100);
	vector<pair<double,double>> results_class2 = n_d.get_results(80,10,60,10,100);
	for(int i=0;i<results_class1.size();i++){
		cout << results_class1[i].first << "," << results_class1[i].second << ",";
		cout << results_class2[i].first << "," << results_class2[i].second << endl;
	}
}

