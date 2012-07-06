#include "UserInfo.h"

#include "Markov.h"
#include <string>
#include <vector>

UserInfo::UserInfo()
{
	wordlist=vector<string>();
	wordseq=vector<string>();
	markovTable=vector<vector<double>>();
}
string UserInfo::GenerateString()
{
	int cw=rand()%wordlist.size();
	string res="";
	while(wordlist.at(cw)!="\n")
	{
		res+=wordlist.at(cw)+" ";
		cw=Markov<double>::randDistr(&markovTable.at(cw));
	}
	return user+": "+res;
}