#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
using namespace std;

typedef struct{
  int h;
  int w;
}Point;

class Image{
private:
  const int MAX = 10000;

public:
  int W, H, label, W_trim, H_trim;
  double S=0;
  Point upleft;
  vector<vector<int>> data;

  bool readdata(string filename){
    string str;
    ifstream fin(filename);
    if(fin){
      fin >> str;
      fin.ignore();
      if(str != "P2") cout << "file format error" << endl;
      while(getline(fin, str), str[0]=='#'); // コメント読み捨て
      sscanf(str.data(), "%d %d", &W, &H);
      data_resize();
      fin >> str; //255
      int h=0, w=0;
      while(fin >> str){
        data[h][w] = stoi(str);
        w = (w+1) % W;
        if(w == 0) h++; 
      }

      trimming();
      return true;
    }
    else return false;
  }

  void data_resize(){
    data.resize(H);
    for(int i=0; i<H; i++){
      data[i].resize(W);
    }
  }

  void trimming(){
    int max_h=0, min_h=MAX, max_w=0, min_w=MAX;
    for(int h=0; h<H; h++){
      for(int w=0; w<W; w++){
        if(!data[h][w]){
          max_h = max(max_h, h);
          min_h = min(min_h, h);
          max_w = max(max_w, w);
          min_w = min(min_w, w);
          S += data[h][w];
        }
      }
    }
    upleft.h = min_h;
    upleft.w = min_w;
    H_trim = max_h - min_h;
    W_trim = max_w - min_w;
  }

  int grad_sum(int hs, int he, int ws, int we){
    int gs = 0;
    for(int h=hs; h<he; h++){
      for(int w=ws; w<we; w++){
        gs += data[h][w];
      }
    }
    return gs;
  }
};

// intで保持したいのでsqrtは取らない
int diff(Image train, Image test){
  int distance = 0;
  int H = train.H;
  int W = train.W;
  for(int h=0; h<H; h++){
    for(int w=0; w<W; w++){
      distance += (train.data[h][w] - test.data[h][w])*(train.data[h][w] - test.data[h][w]);
    }
  }
  return distance;
}

void print_time(chrono::system_clock::time_point start){
  auto end = chrono::system_clock::now();     
  auto dur = end - start;       
  auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
  cout << msec << " milli sec" << endl;
}
