#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <map>
#include <list>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

class Grani
{
public:
	char c;             // символ
	unsigned int key;   // ÷àñòîòíîñòü
	int left;           // ëåâàÿ ãðàíèöà
	int right;          // ïðàâàÿ ãðàíèöà
};

class MapKeys
{
public:
	map<char, int> mp;

	void CreateMap(ifstream& f)
	{
		// ñîçäàåì êîíòåéíåð map
		// ñèìâîë ïðåäñòàâëÿåòñÿ êàê èíäåêñ
		// è ïî ýòîìó èíäåêñó çíà÷åíèå ÿ÷åéêè óâåëè÷èâàåòñÿ íà 1
		// òî åñòü èùåì êîë-âî âõîæäåíèé ñèìâîëà â òåêñò (÷àñòîòíîñòü)
		for (char c = f.get(); c != EOF; c = f.get())
		{
			mp[c]++;
		}
		f.clear();
		f.seekg(0);   // âîçâðàùàåì óêàçàòåëü â íà÷àëî ôàéëà
	}

	void PrintMap()
	{
		// begin() è end() ýòî íà÷àëî è êîíåö êîíòåéíåðà map
		// ïðîõîäèìñÿ èòåðàòîðîì ïî êîíòåéíåðó è ïå÷àòàåì ñîäåðæèìîå
		// it->first - ïå÷àòàåì ñèìâîë char
		// it->second - ÷àñòîòíîñòü ñèìâîëà
		map<char, int>::iterator it;
		for (it = mp.begin(); it != mp.end(); it++)
		{
			cout << it->first << ": " << it->second << endl;
		}
	}
};

bool compare(Grani obj1, Grani obj2)
{
	if (obj1.key > obj2.key)
	{
		return true;
	}
	return false;
}







int main()
{
	ifstream fin("text.txt", ios::binary);      // èñõîäíûé òåêñò
	ofstream fout("encode.txt", ios::binary);   // ðåçóëüòàò êîäèðîâàíèÿ

	MapKeys maap;
	maap.CreateMap(fin);
	cout << "PrintMap:" << endl;
	maap.PrintMap();
	cout << "size = " << maap.mp.size() << endl << endl;

	list<Grani> spisok;
	map<char, int>::iterator itr;
	unsigned long long count = 0;



	// ïåðåïèñûâàåì ñîäåðæèìîå êîíòåéíåðà map â ñïèñîê
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)
	{
		Grani tmp;
		tmp.c = itr->first;      // it->first - ñèìâîë char
		tmp.key = itr->second;   // it->second - ÷àñòîòíîñòü
		count += tmp.key;
		spisok.push_back(tmp);   // ïîìåùàåì â êîíåö ñïèñêà
	}
	spisok.sort(compare);        // ñîðòèðóåì ñïèñîê (ïî óáûâàíèþ)



	// âûâîäèì ñîäåðæèìîå ñïèñêà
	cout << "spisok:" << endl;
	int temp = 0;
	for (auto i = spisok.begin(); i != spisok.end(); i++)
	{
		cout << i->c << " ~ ";

		i->left = temp;
		cout << "[" << i->left << "; ";

		i->right = temp + i->key;
		cout << i->right << ")" << endl;

		temp = i->right;
	}
	cout << endl;



	// âûïèñûâàåì êîíòåéíåð â ôàéë
	int k = maap.mp.size();
	fout.write((char*)&k, sizeof(k));   // çàïèñûâàåì êîëè÷åñòâî ñèìâîëîâ ýëåìåíòîâ
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)
	{
		fout.write((char*)&itr->first, sizeof(itr->first));     // ïèøåì ñèìâîë
		fout.write((char*)&itr->second, sizeof(itr->second));   // ïèøåì êîë-âî
	}



	list<Grani>::iterator gr;
	unsigned short l = 0;      // íèæíÿÿ ãðàíèöà
	unsigned short h = 65535;  // âåðõíÿÿ ãðàíèöà
	int delitel = spisok.back().right;   // back() - óêàçàòåëü íà ïîñëåäíèé ýëåìåíò
	unsigned short first = (h + 1) / 4;   //16384
	unsigned short half = first * 2;	  //32768
	unsigned short thride = first * 3;    //49152
	int bitsf = 0;   // ñêîëüêî áèòîâ ñáðàñûâàòü
	char tmp = 0;
	int ncount = 0;

	while (!fin.eof())
	{
		int c = fin.get();
		if (c == EOF)
		{
			break;
		}
		for (gr = spisok.begin(); (gr != spisok.end()) && (gr->c != c); gr++);
		int d = h - l + 1;
		h = l + gr->right * d / delitel - 1;  // âåðõíÿÿ ãðàíèöà
		l = l + gr->left * d / delitel;       // íèæíÿÿ ãðàíèöà

		while (1)
		{
			if (h < half)
			{
				ncount++;
				if (ncount == 8)
				{
					fout.put(tmp);
					tmp = 0;
					ncount = 0;
				}
				for (; bitsf > 0; bitsf--)
				{
					tmp = tmp | (1 << (7 - ncount));
					ncount++;
					if (ncount == 8)
					{
						fout.put(tmp);
						tmp = 0;
						ncount = 0;
					}
				}
			}
			else
			{
				if (l >= half)
				{
					tmp = tmp | (1 << (7 - ncount));
					ncount++;
					if (ncount == 8)
					{
						fout.put(tmp);
						tmp = 0;
						ncount = 0;
					}
					for (; bitsf > 0; bitsf--)
					{
						ncount++;
						if (ncount == 8)
						{
							fout.put(tmp);
							tmp = 0;
							ncount = 0;
						}
					}
					h -= half;
					l -= half;
				}
				else
				{
					if ((l >= first) && (h < thride))
					{
						bitsf++;
						h -= first;
						l -= first;
					}
					else
					{
						break;
					}
				}
			}

			h = h << 1;
			l = l << 1;
			h++;
		}
	}

	fin.close();
	fout.close();

	return 0;
}
