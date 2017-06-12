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
	int W, H, W_trim, H_trim;
	double S, S_trim;
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
};

Image balance(Image templates, double scale, int rot){
		static const double pi = 3.141592653589793;
		double theta = rot*pi/180;
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

int main(){
	// 時間計測開始
	auto start = std::chrono::system_clock::now();

	Image target;
	target.readdata("images/images4/image.pgm");
	
	vector<Image> templates;
	for(int i=0; 1;i++){
		templates.resize(i+1);
		if(! templates[i].readdata("images/images4/template"+to_string(i+1)+".pgm")){
			templates.resize(i);
			break;
		}
	}

	// targetの走査 オブジェクトを見つけるごとに最適なtemplateを探す
	for(int h=0; h<target.H; h++){
		for(int w=0; w<target.W; w++){
			if(! target.visited[h][w]){
				target.trimming(h,w);
				vector<int> diff_templates(templates.size());
				vector<Point> center_dist(templates.size());
				vector<pair<double, int>> scale_rot(templates.size());
				for(int i=0; i<templates.size(); i++){
					// templateの倍率は画素値の和から，角度は-90~90まで総当たり
					double scale = sqrt((double)target.S_trim / (double)templates[i].S);
					vector<int> diff_rots(180);
					for(int r=0; r<180; r++){
						Image temp = balance(templates[i], scale, r-90);
						// templateは1度trimmingできればよい 縮小により離れた点を読むことを防ぐ
						bool trimmed = false;
						for(int h_=0; h_<temp.H; h_++){
							for(int w_=0; w_<temp.W; w_++){
								if(! temp.visited[h_][w_] and ! trimmed){
									temp.trimming(h_, w_);
									center_dist[i] = {temp.H/2-temp.upleft.h, temp.W/2-temp.upleft.w};
									trimmed = true;
								}
							}
						}
						// 左上の点を合わせて比較	 範囲は小さい方に合わせる
						for(int dh=0; dh<min(target.H_trim,temp.H_trim); dh++){
							for(int dw=0; dw<min(target.W_trim, temp.W_trim); dw++){
								diff_rots[r] += pow((temp.data[temp.upleft.h+dh][temp.upleft.w+dw] 
																- target.data[target.upleft.h+dh][target.upleft.w+dw]), 2);
							}
						}
						// 比較した画素数で割り平均をとる
						diff_rots[r] /= min(target.H_trim,temp.H_trim)*min(target.W_trim, temp.W_trim);
					}
					// 1つのtemplateについてどの角度が最も一致するか決定
					auto diff_rots_min = min_element(diff_rots.begin(), diff_rots.end());
					int best_rot_num = distance(diff_rots.begin(), diff_rots_min);
					scale_rot[i] = {scale, best_rot_num -90};
					diff_templates[i] = diff_rots[best_rot_num];
				}
				// どのtemplateが最も一致するか決定
				auto diff_templates_min = min_element(diff_templates.begin(), diff_templates.end());
				int ans_num = distance(diff_templates.begin(), diff_templates_min);
				int center_h = target.upleft.h + (center_dist[ans_num].h);
				int center_w = target.upleft.w + (center_dist[ans_num].w);
				cout << "template" << ans_num+1 << "  " << center_w << "  " << center_h 
				     << " " << scale_rot[ans_num].second << "  " << scale_rot[ans_num].first << endl;
			}
		}
	}
	auto end = std::chrono::system_clock::now();     
  auto dur = end - start;       
  auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  std::cout << msec << " milli sec \n";
	return 0;
}
