#include "find.cpp"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>


int getdir (string dir, vector<string> &files)
{
	//ディレクトリ直下のファイル一覧を返すが，.と..が最初に含まれる
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}



int main(){
	// 時間計測開始
	auto start = std::chrono::system_clock::now();

	ofstream sum_out("summation_output.csv");
	for(int i=0;i<(256*28*28)/100;i++){
		sum_out << i << ",";
	}
	sum_out << endl;
	ofstream upper_ratio("sum_upper_ratio.csv");
	for(int i=0;i<100;i++){
		upper_ratio << i << ",";
	}
	upper_ratio << endl;
	ofstream left_ratio("sum_left_ratio.csv");
	for(int i=0;i<100;i++){
		left_ratio << i << ",";
	}
	left_ratio << endl;
	for(int d=0;d<10;d++){
		//ディレクトリループ
		string dir = string("images/get_characteristic_point/" + to_string(d));
		vector<string> files = vector<string>();
		getdir(dir,files);
		int summation[int((256*28*28)/100)] = {};
		int sum_upper_ratio[100] = {};
		int sum_left_ratio[100] = {};
		for (int i = 2;i < files.size();i++) {
		 	//ファイル全部ループ
		 	Image target;
		 	target.readdata(dir + "/" + files[i]);
		 	summation[int(target.S/100)]++;
			sum_upper_ratio[target.sum_upper_ratio()]++;
			sum_left_ratio[target.sum_left_ratio()]++;
		}
		for(int i=0;i<(256*28*28)/100;i++){
			sum_out << summation[i] << ",";
		}
		for(int i=0;i<100;i++){
			upper_ratio << sum_upper_ratio[i] << ",";
			left_ratio << sum_left_ratio[i] << ",";
		}
		sum_out << endl;
		upper_ratio << endl;
		left_ratio << endl;
	}



	auto end = std::chrono::system_clock::now();     
	auto dur = end - start;       
	auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
	std::cout << msec << " milli sec \n";
	return 0;
}

