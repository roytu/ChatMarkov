/*
* ChatMarkov
* Simulate a chat based on previous chatlogs and Markov chains
*
* ThatSnail
* Email: thatsnail@gmail.com
* Web: http://www.thatsnail.blogspot.com/
*/

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <time.h>
#include <cmath>

#include "Message.h"
#include "UserInfo.h"
#include "Markov.h"

typedef unsigned int uint;
using namespace std;

//TODO: Add support for more formats
enum LOG_TYPE {NONE, AIM};

//Edit these to your liking
const string DIRECTORY = "chatlog-AIM.txt";
const LOG_TYPE LOGTYPE = AIM;

float getTime();
void cleanLog();
void cleanAIM();
void copyFile(string src,string dest);
void constructChatlog(vector<Message>* log);
void constructWordlist(string words,vector<string>* seq,vector<string>* list);
int getID(string word,vector<string>* list);
void makeMarkov(vector<vector<double>>* table, vector<string>* seq, vector<string>* list);
template<class T> void outputList(vector<T>* list);
template<class T> void outputTable(vector<vector<T>>* table);

void constructUserlist(vector<Message>* log,vector<string>* list);
void constructChatstrings(vector<Message>* strings,vector<Message>* log,vector<string>* users);
void constructUserInfo(vector<UserInfo>* userinfo, vector<string>* userlist, vector<Message>* chatstrings);

int c=clock();

vector<string> wordlist = vector<string>();
vector<string> wordseq = vector<string>();
vector<vector<double>> markovTable = vector<vector<double>>();

string testsubject="How are you?  \n  How are your families?  How is the weather? \n";

vector<string> userlist = vector<string>();
vector<Message> chatlog = vector<Message>();
vector<Message> chatstrings = vector<Message>();

vector<UserInfo> userinfo = vector<UserInfo>();

void constructChatlog(vector<Message>* log)
{
	ifstream chat;
	chat.open("chatlog.txt");
	if(chat.is_open())
	{
		while(chat.good())
		{
			string line;
			getline(chat,line);

			if(line==""){continue;}
	
			string user="";
			for(uint i=0;i<line.size();i++)
			{
				char letter=line.at(i);
				if(letter==':')
				{
					break;
				}
				else
				{
					user+=letter;
				}
			}
	
			string text=string(line.begin()+user.size()+2,line.end());
	
			Message message=Message();
			message.user=user;
			message.text=text;
			chatlog.push_back(message);
		}
		chat.close();
	}
}
void constructWordlist(string words,vector<string>* seq,vector<string>* list)
{
	string word="";
	for(uint i=0;i<words.size();i++)
	{
		if(words[i]==' ')
		{
			seq->push_back(word);
			word="";
		}
		else
		{
			word+=words.at(i);
		}
	}
	if(word!=""){seq->push_back(word);}

	*list=*seq;
	sort(list->begin(),list->end());
	list->erase(unique(list->begin(),list->end()));
}
int getID(string word,vector<string>* list)
{
	for(uint i=0;i<list->size();i++)
	{
		if(list->at(i)==word)
		{
			return i;
		}
	}
	return -1;
}
void makeMarkov(vector<vector<double>>* table, vector<string>* seq, vector<string>* list)
{
	table->clear();
	table->resize(list->size(),vector<double>(list->size(),0));

	for(uint x=0;x<table->size();x++)
	{
		vector<double>* row=&table->at(x);
		string currentWord=list->at(x);
		if(currentWord=="\n"){continue;}

		double sum=0;
		for(uint i=0;i<seq->size()-1;i++)
		{
			if(seq->at(i)==currentWord)
			{
				string nextWord=seq->at(i+1);
				int c=getID(nextWord,list);
				sum+=++row->at(c);
			}
		}

		//normalize
		if(sum>0)
		{
			for(uint i=0;i<row->size();i++)
			{
				row->at(i)/=sum;
			}
		}
	}
}
template<class T> void outputList(vector<T>* list)
{
	for(uint i=0;i<list->size();i++)
	{
		cout<<list->at(i)<<" ";
	}
	cout<<endl;
}
template<class T> void outputTable(vector<vector<T>>* table)
{
	for(uint row=0;row<table->size();row++)
	{
		cout<<row<<" | ";
		for(uint col=0;col<table->at(row).size();col++)
		{
			cout<<table->at(row).at(col)<<" ";
		}
		cout<<endl;
	}
}

