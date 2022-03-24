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

	// ����������� �� ���������
	Uzel()
	{
		this->s = '\0';
		this->key = 0;
		this->left = NULL;
		this->right = NULL;
	}

	// ����������� (������, ����)
	Uzel(char symbol, int k)
	{
		this->s = symbol;
		this->key = k;
		this->left = NULL;
		this->right = NULL;
	}

	// ����������� (����� �������, ������ �������)
	Uzel(Uzel* l, Uzel* r)
	{
		this->s = '\0';
		this->key = l->key + r->key;
		this->left = l;
		this->right = r;
	}

	// ����������
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
	f.seekg(0);   // ���������� ��������� � ������ �����
	char s;

	for (char symbol = f.get(); symbol != EOF; symbol = f.get())
	{
		// ������������ ������ �� ������ ������
		s = symbol;

		// ���� �� ����� �� ����� ������
		while (symbol != '\n')
		{
			symbol = f.get();   // ��������� � ����. �������
			if (symbol != EOF)
			{
				if (symbol == '1')
				{
					vec.push_back(1);   // ������� � ������ 1
				}
				if (symbol == '0')
				{
					vec.push_back(0);   // ������� � ������ 0
				}
			}
		}

		table[s] = vec;   // ������� � ������� ������
		vec.clear();      // ������� ������

	}
}



void PrintTable(map<char, vector<bool> >& table)
{
	// ���������� �� ���������� ��������
	for (auto i = table.begin(); i != table.end(); i++)
	{
		cout << i->first << ":";
		for (int j = 0; j < table[i->first].size(); j++)   // ������� ������ ��������
		{
			cout << table[i->first][j];
		}
		cout << endl;
	}
}











int main()
{
	ifstream table_f("table.txt");   // ��������� ���� ��� ������

	// ���� ���� �� ������
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

	// ������ ������ � �������� ������� �������
	map<vector<bool>, char> invtable;
	for (auto i = table.begin(); i != table.end(); i++)
	{
		invtable[i->second] = i->first;
	}



	// �������� ��������� �����
	ifstream is_f("encode.txt");
	int len = 0;
	int binlen = 0;

	// ������� ����� ����� � ��� �������� �����
	for (char c = is_f.get(); c != EOF; c = is_f.get())
	{
		len++;
	}
	binlen = (len - 1) * 8;
	is_f.clear();
	is_f.seekg(0);   // ���������� ��������� � ������ �����

	// ������ ����� � ������ ����� text.txt
	// ��� ���-�� ������ ���, ������� �������� � �����
	char tmp = is_f.get();   // ��������� ������ ������ �� �����
	int digit = 0;
	if ('0' <= tmp <= '7')
	{
		digit = tmp - '0';
		binlen = binlen - digit;   // ���-�� �������� ��� ��� �������������
		cout << endl << "binlen = " << binlen << endl;
	}
	else
	{
		cout << "Wrong file text.txt" << endl;
		is_f.close();
		exit(0);
	}

	// ������� �������� ����
	ofstream os_bin("binary.txt");   // ��������� �������� ���� ��� ������
	int count = 0;   // ���-�� ���������� � ���� �����
	for (tmp = is_f.get(); tmp != EOF; tmp = is_f.get())
	{
		unsigned char mask = 1;
		mask = mask << 7;
		for (int i = 0; (i < 8) && (count < binlen); i++)
		{
			if ((tmp & mask) != 0)   // ���� i-�� ��� ���������
			{
				os_bin << "1";   // �������� 1 � �������� ����
			}
			else
			{
				os_bin << "0";   // �������� 0 � �������� ����
			}
			mask = mask >> 1;
			count++;
		}
	}
	os_bin.close();



	// ����� ����������
	ifstream is_bin("binary.txt");   // � ����� ���������, �� ��� ��� ������
	ofstream os_res("decode.txt");   // ��������� �������������

	vector<bool> tmpvec;
	for (char bit = is_bin.get(); bit != EOF; bit = is_bin.get())
	{
		// ���� ��� �� ����� = 1
		if (bit == '1')
		{
			tmpvec.push_back(1);   // ��������� ������
		}
		else
		{
			tmpvec.push_back(0);   // ��������� ������
		}

		// ���� ���������� ������ � ������� �� ������� �������
		if (invtable[tmpvec])
		{
			os_res << invtable[tmpvec];   // ������� ���� ������ � ����
			tmpvec.clear();               // �������� ������ ��� ����. �������
		}
	}

	is_bin.close();
	os_res.close();
	is_f.close();
	return 0;
}