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
	ifstream fin("text.txt", ios::binary);      // исходный текст
	ofstream fout("encode.txt", ios::binary);   // результат кодирования

	MapKeys maap;
	maap.CreateMap(fin);
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



	// выписываем контейнер в файл
	int k = maap.mp.size();
	fout.write((char*)&k, sizeof(k));   // записываем количество символов элементов
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)
	{
		fout.write((char*)&itr->first, sizeof(itr->first));     // пишем символ
		fout.write((char*)&itr->second, sizeof(itr->second));   // пишем кол-во
	}



	list<Grani>::iterator gr;
	unsigned short l = 0;      // нижняя граница
	unsigned short h = 65535;  // верхняя граница
	int delitel = spisok.back().right;   // back() - указатель на последний элемент
	unsigned short first = (h + 1) / 4;   //16384
	unsigned short half = first * 2;	  //32768
	unsigned short thride = first * 3;    //49152
	int bitsf = 0;   // сколько битов сбрасывать
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
		h = l + gr->right * d / delitel - 1;  // верхняя граница
		l = l + gr->left * d / delitel;       // нижняя граница

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