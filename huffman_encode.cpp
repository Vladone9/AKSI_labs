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
		cout << endl;
	}
};





void CreateTable(Uzel* root, vector<bool>& vec, map<char, vector<bool>>& table)
{
	if (root->left != NULL)
	{
		vec.push_back(0);                       // ������� � ������ 0
		CreateTable(root->left, vec, table);    // ������ ��������
	}

	if (root->right != NULL)
	{
		vec.push_back(1);                       // ������� � ������ 1
		CreateTable(root->right, vec, table);   // ������ ��������
	}

	// ���� ����� �� ����� ������
	if ((root->left == NULL) && (root->right == NULL))
	{
		table[root->s] = vec;   // ������� � ������� ������
	}
	// ���� ������ ������� �� �������
	if (vec.size())
	{
		vec.pop_back();   // ����������� ��������� ��� �� ������� � ���� �� ������ �����
	}
}







int main()
{
	ifstream is_f("text.txt");       // �������� �����
	ofstream os_res("encode.txt");   // ��������� �����������
	
	MapKeys maap;
	maap.CreateMap(is_f);
	cout << "PrintMap:" << endl;
	maap.PrintMap();

	list<Uzel*> tree;
	map<char, int>::iterator itr;



	// ������� ������ ���������� �� ���������� map
	for (itr = maap.mp.begin(); itr != maap.mp.end(); ++itr)   // mp - �������� ����������
	{
		Uzel* ptr = new Uzel;    // ������� ��������� ���� Uzel
		ptr->s = itr->first;     // ����������� ���� ���������� (first ��� ������)
		ptr->key = itr->second;  // ����������� ���� ���������� (second ��� ����)
		tree.push_back(ptr);     // ��������� ������ (������ �������� � �����)
	}

	

	// ����� �������� ������ � ������ ��������� 1 ������� - ��������� �� ������ ������
	while (tree.size() != 1)
	{
		tree.sort(compare);          // ��������� ������ (�� �����������)
		Uzel* l = tree.front();   // �������� ����� ���� � ������ ������
		tree.pop_front();            // ���������� ������ ���� �� ������
		Uzel* r = tree.front();  // �������� ������ ���� � ������ ������
		tree.pop_front();            // ���������� ������ ���� �� ������

		Uzel* parent = new Uzel(l, r);   // ������� ����� ���� ��� ����. �����
		tree.push_back(parent);      // �������� ����� ���� � ����� ������
	}
	Uzel* root = tree.front();



	vector<bool> vec;
	map<char, vector<bool> > table;
	CreateTable(root, vec, table);



	// ����� ������� � �������
	cout << "Table:" << endl;
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)  // ���������� �� ���������� ��������
	{
		cout << itr->first << ":";
		for (int j = 0; j < table[itr->first].size(); j++)   // ������� ������ ��������
		{
			cout << table[itr->first][j];
		}
		cout << endl;
		
	}



	// ����� ������� � ����
	ofstream os_table("table.txt");   // ����� ������� � ����
	for (itr = maap.mp.begin(); itr != maap.mp.end(); itr++)  // ���������� �� ���������� ��������
	{
		os_table << itr->first << ":";
		for (int j = 0; j < table[itr->first].size(); j++)   // ������� ������ ��������
		{
			os_table << table[itr->first][j];
		}
		os_table << "\n";

	}
	os_table.close();



	// �������� ��������� �����
	ofstream os_bin("binary.txt");   // �������� ����� � �������� ����
	is_f.clear();
	is_f.seekg(0);   // ���������� ��������� � ������ �����
	for (char c = is_f.get(); c != EOF; c = is_f.get())
	{
		vector<bool> tmp = table[c];            // ������������ ������ �� �������
		for (int i = 0; i < tmp.size(); i++)    // ���������� �� ����� �������, ������� �� ��������
		{
			if ((tmp[i] | 1) == tmp[i])         // ���� � ���� 1
			{
				os_bin << "1";
			}
			else
			{
				os_bin << "0";
			}
		}
	}
	os_bin.close();   // ��������� ���� � �������� �����





	// ����� ����������
	ifstream is_bin("binary.txt");   // � ����� ���������, �� ��� ��� ������

	// ���������� ���-�� �������� � �����
	int binlen = 0;
	for (char c = is_bin.get(); c != EOF; c = is_bin.get())
	{
		binlen++;
	}
	cout << endl << "binlen = " << binlen << endl;
	is_bin.close();



	ifstream iss_bin("binary.txt");

	// ���������� � ����-��������� ���-�� ���������� ����� (����� ��� �������������)
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
		// ���������� 1 � ������� �������
		if (bit == '1')
		{
			sym = sym | (1 << count);
		}

		// ���� ������� ���, �� ������ �� ����������

		count--;
		
		// ���� �������� 8 ���
		if (count == -1)
		{
			os_res << sym;       // ���������� ������ � ����-���������

			count = 7;
			sym = 0;
		}
	}
	os_res << sym;   // ���������� ��������� ������
	iss_bin.close();



	is_f.close();
	os_res.close();
	return 0;
}