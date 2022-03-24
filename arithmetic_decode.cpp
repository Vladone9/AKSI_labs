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
	unsigned int key;   // частотность
	int left;           // левая граница
	int right;          // правая граница
};

class MapKeys
{
public:
	map<char, int> mp;

	void CreateMap(ifstream& f)
	{
		// создаем контейнер map
		// символ представляется как индекс
		// и по этому индексу значение ячейки увеличивается на 1
		// то есть ищем кол-во вхождений символа в текст (частотность)
		for (char c = f.get(); c != EOF; c = f.get())
		{
			mp[c]++;
		}
		f.clear();
		f.seekg(0);   // возвращаем указатель в начало файла
	}

	int HeadRead(ifstream& f)
	{
		unsigned long long len = 0;   // длина общего текста, не считая головы
		int key;
		int q;
		char s;

		f.read((char*)&q, sizeof(q));
		for (int i = 0; i < q; i++) {
			f.read((char*)&s, sizeof(s));		// считываем символ
			f.read((char*)&key, sizeof(key));	// считываем его значение
			mp[s] = key;						// помещаем ключ в контейнер map
			len = len + key;
		}

		return len;
	}

	void PrintMap()
	{
		// begin() и end() это начало и конец контейнера map
		// проходимся итератором по контейнеру и печатаем содержимое
		// it->first - печатаем символ char
		// it->second - частотность символа
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
	ifstream fin("encode.txt", ios::binary);
	ofstream fout("decode.txt", ios::binary);

	MapKeys maap;
	// считываем контейнер из файла и получаем общую длину
	int len = maap.HeadRead(fin);
	cout << "PrintMap:" << endl;
	maap.PrintMap();
	cout << "size = " << maap.mp.size() << endl << endl;

	list<Grani> spisok;
	map<char, int>::iterator itr;
	unsigned long long count = 0;



	// переписываем содержимое контейнера map в список
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)
	{
		Grani tmp;
		tmp.c = itr->first;      // it->first - символ char
		tmp.key = itr->second;   // it->second - частотность
		count += tmp.key;
		spisok.push_back(tmp);   // помещаем в конец списка
	}
	spisok.sort(compare);        // сортируем список (по убыванию)



	// выводим содержимое списка
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



	list<Grani>::iterator gr;
	unsigned short l = 0;      // нижняя граница
	unsigned short h = 65535;  // верхняя граница
	int last = spisok.back().right;   // back() - указатель на последний элемент
	unsigned short first = (h + 1) / 4;   //16384
	unsigned short half = first * 2;	  //32768
	unsigned short thride = first * 3;    //49152
	int value = (fin.get() << 8) | fin.get();
	char tmp = fin.get();
	int ncount = 0;

	while (len)
	{
		int d = h - l + 1;
		unsigned int freq = (((value - l + 1) * last) - 1) / d;
		for (gr = spisok.begin(); gr->right <= freq; gr++);
		h = l + gr->right * d / last - 1;  // верхняя граница
		l = l + gr->left * d / last;       // нижняя граница
		
		while (1)
		{
			if (h < half);
			else
			{
				if (l >= half)
				{
					value -= half;
					l -= half;
					h -= half;
				}
				else
				{
					if ((l >= first) && (h < thride))
					{
						value -= first;
						l -= first;
						h -= first;
					}
					else
					{
						break;
					}
				}
			}

			l = l << 1;
			h = h << 1;
			h++;
			value = value << 1;
			value += ((tmp & (1 << (7 - ncount))) >> (7 - ncount));
			ncount++;
			if (ncount == 8)
			{
				tmp = fin.get();
				ncount = 0;
			}
		}

		fout << gr->c;
		len--;
	}

	fin.close();
	fout.close();

	return 0;
}