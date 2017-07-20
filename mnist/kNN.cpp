#include "functions.cpp"

int main(){
  auto start = chrono::system_clock::now();

  const int TRAIN = 60000;
  vector<Image> train_data(TRAIN);
  for(int i=0; i<=9; i++){
    for(int j=0; j<TRAIN; j++){
      string file_name;
      stringstream ss;
      ss << setfill('0') << setw(5) << right << to_string(j+1) << ".pgm";
      ss >> file_name;
      if(train_data[j].readdata("images/train_img_pgm/"+to_string(i)+"/img"+file_name)){
        train_data[j].label = i;
      }
    }
  }
  cout << "finished reading train data" << endl;   
  print_time(start);

  const int TEST = 500;
  vector<Image> test_data(TEST);
  for(int i=0; i<=9; i++){
    for(int j=0; j<TEST; j++){
      string file_name;
      stringstream ss;
      ss << setfill('0') << setw(5) << right << to_string(j+1) << ".pgm";
      ss >> file_name;
      if(test_data[j].readdata("images/test_img_pgm/"+to_string(i)+"/img"+file_name)){
        test_data[j].label = i;
      }
    }
  }
  cout << "finished test train data " << endl;
  print_time(start);

  const int K = 7;
  vector<pair<int,int> > result(10, make_pair(0, 0));
  for(int i=0; i<test_data.size(); i++){
    vector<pair<int,int> > distance(TRAIN);
    for(int j=0; j<TRAIN; j++)
      distance[j] = make_pair(diff(train_data[j], test_data[i]), j);

    sort(distance.begin(), distance.end());
    // for(int k=0; k<10; k++)
    //   cout << distance[k].first << " " << train_data[distance[k].second].label << endl;

    vector<int> vote(10, 0);
    for(int k=0; k<K; k++)
      vote[train_data[distance[k].second].label]++;

    int r_label = max_element(vote.begin(), vote.end()) - vote.begin(); 
    int t_label = test_data[i].label;
    // cout << "ans:" << t_label << " result:" << r_label << endl;
    result[t_label].first++;
    if(t_label == r_label)
      result[t_label].second++;
    if(i%1000 == 0)
      cout << i << endl;
  }

  for(int i=0; i<10; i++){
    cout << result[i].first << " " << result[i].second << " " << (double)result[i].second/result[i].first << endl;
  }

  cout << "finished all" << endl;
  print_time(start);

  return 0;
}

