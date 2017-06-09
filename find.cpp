g++ -std=c++11 find.cpp && ./a.out
// g++ -std=c++11 find.cpp && ./a.out
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <chrono>
using namespace std;

class Image{
public:
	const int MAX = 10000;
	int W,H;
	pair<int,int> upleft, downright;
	vector<vector<int>> data;
	vector<vector<bool>> visited;

	bool readdata(string filename){
		string str;
		ifstream fin(filename);
		if(fin){
			fin >> str; fin.ignore();
			if(str != "P2") cout << "file format error" << endl;
			getline(fin, str); // コメント読み捨て

			// data領域の確保
			fin >> W >> H; 
			data.resize(H);
			for(int i=0; i<H; i++){
				data[i].resize(W);
			}
			fin >> str; //255
			int h=0, w=0;
			while(fin >> str){
				data[h][w] = stoi(str);
				w	= (w+1) % W;
				if(w == 0) h++; 
			}
			makevisited();
			return true;
		}
		else return false;
	}

	// 未踏はfalse, 訪問済みtrue, 0が入っている場所はtrue
	void makevisited(){
		visited.resize(H);
		for(int i=0; i<H; i++){
			visited[i].resize(W);
		}

		for(int h=0; h<H; h++){
			for(int w=0; w<W; w++){
				visited[h][w] = false;
				if(data[h][w] == 0){
					visited[h][w] = true;
				}
			}
		}
	}

	// 左上端と右下端座標を計算
	void triming(int h, int w){
		queue< pair<int,int> > que;
		int dh[4] = {1,0,-1,0};
		int dw[4] = {0,1,0,-1};
		que.push({h,w});
		visited[h][w] = true;
		int max_h=0, min_h=MAX, max_w=0, min_w=MAX;
		while(que.size()){
			int h2 = que.front().first; 
			int w2 = que.front().second; que.pop();
			max_h = max(max_h, h2);
			min_h = min(min_h, h2);
			max_w = max(max_w, w2);
			min_w = min(min_w, w2);
			for(int i=0; i<4; i++){
				int h3 = h2 + dh[i];
				int w3 = w2 + dw[i];
				if(h3>0 && h3<H && w3>0 && w3<W){
					if(! visited[h3][w3]){
						que.push({h3,w3});
						visited[h3][w3] = true;
					}
				}
			}
		}
		downright.first = max_h;
		downright.second = max_w;
		upleft.first = min_h;
		upleft.second = min_w;
	}

};

int main(){
	auto start = std::chrono::system_clock::now();

	Image target;
	target.readdata("images/images1/image.pgm");
	
	vector<Image> templates;
	for(int i=0; 1;i++){
		templates.resize(i+1);
		if(! templates[i].readdata("images/images1/template"+to_string(i+1)+".pgm")){
			templates.resize(i);
			break;
		}
	}

	// templatesの切り出し
	for(int i=0; i<templates.size(); i++)
		for(int h=0; h<templates[i].H; h++)
			for(int w=0; w<templates[i].W; w++)
				if(! templates[i].visited[h][w])
					templates[i].triming(h,w);

	// targetの走査
	for(int h=0; h<target.H; h++)
		for(int w=0; w<target.W; w++){
			if(! target.visited[h][w]){
				target.triming(h,w);
				vector<int> diff;
				diff.resize(templates.size());
				for(int i=0; i<templates.size(); i++){
					int max_h_tgt = target.downright.first;
					int min_h_tgt = target.upleft.first;
					int max_w_tgt = target.downright.second;
					int min_w_tgt = target.upleft.second;
					int max_h_tmp = templates[i].downright.first;
					int min_h_tmp = templates[i].upleft.first;
					int max_w_tmp = templates[i].downright.second;
					int min_w_tmp = templates[i].upleft.second;				
					for(int dh=0; dh<max_h_tmp-min_h_tmp; dh++){
						for(int dw=0; dw<max_w_tmp-min_w_tmp; dw++){
							diff[i] += pow((templates[i].data[min_h_tmp+dh][min_w_tmp+dw] 
															- target.data[min_h_tgt+dh][min_w_tgt+dw]), 2);
						}
					}
				}
				auto min_diff = min_element(diff.begin(), diff.end());
				int ans_num = distance(diff.begin(), min_diff);
				int center_h = target.upleft.first + (14-templates[ans_num].upleft.first);
				int center_w = target.upleft.second + (14-templates[ans_num].upleft.second);
				cout << "template" << ans_num+1 << " " << center_h << " " << center_w << endl;
			}
		}

		auto end = std::chrono::system_clock::now();     
    auto dur = end - start;       
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << msec << " milli sec \n";

	return 0;
}
