// g++ -std=c++11 find.cpp && ./a.out
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <chrono>
using namespace std;

typedef struct {
	int h;
	int w;
}Point;

class Image{
public:
	const int MAX = 10000;
	int W, H, S, W_trim, H_trim, S_trim;
	Point upleft;
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

	// 未踏はfalse, 訪問済みtrue
	// 0が入っている場所はtrue, ついでに総面積Sも算出
	void makevisited(){
		visited.resize(H);
		for(int i=0; i<H; i++){
			visited[i].resize(W);
		}
		int S_=0;
		for(int h=0; h<H; h++){
			for(int w=0; w<W; w++){
				if(data[h][w] == 0){
					visited[h][w] = true;
					S_++;
				}
			}
		}
		S = H * W - S_;
	}

	// 左上端と右下端座標を計算
	void triming(int h, int w){
		queue<Point> que;
		int dh[4] = {1,0,-1,0};
		int dw[4] = {0,1,0,-1};
		que.push({h,w});
		visited[h][w] = true; 
		S_trim = 1;
		int max_h=0, min_h=MAX, max_w=0, min_w=MAX;
		while(que.size()){
			Point p = que.front(); que.pop();
			max_h = max(max_h, p.h);
			min_h = min(min_h, p.h);
			max_w = max(max_w, p.w);
			min_w = min(min_w, p.w);
			for(int i=0; i<4; i++){
				Point np = {p.h + dh[i], p.w + dw[i]};
				if(np.h>0 && np.h<H && np.w>0 && np.w<W){
					if(! visited[np.h][np.w]){
						que.push(np);
						S_trim++;
						visited[np.h][np.w] = true;
					}
				}
			}
		}
		upleft.h = min_h;
		upleft.w = min_w;
		H_trim = max_h - min_h;
		W_trim = max_w - min_w;
	}
};

Image balance(Image templates, double scale, int rot){
		Image temp;
		temp.H = templates.H * scale;
		temp.W = templates.W * scale;
		temp.data.resize(temp.H);
		for(int i=0; i<temp.H; i++){
			temp.data[i].resize(temp.W);
		}
		for(int h=0; h<temp.H; h++){
			for(int w=0; w<temp.W; w++){
				int h_ = static_cast<int>(h/scale+0.9);
				int w_ = static_cast<int>(w/scale+0.9);
				if(h_ >= templates.H) h_ = templates.H-1;
				if(w_ >= templates.W) w_ = templates.W-1;
				temp.data[h][w] = templates.data[h_][w_];
				
			}
		}

		temp.makevisited();
		return temp;
}

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


	// targetの走査
	for(int h=0; h<target.H; h++){
		for(int w=0; w<target.W; w++){
			if(! target.visited[h][w]){
				target.triming(h,w);
				vector<int> diff;
				diff.resize(templates.size());
				vector<int> diff(templates.size());
				for(int i=0; i<templates.size(); i++){
					for(int dh=0; dh<max_h_tmp-min_h_tmp; dh++){
						for(int dw=0; dw<max_w_tmp-min_w_tmp; dw++){
					// templateの大きさと角度の調整
					double scale = (double)target.S_trim / (double)templates[i].S;
					int rot=0;
					Image temp = balance(templates[i], scale, rot);
					for(int h_=0; h_<temp.H; h_++){
						for(int w_=0; w_<temp.W; w_++){
							if(! temp.visited[h_][w_]){
								temp.triming(h_, w_);
							}
						}
					}
					//左上の点を合わせて比較		
					for(int dh=0; dh<min(target.H_trim,temp.H_trim); dh++){
						for(int dw=0; dw<min(target.W_trim, temp.W_trim); dw++){
							diff[i] += pow((temp.data[temp.upleft.h+dh][temp.upleft.w+dw] 
															- target.data[target.upleft.h+dh][target.upleft.w+dw]), 2);
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
