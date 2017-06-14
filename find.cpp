// g++ -O3 -std=c++11 find.cpp && ./a.out
#define REMOVE_NOISE_WHITE_THRESHOLD 192
#define REMOVE_NOISE_BLACK_THRESHOLD 64


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <chrono>
using namespace std;

typedef struct{
	int h;
	int w;
}Point;

typedef struct{
	int template_num;
	int diff;
	Point c_dist;
	double scale;
	int rot;
}Output;

class Image{
public:
	const int MAX = 10000;
	int W, H, W_trim, H_trim;
	double S, S_trim;
	Point upleft;
	vector<vector<int>> data;
	vector<vector<bool>> visited;

	// 画像の読み込み ファイルが存在しない場合falseを返す
	bool readdata(string filename){
		string str;
		ifstream fin(filename);
		if(fin){
			fin >> str; fin.ignore();
			if(str != "P2") cout << "file format error" << endl;
			getline(fin, str); // コメント読み捨て
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
	// 0が入っている場所はtrue, ついでに総面積S(=画素値の合計)も算出
	void makevisited(){
		visited.resize(H);
		for(int i=0; i<H; i++){
			visited[i].assign(W,false);
		}
		S = 0;
		for(int h=0; h<H; h++){
			for(int w=0; w<W; w++){
				if(data[h][w] == 0){
					visited[h][w] = true;
				}
				else{
					S += data[h][w];
				}
			}
		}
	}

	// 内接する四角の左上端と幅，オブジェクトの画素値の合計を計算
	void trimming(int h, int w){
		queue<Point> que;
		int dp[3] = {-1,0,1};
		que.push({h,w});
		visited[h][w] = true; 
		S_trim = data[h][w];
		int max_h=0, min_h=MAX, max_w=0, min_w=MAX;
		while(que.size()){
			Point p = que.front(); que.pop();
			max_h = max(max_h, p.h);
			min_h = min(min_h, p.h);
			max_w = max(max_w, p.w);
			min_w = min(min_w, p.w);
			for(int i=0; i<3; i++){
				for(int j=0; j<3; j++){
					Point np = {p.h + dp[i], p.w + dp[j]};
					if(np.h>=0 && np.h<H && np.w>=0 && np.w<W){
						if(! visited[np.h][np.w]){
							que.push(np);
							S_trim += data[np.h][np.w];
							visited[np.h][np.w] = true;
						}
					}
				}
			}
		}
		upleft.h = min_h;
		upleft.w = min_w;
		H_trim = max_h - min_h;
		W_trim = max_w - min_w;
	}

	// 画素情報と周辺平均を投げると条件によってノイズの画素を綺麗にする
	void remove_noise_pixel(vector<vector<int>>& nonoise_data,int h,int w,int peripheral_ave){
		if(data[h][w] == 0 && peripheral_ave > REMOVE_NOISE_WHITE_THRESHOLD){
			nonoise_data[h][w] = 255;
		}
		else if(data[h][w] == 255 && peripheral_ave < REMOVE_NOISE_BLACK_THRESHOLD){
			nonoise_data[h][w] = 0;
		}else{
			nonoise_data[h][w] = data[h][w];
		}
	
	}

	// ノイズ除去
	void remove_noise(){
		int peripheral_ave = 0;

		vector<vector<int>> nonoise_data;
		nonoise_data.resize(H);
		for(int i=0; i<H; i++){
			nonoise_data[i].resize(W);
		}
		
		for(int h=0; h<H; h++){
			for(int w=0; w<W; w++){
				// 四辺上の端は周辺1画素ずつは取れない(はみ出す)ので対策
				if(h == 0){
					// 上辺
					if(w==0){
						peripheral_ave = (data[h][w+1] + data[h+1][w] + data[h+1][w+1]) / 3;
					}
					if(w==W){
						peripheral_ave = (data[h][w-1] + data[h+1][w-1] + data[h+1][w]) / 3;
					}else{
						peripheral_ave = (data[h][w-1] + data[h][w+1] + data[h+1][w-1] + data[h+1][w] + data[h+1][w+1]) / 5;
					}
					remove_noise_pixel(nonoise_data,h,w,peripheral_ave);
				}else if(h == H-1){
					// 下辺
					if(w==0){
						peripheral_ave = (data[h-1][w] + data[h-1][w+1] + data[h][w+1]) / 3;
					}
					if(w==W){
						peripheral_ave = (data[h-1][w-1] + data[h-1][w] + data[h][w-1]) / 3;
					}else{
						peripheral_ave = (data[h-1][w-1] + data[h-1][w] + data[h-1][w+1] + data[h][w-1] + data[h][w+1]) / 5;
					}
					remove_noise_pixel(nonoise_data,h,w,peripheral_ave);
				}else if(w == 0){
					// 左辺
					peripheral_ave = (data[h-1][w] + data[h-1][w+1] + data[h][w+1] + data[h+1][w] + data[h+1][w+1]) / 5;
					remove_noise_pixel(nonoise_data,h,w,peripheral_ave);
				}else if(w == W-1){
					// 右辺
					peripheral_ave = (data[h-1][w-1] + data[h-1][w] + data[h][w-1] + data[h+1][w-1] + data[h+1][w]) / 5;
					remove_noise_pixel(nonoise_data,h,w,peripheral_ave);
				}
				// 隣接周辺画素がすべて取れるエリア
				else if(data[h][w] == 0){
					// 黒画素
					peripheral_ave = (data[h-1][w-1] + data[h-1][w] + data[h-1][w+1] + data[h][w-1] + data[h][w+1] + data[h+1][w-1] + data[h+1][w] + data[h+1][w+1]) / 8;
					if(count_black(h,w) < 4){
						nonoise_data[h][w] = 255;
					}else if(peripheral_ave > REMOVE_NOISE_WHITE_THRESHOLD){
						nonoise_data[h][w] = 255;
					}else{
						nonoise_data[h][w] = data[h][w];
					}
				}else if(data[h][w] == 255){
					// 白画素
					peripheral_ave = (data[h-1][w-1] + data[h-1][w] + data[h-1][w+1] + data[h][w-1] + data[h][w+1] + data[h+1][w-1] + data[h+1][w] + data[h+1][w+1]) / 8;
					if(count_white(h,w) < 4){
						nonoise_data[h][w] = 0;
					}else if(peripheral_ave < REMOVE_NOISE_BLACK_THRESHOLD){
						nonoise_data[h][w] = 0;
					}else{
						nonoise_data[h][w] = data[h][w];
					}
				}else{
					nonoise_data[h][w] = data[h][w];
				}
			}
		}
		data = nonoise_data;
	}
	int count_black(int h,int w){
		// クソコード
		int c = 0;
		if(data[h-1][w-1] < 15){c++;}
		if(data[h-1][w] < 15){c++;}
		if(data[h-1][w+1] < 15){c++;}
		if(data[h][w-1] < 15){c++;}
		if(data[h][w+1] < 15){c++;}
		if(data[h+1][w-1] < 15){c++;}
		if(data[h+1][w] < 15){c++;}
		if(data[h+1][w+1] < 15){c++;}
		return c;
	}
	int count_white(int h,int w){
		// クソコード
		int c = 0;
		if(data[h-1][w-1] > 192){c++;}
		if(data[h-1][w] > 192){c++;}
		if(data[h-1][w+1] > 192){c++;}
		if(data[h][w-1] > 192){c++;}
		if(data[h][w+1] > 192){c++;}
		if(data[h+1][w-1] > 192){c++;}
		if(data[h+1][w] > 192){c++;}
		if(data[h+1][w+1] > 192){c++;}
		return c;
	}
	void writedata(string filename){
		string str;
		ofstream fout(filename);
		fout << "P2" << endl;
		fout << "# created by find.cpp" << endl;
		fout << W << " " << H << endl; 
		fout << 255 << endl;
		for(int h=0; h<H; h++){
			for(int w=0; w<W;w++){
				fout << data[h][w] << " ";
			}
			fout << endl;
		}
	}

	Image get_binarized_image(){
		// 二値化　まだ雑
		Image bin_image;
		bin_image.W = W;
		bin_image.H = H;
		bin_image.data.resize(H);
		for(int i=0; i<H; i++){
			bin_image.data[i].resize(W);
		}
		for(int h=0;h<H;h++){
			for(int w=0;w<W;w++){
				if(data[h][w] > 128){
					bin_image.data[h][w] = 255;
				}else{
					bin_image.data[h][w] = 0;
				}
			}
		}
		return bin_image;
	}

	void data_resize(){
		data.resize(H);
		for(int i=0; i<H; i++){
			data[i].resize(W);
		}
	}
	void data_resize(int height,int width){
		H = height;
		W = width;
		data.resize(height);
		for(int i=0; i<H; i++){
			data[i].resize(width);
		}
	}


};

// 入力画像をscale倍しrot度回転させた画像を返す
Image balance(Image templates, double scale, int rot){
		double theta = rot*M_PI/180;
		Image temp;
		temp.H = templates.H * scale;
		temp.W = templates.W * scale;
		// 28*28のオリジナルtemplate画像から画素を持ってくるので拡大倍率によらず中心は常に(14,14)
		Point c = {templates.H/2, templates.W/2};

		temp.data.resize(temp.H);
		for(int i=0; i<temp.H; i++)
			temp.data[i].resize(temp.W);

		//画像中心を中心として回転
		for(int h=0; h<temp.H; h++){
			for(int w=0; w<temp.W; w++){
				double h_s = h/scale;
				double w_s = w/scale;
				int h_r = cos(-theta)*(h_s-c.h) - sin(-theta)*(w_s-c.w) + c.h;
				int w_r = sin(-theta)*(h_s-c.h) + cos(-theta)*(w_s-c.w) + c.w;
				if(h_r >= templates.H) h_r = templates.H-1;
				if(w_r >= templates.W) w_r = templates.W-1;
				if(h_r < 0) h_r = 0;
				if(w_r < 0) w_r = 0;
				temp.data[h][w] = templates.data[h_r][w_r];
			}
		}
		temp.makevisited();
		return temp;
}

