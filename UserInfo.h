#pragma once

#include <string>
#include <vector>

using namespace std;

class UserInfo
{
public:
	string user;
	vector<string> wordlist;
	vector<string> wordseq;
	vector<vector<double>> markovTable;

	UserInfo();
	string GenerateString();
};