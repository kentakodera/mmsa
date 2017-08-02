// k-NNのk
#define KNN_VAL 5
// 各クラスのデータを何個作るか
#define TOTAL_DATA_AMOUNT 100
// 各クラス何個づつ判定に用いるか
#define CLASSIFICATION_POINT 5

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



class KNN {
	private:
		vector<pair<double,double>> results_class1;
		vector<pair<double,double>> results_class2;
	public:
		KNN(vector<pair<double,double>> r_class1,vector<pair<double,double>> r_class2){
			results_class1 = r_class1;
			results_class2 = r_class2;
		}
		int classification_by_knn(){
			for(int i=TOTAL_DATA_AMOUNT - CLASSIFICATION_POINT;i<TOTAL_DATA_AMOUNT;i++){
				double x = results_class1[i].first;
				double y = results_class1[i].second;
				cout << "Class 1 Point at (" << x << "," << y << ") classified as Class " << point_classification_by_knn(x,y) << endl;
				x = results_class2[i].first;
				y = results_class2[i].second;
				cout << "Class 2 Point at (" << x << "," << y << ") classified as Class " << point_classification_by_knn(x,y) << endl;
			}
		}

		int point_classification_by_knn(double x,double y){
			// x,yの点に対してk-NN法でどっちに分類されるか返す
			// nearest_neighborを格納する配列
			// クラス，距離を格納，0,-1で初期化
			pair<int,double> nearest_neighbors[KNN_VAL];
			for(int c=0;c<KNN_VAL;c++){
				nearest_neighbors[c] = make_pair(0,-1);
			}

			// それぞれのクラスに対して総なめしてNNを指定数取る
			for(int l=0;l<TOTAL_DATA_AMOUNT - CLASSIFICATION_POINT;l++){
				double distance_c1 = pow(results_class1[l].first - x,2) + pow(results_class1[l].second - y,2);
				double distance_c2 = pow(results_class2[l].first - x,2) + pow(results_class2[l].second - y,2);
				for(int c=0;c<KNN_VAL;c++){
					// class 1のNNを取得
					if(nearest_neighbors[c].second < 0){
						nearest_neighbors[c] = make_pair(1,distance_c1);
						break;
					}
					if(nearest_neighbors[c].second > distance_c1){
						pair<int,double> tmp = nearest_neighbors[c];
						nearest_neighbors[c] = make_pair(1,distance_c1);
						if(c<KNN_VAL-2){
							nearest_neighbors[c+1] = tmp;
						}
						break;
					}
				}
				for(int c=0;c<KNN_VAL;c++){
					// class 2のNNを取得
					if(nearest_neighbors[c].second < 0){
						nearest_neighbors[c] = make_pair(1,distance_c2);
						break;
					}
					if(nearest_neighbors[c].second > distance_c2){
						pair<int,double> tmp = nearest_neighbors[c];
						nearest_neighbors[c] = make_pair(2,distance_c2);
						if(c<KNN_VAL-2){
							nearest_neighbors[c+1] = tmp;
						}
						break;
					}
				}
			}
			//Nearest neighbor格納済み
			int sum=0;
			for(int c=0;c<KNN_VAL;c++){
				sum += nearest_neighbors[c].first;
			}
			//2が多ければ2を，1が多ければ1を返す
			if(sum > (KNN_VAL * 3) / 2){
				return 2;
			}else{
				return 1;
			}
		}
};





int main(){
	NormalDistribution n_d;
	vector<pair<double,double>> results_class1 = n_d.get_results(30,5,30,10,TOTAL_DATA_AMOUNT);
	vector<pair<double,double>> results_class2 = n_d.get_results(80,10,60,10,TOTAL_DATA_AMOUNT);

	KNN knn(results_class1,results_class2);
	knn.classification_by_knn();

}

