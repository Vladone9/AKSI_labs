#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdbool>
#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include <iterator>
using namespace std;

class Uzel
{
public:
	char s;
	int key;
	Uzel* left;
	Uzel* right;

	// конструктор по умолчанию
	Uzel()
	{
		this->s = '\0';
		this->key = 0;
		this->left = NULL;
		this->right = NULL;
	}

	// конструктор (символ, ключ)
	Uzel(char symbol, int k)
	{
		this->s = symbol;
		this->key = k;
		this->left = NULL;
		this->right = NULL;
	}

	// конструктор (левый потомок, правый потомок)
	Uzel(Uzel* l, Uzel* r)
	{
		this->s = '\0';
		this->key = l->key + r->key;
		this->left = l;
		this->right = r;
	}

	// деструктор
	~Uzel()
	{
		delete[] this->left;
		delete[] this->right;
	}

	friend ostream& operator<<(ostream&, const Uzel&);
};

ostream& operator<<(ostream& os, const Uzel& obj)
{
	os << obj.s << ": " << obj.key << endl;
	return os;
}

bool compare(const Uzel* l, const Uzel* r)
{
	if (l->key < r->key)
	{
		return true;
	}

	return false;
}



void CreateTable(ifstream& f, vector<bool>& vec, map<char, vector<bool> >& table)
{
	f.clear();
	f.seekg(0);   // возвращаем указатель в начало файла
	char s;

	for (char symbol = f.get(); symbol != EOF; symbol = f.get())
	{
		// переписываем символ из начала строки
		s = symbol;

		// пока не дошли до конца строки
		while (symbol != '\n')
		{
			symbol = f.get();   // переходим к след. символу
			if (symbol != EOF)
			{
				if (symbol == '1')
				{
					vec.push_back(1);   // заносим в вектор 1
				}
				if (symbol == '0')
				{
					vec.push_back(0);   // заносим в вектор 0
				}
			}
		}

		table[s] = vec;   // заносим в таблицу вектор
		vec.clear();      // очищаем вектор

	}
}



void PrintTable(map<char, vector<bool> >& table)
{
	// проходимся по записанным символам
	for (auto i = table.begin(); i != table.end(); i++)
	{
		cout << i->first << ":";
		for (int j = 0; j < table[i->first].size(); j++)   // выводим вектор побитово
		{
			cout << table[i->first][j];
		}
		cout << endl;
	}
}











int main()
{
	ifstream table_f("table.txt");   // открываем файл для чтения

	// если файл не открыт
	if (!table_f.is_open())
	{
		cout << "File table.txt does not exist";
		exit(0);
	}

	vector<bool> vec;
	map<char, vector<bool> > table;
	CreateTable(table_f, vec, table);
	table_f.close();
	cout << "PrintTable:" << endl;
	PrintTable(table);

	// меняем индекс и значение таблицы местами
	map<vector<bool>, char> invtable;
	for (auto i = table.begin(); i != table.end(); i++)
	{
		invtable[i->second] = i->first;
	}



	// создание бинарного файла
	ifstream is_f("encode.txt");
	int len = 0;
	int binlen = 0;

	// считаем длину файла и его бинарную длину
	for (char c = is_f.get(); c != EOF; c = is_f.get())
	{
		len++;
	}
	binlen = (len - 1) * 8;
	is_f.clear();
	is_f.seekg(0);   // возвращаем указатель в начало файла

	// читаем цифру в начале файла text.txt
	// это кол-во лишних бит, которые добавили в конец
	char tmp = is_f.get();   // считываем первый символ из файла
	int digit = 0;
	if ('0' <= tmp <= '7')
	{
		digit = tmp - '0';
		binlen = binlen - digit;   // кол-во значащих бит для декодирования
		cout << endl << "binlen = " << binlen << endl;
	}
	else
	{
		cout << "Wrong file text.txt" << endl;
		is_f.close();
		exit(0);
	}

	// создаем бинарный файл
	ofstream os_bin("binary.txt");   // открываем бинарный файл для записи
	int count = 0;   // кол-во записанных в файл битов
	for (tmp = is_f.get(); tmp != EOF; tmp = is_f.get())
	{
		unsigned char mask = 1;
		mask = mask << 7;
		for (int i = 0; (i < 8) && (count < binlen); i++)
		{
			if ((tmp & mask) != 0)   // если i-ый бит ненулевой
			{
				os_bin << "1";   // печатаем 1 в бинарный файл
			}
			else
			{
				os_bin << "0";   // печатаем 0 в бинарный файл
			}
			mask = mask >> 1;
			count++;
		}
	}
	os_bin.close();



	// вывод результата
	ifstream is_bin("binary.txt");   // и снова открываем, но уже для чтения
	ofstream os_res("decode.txt");   // результат декодирования

	vector<bool> tmpvec;
	for (char bit = is_bin.get(); bit != EOF; bit = is_bin.get())
	{
		// если бит из файла = 1
		if (bit == '1')
		{
			tmpvec.push_back(1);   // заполняем вектор
		}
		else
		{
			tmpvec.push_back(0);   // заполняем вектор
		}

		// если существует символ в таблице по данному вектору
		if (invtable[tmpvec])
		{
			os_res << invtable[tmpvec];   // выводим этот символ в файл
			tmpvec.clear();               // обнуляем вектор для след. символа
		}
	}

	is_bin.close();
	os_res.close();
	is_f.close();
	return 0;
}