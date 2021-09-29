#include <iostream>
#include <fstream> 
#include <string> 
#include <vector>

using namespace std;

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

// Complejidad: O(n)
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

int main(){
    ifstream mfile;
    ofstream outfile("checking.txt");
    string fileName, fileM, malicious, text;
    vector<string> texts;
    
    fileM = "mcode.txt";    //  malicious code KMP search
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
    // longest palindrome and its position
    outfile << "==============" << endl;
    outfile << "Palíndromo más grande: " << endl;
    for(int i=0; i<=2; i++){
        string palindrome = manacher(texts[i]);
        printPalindrome(palindrome, texts[i], i+1, outfile);
        //cout << "longest palindrome: " << palindrome << endl;
        if(i!=2){
            outfile << "----" << endl;
        }
    }
    outfile << "==============" << endl;

    outfile.close();
    return 0;
};