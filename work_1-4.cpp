#include "find.cpp"

int main(){
	// 時間計測開始
	auto start = std::chrono::system_clock::now();

	Image target;
	target.readdata("images/images4/image.pgm");
	

	// template画像がある限り読み込み
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
				Output ans;
				ans.diff = 2147483647;
				Output now;
				for(int i=0; i<templates.size(); i++){
					now.template_num = i;
					// templateの倍率を画素値の和の比から求め，角度は-90~90まで総当たり
					now.scale = sqrt((double)target.S_trim / (double)templates[i].S);
					for(int r=0; r<180; r++){
						now.rot = r-90;
						Image temp = balance(templates[i], now.scale, now.rot);
						// templateは1度trimmingできればよい 縮小により離れた点を読むことを防ぐ
						bool trimmed = false;
						for(int h_=0; h_<temp.H; h_++){
							for(int w_=0; w_<temp.W; w_++){
								if(! temp.visited[h_][w_] and ! trimmed){
									temp.trimming(h_, w_);
									now.c_dist = {temp.H/2-temp.upleft.h, temp.W/2-temp.upleft.w};
									trimmed = true;
								}
							}
						}
						// 左上の点を合わせて比較	 範囲は小さい方に合わせる
						int diff = 0;
						Point range = {min(target.H_trim,temp.H_trim), min(target.W_trim, temp.W_trim)};
						for(int dh=0; dh<range.h; dh++){
							for(int dw=0; dw<range.w; dw++){
								int temp_pixel = temp.data[temp.upleft.h+dh][temp.upleft.w+dw];
								int target_pixel = target.data[target.upleft.h+dh][target.upleft.w+dw];
								// 1画素あたりの画素値の差の2乗を求める
								diff += pow((temp_pixel - target_pixel), 2) / (range.h * range.w) ;
							}
						}
						// diffが小さい方を答えとする
						if(diff < ans.diff){	
							now.diff = diff;	
							ans = now;
						}
					}
				}
				int center_h = target.upleft.h + (ans.c_dist.h);
				int center_w = target.upleft.w + (ans.c_dist.w);
				cout << "template" << ans.template_num+1 << "\t" << center_w << "\t" << center_h 
				     << "\t" << ans.rot << "\t" << ans.scale << endl;
			}
		}
	}
	auto end = std::chrono::system_clock::now();     
    auto dur = end - start;       
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << msec << " milli sec \n";
	return 0;
}
