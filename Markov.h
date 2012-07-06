#pragma once

#include <vector>

using namespace std;

template<class T> class Markov
{
public:
	static int randDistr(vector<T>* dist);
};

template<class T> int Markov<T>::randDistr(vector<T>* dist)
{
	int prec = 10000;

	int r=rand()%prec;
	int sum=0;
	for(int i=0;i<(int)dist->size();i++)
	{
		sum+=(int)(dist->at(i)*prec);
		if(r<sum)
		{
			return i;
			break;
		}
	}
	return 0;
}