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
	char c;             // ������
	unsigned int key;   // �����������
	int left;           // ����� �������
	int right;          // ������ �������
};

class MapKeys
{
public:
	map<char, int> mp;

	void CreateMap(ifstream& f)
	{
		// ������� ��������� map
		// ������ �������������� ��� ������
		// � �� ����� ������� �������� ������ ������������� �� 1
		// �� ���� ���� ���-�� ��������� ������� � ����� (�����������)
		for (char c = f.get(); c != EOF; c = f.get())
		{
			mp[c]++;
		}
		f.clear();
		f.seekg(0);   // ���������� ��������� � ������ �����
	}

	void PrintMap()
	{
		// begin() � end() ��� ������ � ����� ���������� map
		// ���������� ���������� �� ���������� � �������� ����������
		// it->first - �������� ������ char
		// it->second - ����������� �������
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
	ifstream fin("text.txt", ios::binary);      // �������� �����
	ofstream fout("encode.txt", ios::binary);   // ��������� �����������

	MapKeys maap;
	maap.CreateMap(fin);
	cout << "PrintMap:" << endl;
	maap.PrintMap();
	cout << "size = " << maap.mp.size() << endl << endl;

	list<Grani> spisok;
	map<char, int>::iterator itr;
	unsigned long long count = 0;



	// ������������ ���������� ���������� map � ������
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)
	{
		Grani tmp;
		tmp.c = itr->first;      // it->first - ������ char
		tmp.key = itr->second;   // it->second - �����������
		count += tmp.key;
		spisok.push_back(tmp);   // �������� � ����� ������
	}
	spisok.sort(compare);        // ��������� ������ (�� ��������)



	// ������� ���������� ������
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



	// ���������� ��������� � ����
	int k = maap.mp.size();
	fout.write((char*)&k, sizeof(k));   // ���������� ���������� �������� ���������
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)
	{
		fout.write((char*)&itr->first, sizeof(itr->first));     // ����� ������
		fout.write((char*)&itr->second, sizeof(itr->second));   // ����� ���-��
	}



	list<Grani>::iterator gr;
	unsigned short l = 0;      // ������ �������
	unsigned short h = 65535;  // ������� �������
	int delitel = spisok.back().right;   // back() - ��������� �� ��������� �������
	unsigned short first = (h + 1) / 4;   //16384
	unsigned short half = first * 2;	  //32768
	unsigned short thride = first * 3;    //49152
	int bitsf = 0;   // ������� ����� ����������
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
		h = l + gr->right * d / delitel - 1;  // ������� �������
		l = l + gr->left * d / delitel;       // ������ �������

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