#include "find.cpp"


int main(){
	// 時間計測開始
	auto start = std::chrono::system_clock::now();

	Image target;
	target.readdata("images/images2/image.pgm");
	target.remove_noise();
	target.makevisited();
 	target.writedata("noise_reduction.pgm");
	

	// template画像がある限り読み込み
	vector<Image> templates;
	for(int i=0; 1;i++){
		templates.resize(i+1);
		if(! templates[i].readdata("images/images2/template"+to_string(i+1)+".pgm")){
			templates.resize(i);
			break;
		}
	}

	// targetの走査 オブジェクトを見つけるごとに最適なtemplateを探す
	for(int h=0; h<target.H; h++){
		for(int w=0; w<target.W; w++){
			if(! target.visited[h][w]){
				target.trimming(h,w);
				if(target.H_trim < 6 || target.W_trim < 6){
					continue;
				}
				Output ans;
				ans.diff = 2147483647;
				ans.scale = 1;
				Output now;
				bool trimmed = false;
				for(int i=0; i<templates.size(); i++){
					for(int h_=0; h_<templates[i].H; h_++){ 
						for(int w_=0; w_<templates[i].W; w_++){ 
							if(! templates[i].visited[h_][w_]){ 
								templates[i].trimming(h_, w_); 
								now.c_dist = {templates[i].H/2-templates[i].upleft.h, templates[i].W/2-templates[i].upleft.w};
								trimmed = true; 
								break;
							} 
						} 
						if(trimmed){
							break;
						}
					} 
					now.template_num = i;
					int diff = 0;
					Point range = {min(target.H_trim,templates[i].H_trim), min(target.W_trim, templates[i].W_trim)};
					for(int dh=0; dh<range.h; dh++){
						for(int dw=0; dw<range.w; dw++){
							int temp_pixel = templates[i].data[templates[i].upleft.h+dh][templates[i].upleft.w+dw];
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

