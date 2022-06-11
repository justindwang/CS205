#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <float.h>
#include <chrono>

using namespace std;

// Manual definitions for file and the row and column sizes in file
#define FILENAME "CS205_SP_2022_Largetestdata__13.txt"
#define ROWS 1000
#define COLS 41

// Function used in std::transform() for calculating Euclidean distance
double diff_square(double a, double b){
    return pow((a - b), 2);
}

// Cross validation function for calculating the accuracy when including a set a features
double cross_validation(const vector<vector<double> > &data, const vector<int> &include_cols){

    // Makes the column of unconsidered features 0
    vector<vector<double> > temp(ROWS,vector<double>(COLS,0));
    for (int row = 0; row < ROWS; row++){
        for(int col : include_cols){
            temp[row][col] = data[row][col]; 
        }
    }
    int correctly_classified = 0;
    
    // iterates through all rows to calculate accuracy
    for(int i = 0; i < ROWS; i++){
        vector<double> to_classify = temp[i];
        double label = data[i][0];
        double nearest_dist = DBL_MAX;
        int neighbor_index = INT_MAX;
        double neighbor_label = 3.0;

        // iterates through all rows to find the nearest neighbor
        for(int k = 0; k < ROWS; k++){
            if(k!=i){
                vector<double> auxiliary;
                transform(to_classify.begin(),to_classify.end(),temp[k].begin(),back_inserter(auxiliary),diff_square);
                double distance = sqrt(accumulate(auxiliary.begin(),auxiliary.end(),0.0));
                if(distance < nearest_dist){
                    nearest_dist = distance;
                    neighbor_index = k;
                    neighbor_label = data[k][0];
                }
            }
        }
        if(int(label) == int(neighbor_label)){
            correctly_classified+=1;
        }
    }

    //returns truncated accuracy value
    return round(1.0*correctly_classified/ROWS * 10000.0)/10000.0;
    
}

// Forward selection search that outputs the trace, final accuracy, and final set of features
void search_feature_forward(const vector<vector<double> > &data){
    vector<int> features;
    double final_accuracy = 0.0;
    vector<int> final_set;

    // iterates through all features to find the final accuracy
    for(int i = 1; i < COLS; i++){
        int feature_to_add = 0;
        double best_accuracy = 0.0;

        // iterates through all subsets containing feature i to find the best accuracy of these sets
        for(int k = 1; k<COLS;k++){
            if(!count(features.begin(),features.end(),k)){
                vector<int> temp = features;
                temp.push_back(k);
                cout << "--Considering adding the " << k << " feature" << endl;
                double accuracy = cross_validation(data, temp);
                if(accuracy > best_accuracy){
                    best_accuracy = accuracy;
                    feature_to_add = k;
                }
            }      
        }
        features.push_back(feature_to_add);
        cout << "On level " << i << " I added feature " << feature_to_add << " to the current set, with best accuracy of " << best_accuracy << endl << endl;
        if(best_accuracy > final_accuracy){
            final_accuracy = best_accuracy;
            final_set = features;
        }
    }

    // outputs the final accuracy and final set of features
    cout << "The best set of features found using forward search is: ";
    for ( int a : final_set ) cout << a << ' ';
    cout << "with an accuracy of " << final_accuracy << endl;
}

// Backward elimination search that outputs the trace, final accuracy, and final set of features
void search_feature_backward(const vector<vector<double> > &data){
    vector<int> features(COLS-1);
    iota(features.begin(), features.end(),1);
    double final_accuracy = 0.0;
    vector<int> final_set;

    // iterates through all features to find the final accuracy
    for(int i = 1; i<COLS;i++){
        int feature_to_remove = 0;
        double best_accuracy = 0.0;

        // iterates through all subsets containing feature i to find the best accuracy of these sets
        for(int k = 1; k<COLS;k++){
            if(count(features.begin(),features.end(),k)){
                vector<int> temp = features;
                temp.erase(remove(temp.begin(), temp.end(),k), temp.end());
                cout << "--Considering removing the " << k << " feature" << endl;
                double accuracy = cross_validation(data, temp);
                if(accuracy > best_accuracy){
                    best_accuracy = accuracy;
                    feature_to_remove = k;
                }
            }      
        }
        features.erase(remove(features.begin(), features.end(),feature_to_remove), features.end());
        cout << "On level " << i << " I removed feature " << feature_to_remove << " from the current set, with best accuracy of " << best_accuracy << endl << endl;
        if(best_accuracy > final_accuracy){
            final_accuracy = best_accuracy;
            final_set = features;
        }
    }

    // outputs the final accuracy and final set of features
    cout << "The best set of features found using backward elimination is: ";
    for ( int a : final_set ) cout << a << ' ';
    cout << "with an accuracy of " << final_accuracy << endl;
}

// Helper function for determining the counts of labels in data
void printlabels( vector<vector<double> > &data){
    int num1 = 0;
    int num2 = 0;
    for ( vector<double> &v : data){
         if(int(v[0] == 1)){
             num1 += 1;
         }
         else{
             num2 += 1;
         }
     }
     cout << "Num 1's: " << num1 << endl;
     cout << "Num 2's: "<< num2 << endl;
}

// main function that reads in the file and performs either forward selection or backward elimination. The search was hardcoded here, but an interface selection system can be added in the future
int main(){
    auto begin = chrono::high_resolution_clock::now();
    fstream file;
    vector<vector<double> > data(ROWS,vector<double>(COLS));

    // Reads in file
    file.open(FILENAME,ios::in);
    if(file.is_open()){
        cout << "File correctly opened" << endl;
        while(!file.eof()){
            for(int row = 0; row < ROWS; row++){
                for (int col = 0; col<COLS;col++){
                    file >> data[row][col];
                }
            }
        }
    }
    else cout << "Unable to open file" << endl;
    file.close();

    // performs either forward selection or backward elimination
    //search_feature_forward(data);
    search_feature_backward(data);

    // calculates and prints cpu runtime
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Runtime: %.3f seconds.\n", elapsed.count() * 1e-9);

    return 0;
}
