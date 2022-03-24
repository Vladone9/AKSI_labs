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
		cout << endl;
	}
};





void CreateTable(Uzel* root, vector<bool>& vec, map<char, vector<bool>>& table)
{
	if (root->left != NULL)
	{
		vec.push_back(0);                       // заносим в вектор 0
		CreateTable(root->left, vec, table);    // делаем рекурсию
	}

	if (root->right != NULL)
	{
		vec.push_back(1);                       // заносим в вектор 1
		CreateTable(root->right, vec, table);   // делаем рекурсию
	}

	// если дошли до листа дерева
	if ((root->left == NULL) && (root->right == NULL))
	{
		table[root->s] = vec;   // заносим в таблицу вектор
	}
	// если размер вектора не нулевой
	if (vec.size())
	{
		vec.pop_back();   // выбрасываем последний бит из вектора и идем по другой ветке
	}
}







int main()
{
	ifstream is_f("text.txt");       // исходный текст
	ofstream os_res("encode.txt");   // результат кодирования
	
	MapKeys maap;
	maap.CreateMap(is_f);
	cout << "PrintMap:" << endl;
	maap.PrintMap();

	list<Uzel*> tree;
	map<char, int>::iterator itr;



	// создаем список указателей из контейнера map
	for (itr = maap.mp.begin(); itr != maap.mp.end(); ++itr)   // mp - название контейнера
	{
		Uzel* ptr = new Uzel;    // создаем указатель типа Uzel
		ptr->s = itr->first;     // присваиваем поле контейнеру (first это символ)
		ptr->key = itr->second;  // присваиваем поле контейнеру (second это ключ)
		tree.push_back(ptr);     // заполняем список (кидаем элементы в конец)
	}

	

	// после создания дерева в списке останется 1 элемент - указатель на корень дерева
	while (tree.size() != 1)
	{
		tree.sort(compare);          // сортируем список (по возрастанию)
		Uzel* l = tree.front();   // помещаем левый узел в начало списка
		tree.pop_front();            // выкидываем первый узел из списка
		Uzel* r = tree.front();  // помещаем правый узел в начало списка
		tree.pop_front();            // выкидываем первый узел из списка

		Uzel* parent = new Uzel(l, r);   // создаем общий узел для наим. узлов
		tree.push_back(parent);      // помещаем новый узел в конец списка
	}
	Uzel* root = tree.front();



	vector<bool> vec;
	map<char, vector<bool> > table;
	CreateTable(root, vec, table);



	// вывод таблицы в консоль
	cout << "Table:" << endl;
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)  // проходимся по записанным символам
	{
		cout << itr->first << ":";
		for (int j = 0; j < table[itr->first].size(); j++)   // выводим вектор побитово
		{
			cout << table[itr->first][j];
		}
		cout << endl;
		
	}



	// вывод таблицы в файл
	ofstream os_table("table.txt");
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)  // проходимся по записанным символам
	{
		os_table << itr->first << ":";
		for (int j = 0; j < table[itr->first].size(); j++)   // выводим вектор побитово
		{
			os_table << table[itr->first][j];
		}
		os_table << "\n";

	}
	os_table.close();



	// создание бинарного файла
	ofstream os_bin("binary.txt");
	is_f.clear();
	is_f.seekg(0);   // возвращаем указатель в начало файла
	for (char c = is_f.get(); c != EOF; c = is_f.get())
	{
		vector<bool> tmp = table[c];            // переписываем вектор из таблицы
		for (int i = 0; i < tmp.size(); i++)    // проходимся по битам вектора, начиная со старшего
		{
			if ((tmp[i] | 1) == tmp[i])         // если в бите 1
			{
				os_bin << "1";
			}
			else
			{
				os_bin << "0";
			}
		}
	}
	os_bin.close();





	// вывод результата
	ifstream is_bin("binary.txt");   // и снова открываем, но уже для чтения

	// подсчитаем кол-во символов в файле
	int binlen = 0;
	for (char c = is_bin.get(); c != EOF; c = is_bin.get())
	{
		binlen++;
	}
	cout << endl << "binlen = " << binlen << endl;
	is_bin.close();



	ifstream iss_bin("binary.txt");

	// записываем в файл-результат кол-во незначащих битов (нужно для декодирования)
	if (binlen % 8 == 0)
	{
		os_res << 0;
	}
	else
	{
		os_res << (8 - (binlen % 8));
	}


	char sym = 0;
	int count = 7;
	char bit;
	for (bit = iss_bin.get(); bit != EOF; bit = iss_bin.get())
	{
		// записываем 1 в старшие разряды
		if (bit == '1')
		{
			sym = sym | (1 << count);
		}

		// если нулевой бит, то ничего не записываем

		count--;
		
		// если записали 8 бит
		if (count == -1)
		{
			os_res << sym;       // записываем символ в файл-результат

			count = 7;
			sym = 0;
		}
	}
	os_res << sym;   // записываем последний символ
	iss_bin.close();



	is_f.close();
	os_res.close();
	return 0;
}