void constructUserlist(vector<Message>* log,vector<string>* list)
{
	for(uint i=0;i<log->size();i++)
	{
		Message m=log->at(i);
		if(find(list->begin(),list->end(),m.user)==list->end())
		{
			list->push_back(m.user);
		}
	}
}
void constructChatstrings(vector<Message>* strings,vector<Message>* log,vector<string>* users)
{
	for(uint i=0;i<users->size();i++)
	{
		string currentUser = users->at(i);
		string userText = "";
		for(uint j=0;j<log->size();j++)
		{
			Message m=log->at(j);
			if(m.user==currentUser)
			{
				userText+=m.text+" \n ";
			}
		}
		Message r = Message();
		r.user=currentUser;
		r.text=userText;
		strings->push_back(r);
	}
}
void constructUserInfo(vector<UserInfo>* userinfo, vector<string>* userlist, vector<Message>* chatstrings)
{
	for(uint i=0;i<userlist->size();i++)
	{
		UserInfo u = UserInfo();
		u.user=userlist->at(i);
		constructWordlist(chatstrings->at(i).text,&u.wordseq,&u.wordlist);
		cout<<endl<<"  Constructing Markov table...";
		makeMarkov(&u.markovTable,&u.wordseq,&u.wordlist);
		cout<<" Done!"<<endl;
		userinfo->push_back(u);
	}
}

void copyFile(string src,string dest)
{
	FILE* fsrc=fopen(src.c_str(),"rb");
	FILE* fdest=fopen(dest.c_str(),"wb");

	char buf[BUFSIZ];
	size_t size;

	while(size=fread(buf,1,BUFSIZ,fsrc))
	{
		fwrite(buf,1,size,fdest);
	}
	fclose(fsrc);
	fclose(fdest);
}

void cleanLog()
{
	copyFile(DIRECTORY,"chatlog.txt");
	switch(LOGTYPE)
	{
	case NONE:
		break;
	case AIM:
		cleanAIM();
		break;
	}
}
void cleanAIM()
{
	string filestr="";

	fstream fsrc;
	fsrc.open("chatlog.txt");
	if(fsrc.is_open())
	{
		while(fsrc.good())
		{
			string line;
			getline(fsrc,line);
			filestr+=line;
		}
	}
	fsrc.close();
	fsrc.open("chatlog.txt",ios_base::in|ios_base::out|ios_base::trunc);

	size_t pos=0;
	while(min(filestr.find("<tr><td class=\"remote\">",pos),filestr.find("<tr><td class=\"local\">",pos))!=string::npos)
	{
		size_t rmf=filestr.find("<tr><td class=\"remote\">",pos);
		size_t lof=filestr.find("<tr><td class=\"local\">",pos);
		if(rmf==string::npos){rmf=INT_MAX;}
		else{rmf+=23;}
		if(lof==string::npos){lof=INT_MAX;}
		else{lof+=22;}
		pos=min(rmf,lof);
		if(pos==string::npos)
		{
			break;
		}
		string user;
		user=string(filestr.begin()+pos,filestr.begin()+filestr.find("&#160;",pos));
		pos=filestr.find(">",pos)+1;
		while(filestr.at(pos)=='<')
		{
			pos=filestr.find(">",pos)+1;
		}
		string message;
		message=string(filestr.begin()+pos,filestr.begin()+filestr.find("</",pos));
		pos=filestr.find("</tr>",pos)+5;
		
		string res=user+": "+message+"\n";
		fsrc.write(res.c_str(),res.size());
	}

	fsrc.close();
}

float getTime()
{
	c=clock()-c;
	return (float)c/CLOCKS_PER_SEC;
}

int main()
{
	srand((unsigned int)time(NULL));

	getTime();
	cout<<"Initializing..."<<endl;
	cout<<" Done!"<<"("<<getTime()<<" secs)"<<endl;

	cout<<"Cleaning logs...";
	cleanLog();
	cout<<" Done!"<<"("<<getTime()<<" secs)"<<endl;

	cout<<"Constructing chatlog...";
	constructChatlog(&chatlog);
	cout<<" Done!"<<"("<<getTime()<<" secs)"<<endl;

	cout<<"Constructing userlist...";
	constructUserlist(&chatlog,&userlist);
	cout<<" Done!"<<"("<<getTime()<<" secs)"<<endl;

	cout<<"Constructing chatstrings...";
	constructChatstrings(&chatstrings,&chatlog,&userlist);
	cout<<" Done!"<<"("<<getTime()<<" secs)"<<endl;

	cout<<"Constructing userinfo...";
	constructUserInfo(&userinfo,&userlist,&chatstrings);
	cout<<" Done!"<<"("<<getTime()<<" secs)"<<endl;

	for(uint i=0;i<50;i++)
	{
		UserInfo ui=userinfo.at(rand()%userinfo.size());
		string m=ui.GenerateString();
		if(m!="")
		{
			cout<<m<<endl;
		}
	}

	system("pause");
	return 0;
}