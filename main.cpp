#include <iostream>
#include <fstream> 
#include <string> 
#include <vector>
#include <algorithm>
#define MAX 1001

using namespace std;
int matrix [MAX][MAX] = {0};

// longest proper prefix which is also suffix
vector<int> lps(string patron){
	int n = patron.length();
	vector<int> lpsv(n,0);
	int j=0, i =1;
	while (i < n){
		if (patron[i] == patron[j]){
			lpsv[i] = j+1;
			j++;
			i++;
		}
		else{
			if (j==0){
				lpsv[i] = 0;
				i++;
			}
			else{
				j = lpsv[j-1];
			}
		}
	}
	return lpsv;
}

// O(n)
vector<int> kmp(string texto, string patron){
	vector<int> posMatch;
	vector<int> lpsv = lps(patron);
	int j=0, i=0; // j ==> patrón,    i ==> texto
	int n=texto.length();
	int m=patron.length();
	while (i < n){
		if (texto[i] == patron[j]){
			j++;
			i++;
			if (j == m){
				posMatch.push_back(i-m);
				j = lpsv[j-1];
			}
		}
		else{
			if (j == 0){
				i++;
			}
			else{
				j = lpsv[j-1];
			}
		}
	}
	return posMatch;
}

void printKMP(vector<int> posMatch, string malicious, string fileName, ofstream &outfile){
    outfile << fileName << " ==> " << posMatch.size() << " veces" << endl;
    if (posMatch.size() == 0){
		outfile << "No hay coincidencias" << endl;
	}	
	else{
		for (int i=0; i<posMatch.size()-1; i++){
			outfile << posMatch[i] << ", ";
		}
        outfile << posMatch[posMatch.size()-1] << endl;
	}
}

string manacher(string text){
	string T = "";
	int n = text.length();
	for(int i=0; i<n; i++){
		T += "|";
		T += text[i];
	}
	T += "|";
	n = T.length();
	vector<int> L(n);
	L[0] = 0;
	L[1] = 1;
	int maxLong=0, maxCenter=0;
	bool exp;
	for (int c=1, li=0, ri=2; ri<n; ri++){
		li = c-(ri-c); //in case it saves position
		exp = false;
		if (c-maxLong <= ri && ri >= c+maxLong){
			if (L[li] < (c+L[c]-ri)){  // Case 1
				L[ri] = L[li];
			}else if (L[li] == (c+L[c]-ri) && (c+L[c]) == 2*n){ // Case 2
				L[ri] = L[li];
			}else if (L[li] == (c+L[li]-ri) && (c+L[c]) < 2*n){ // Case 3
				L[ri] = L[li];
				exp = true;
			}else if (L[li] > (c+L[c]-ri) && (c+L[c]) < 2*n){
				L[ri] = (c+L[c]-ri);
				exp = true;
			}
		}
		else{
			L[ri] = 0;
			exp = true;
		}
		if(exp){
			while((ri + L[ri] < n) && (ri-L[ri] > 0) && T[ri-L[ri]-1] == T[ri+L[ri]+1]){
				L[ri]++;
			}
		}
		c = ri;
		if (L[ri] > maxLong){
			maxLong = L[ri];
			maxCenter = ri;
		}
	}
	int start = (maxCenter-maxLong) / 2;
	string outp = "";
	for(int i=start; i< (start+maxLong); i++){
		outp += text[i];
	}
	return outp;
}

void printPalindrome(string palindrome, string text, int idx, ofstream &outfile){
    vector<int> posMatch = kmp(text, palindrome);
    outfile << "transmission" << idx << ".txt ==> Posición: ";
    for(int i=1; i<posMatch.size(); i++){
        outfile << posMatch[i-1] << ", ";
	}
    outfile << posMatch[posMatch.size()-1] << endl;
    outfile << palindrome << endl;
}

// O(n*m)
void solveLCSStr(string s1, string s2, ofstream &outfile){
    int maxi = 0, row, col;
    for(int i=0; i<s1.length(); i++){
        if(s1[i] == s2[0]){
            matrix[i][0] = 1; maxi = 1;
        }else{
            matrix[i][0] = 0;
        }
    }for(int j=0; j<s2.length(); j++){
        if(s2[j] == s1[0]){
            matrix[0][j] = 1; maxi = 1;
        }else{
            matrix[0][j] = 0;
        }
    }

    for(int i=1; i<s1.length(); i++){
        for(int j=1; j<s2.length(); j++){
            if(s1[i] == s2[j]){
                matrix[i][j]  = matrix[i-1][j-1]+1;
                //maxi = (matrix[i][j] > maxi ? matrix[i][j] : maxi);
                if(matrix[i][j] > maxi){
                    maxi = matrix[i][j];
                    row = i; col = j;
                }
            }else{
                matrix[i][j]  = 0;
            }
        }
    }
    char* lcsstring = (char*)malloc((maxi + 1) * sizeof(char));
    while(matrix[row][col] != 0) {
        lcsstring[--maxi] = s2[col - 1]; // or s2[col-1]
        // move diagonally up to previous cell
        row--;
        col--;
    }
    outfile << "LCS: " << lcsstring << endl;
    //return maxi;
}

int main(){
    ifstream mfile;
    ofstream outfile("checking.txt");
    string fileName, fileM, malicious, text;
    vector<string> texts;
    
    fileM = "mcode.txt";    //  a. malicious code KMP search
    mfile.open(fileM);
    while(!mfile.eof()){
        mfile >> malicious;
        //cout << "mcode: " << malicious << endl;
        outfile << "Código: " << malicious << endl;
        for(int i=1; i<=3; i++){
            ifstream infile;
            fileName = "transmission";
            fileName += to_string(i) + ".txt";
            infile.open(fileName);
            infile >> text;
            texts.push_back(text);
            //KMP 
            vector<int> posMatch = kmp(text, malicious);
            printKMP(posMatch, malicious, fileName, outfile);
            //outfile << text << endl;
            infile.close();
        }
    }
    mfile.close();
    
    outfile << "==============" << endl;
    outfile << "Palíndromo más grande: " << endl; // b. longest palindrome and its position
    for(int i=0; i<=2; i++){
        string palindrome = manacher(texts[i]);
        printPalindrome(palindrome, texts[i], i+1, outfile);
        if(i!=2){
            outfile << "----" << endl;
        }
    }
    outfile << "==============" << endl;
    outfile << "Substring más largo" << endl;
    outfile << "transmission1 and transmission2: ";
    solveLCSStr(texts[0].substr(0, 1000), texts[1].substr(0, 1000), outfile);
    //.substr(0, 1000)

    outfile.close();
    return 0;
};