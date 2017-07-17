#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
using namespace std;

class Image{
public:
  int W, H, label;
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


int main(){
  auto start = chrono::system_clock::now();

  const int TRAIN = 1000;
  vector<Image> train_data(TRAIN);
  for(int i=0; i<=9; i++){
    for(int j=0; j<TRAIN; j++){
      string file_name;
      stringstream ss;
      ss << setfill('0') << setw(5) << right << to_string(j+1) << ".pgm";
      ss >> file_name;
      if(train_data[j].readdata("train_img_pgm/"+to_string(i)+"/img"+file_name)){
        train_data[j].label = i;
      }
    }
  }

  auto end = chrono::system_clock::now();     
  auto dur = end - start;       
  auto msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
  std::cout << "finished reading train data " << msec << " milli sec \n";

  vector<Image> test_data(10000);
  for(int i=0; i<=9; i++){
    for(int j=0; j<10000; j++){
      string file_name;
      stringstream ss;
      ss << setfill('0') << setw(5) << right << to_string(j+1) << ".pgm";
      ss >> file_name;
      if(test_data[j].readdata("test_img_pgm/"+to_string(i)+"/img"+file_name)){
        test_data[j].label = i;
      }
    }
  }

  end = chrono::system_clock::now();     
  dur = end - start;       
  msec = chrono::duration_cast<chrono::milliseconds>(dur).count();
  std::cout << "finished test train data " << msec << " milli sec \n";

  const int K = 5;
  vector<pair<int,int> > result(10, make_pair(0, 0));
  for(int i=0; i<10; i++){
    vector<pair<int,int> > distance(TRAIN);
    for(int j=0; j<TRAIN; j++)
      distance[j] = make_pair(diff(train_data[j], test_data[i]), j);

    sort(distance.begin(), distance.end());
    for(int k=0; k<10; k++)
      cout << distance[k].first << " " << train_data[distance[k].second].label << endl;

    vector<int> vote(10, 0);
    for(int k=0; k<K; k++)
      vote[train_data[distance[k].second].label]++;
    
    for(int k=0; k<10; k++)
      cout << vote[k] << endl;


    int r_label = max_element(vote.begin(), vote.end()) - vote.begin(); 
    int t_label = test_data[i].label;
    cout << t_label << " " << r_label << endl;
    result[t_label].first++;
    if(t_label == r_label)
      result[t_label].second++;
    if(i%1000 == 0)
      cout << i << endl;
  }

  for(int i=0; i<10; i++){
    cout << result[i].first << " " << result[i].second << " " << (double)result[i].second/result[i].first << endl;
  }

  end = std::chrono::system_clock::now();     
  dur = end - start;       
  msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  std::cout << "finished test train data " << msec << " milli sec \n";

}