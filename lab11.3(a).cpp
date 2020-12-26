#include <iostream> 
#include <fstream> 
#include <string> 
#include <iomanip>
#include <math.h>
#include <Windows.h>

using namespace std;

enum Spesialnist { KN, INF, ME, FI, TN };

string spesialnistStr[] = { "��������� �����", "�����������", "���������� �� ��������", "Գ���� �� �����������", "������� ��������" };

struct Student {
	char prizv[64];
	int kurs;
	Spesialnist spesialnist;
	int mark_math;
	int mark_phys;
	int mark_inf;
};

double Average_Mark(Student student) {
	return round((student.mark_phys + student.mark_math + student.mark_inf) / 3. * 100) / 100.;
}

void CreateBIN(char* fname) {
	ofstream fout(fname, ios::binary);
	char ch;
	int n = 1;
	int spes_number;
	Student student;
	do {
		cout << "������� � " << n << ":" << endl;
		cin.get();  // ������� ����� ���������, �� ����������� �������
		cin.sync(); // "����� �����", �� �� ����� ������ ��������� ������� �����
		cout << "    �������: ";
		cin >> student.prizv;
		cout << "    ����: ";
		cin >> student.kurs;
		cout << "   ������������: (0 - ��������� �����, 1 - �����������, 2 - ���������� �� ��������,";
		cout << " 3 - Գ���� �� �����������, 4 - ������� ��������): ";
		cin >> spes_number;
		student.spesialnist = (Spesialnist)spes_number;
		cout << "    ������ � ������: ";
		cin >> student.mark_phys;
		cout << "    ������ � ����������: ";
		cin >> student.mark_math;
		cout << "    ������ � �����������: ";
		cin >> student.mark_inf;
		fout.write((char*)&student, sizeof(student));
		n++;
		cout << "continue? (y/n): ";
		cin >> ch;
	} while (ch == 'y' || ch == 'Y');
	cout << endl;
}

void Print(char* fname) {
	ifstream fin(fname, ios::binary);
	if (!fin) {
		cout << "Print: file opening fails. " << endl;
		return;
	}
	cout << "file name: " << fname << endl;
	cout << "==========================================================================================================" << endl;
	cout << "| � |   �������   |  ����  |      ������������      | Գ���� | ���������� | ����������� | ������� ��� |" << endl;
	cout << "----------------------------------------------------------------------------------------------------------" << endl;
	Student student;
	int i = 1;
	while (fin.read((char*)&student, sizeof(Student))) {
		cout << "| " << i << " ";
		cout << "| " << setw(12) << left << student.prizv << " ";
		cout << "|    " << student.kurs << "   ";
		cout << "| " << setw(23) << left << spesialnistStr[student.spesialnist] << " ";
		cout << "| " << setw(6) << right << student.mark_phys << " ";
		cout << "| " << setw(10) << right << student.mark_math << " ";
		cout << "| " << setw(11) << right << student.mark_inf << " ";
		cout << "| " << setw(12) << right << Average_Mark(student) << " ";
		cout << "|" << endl;
		i++;
	}
	cout << "==========================================================================================================" << endl;
	cout << endl;
}

template <typename T>
void fWrite(fstream& f, const int i, const T x) {
	f.seekp(i * (long)sizeof(T));
	f.write((char*)&x, sizeof(T));
}

template <typename T>
T fRead(fstream& f, const int i) {
	T x;
	f.seekg(i * (long)sizeof(T));
	f.read((char*)&x, sizeof(T));
	return x;
}

template <typename T>
T f_Read(ifstream& f, const int i) {
	T x;
	f.seekg(i * (long)sizeof(T));
	f.read((char*)&x, sizeof(T));
	return x;
}

template <typename T>
void fChange(fstream& f, const int i, const int j) {
	T x = fRead<T>(f, i);
	T y = fRead<T>(f, j);
	fWrite<T>(f, i, y);
	fWrite<T>(f, j, x);
}

bool Less(Student A, Student B, bool OrEqual) {
	if (B.kurs == 0 || A.kurs == 0) return false;
	if (OrEqual && strcmp(A.prizv, B.prizv) == 0 && A.kurs == B.kurs) return true;
	if (Average_Mark(A) > Average_Mark(B)) return true;
	if (Average_Mark(A) == Average_Mark(B) && A.kurs < B.kurs) return true;
	if (Average_Mark(A) == Average_Mark(B) && A.kurs == B.kurs && A.prizv > B.prizv) return true;
	return false;
}

template <typename T>
void SortBIN_direct(char* fname) {
	fstream f(fname, ios::binary | ios::in | ios::out);
	f.seekg(0, ios::end);

	int size = f.tellg() / sizeof(T);

	f.seekg(0, ios::beg);

	for (int i0 = 1; i0 < size; i0++)
		for (int i1 = 0; i1 < size - i0; i1++) {
			T a = fRead<T>(f, i1);
			T b = fRead<T>(f, i1 + 1);
			if (Less(b, a, false)) fChange<T>(f, i1, i1 + 1);
		}
	f.seekp(0, ios::end);
}

template <typename T>
void Build_Index(char* fname, char* gname) {
	ofstream g(gname, ios::binary);
	T s, z = T(), mins = T();
	bool mins_is_empty;
	bool z_is_empty = true;
	do {
		mins_is_empty = true;
		ifstream f(fname, ios::binary);
		if (!f) {
			cout << "Build_Index: file opening fails." << endl;
			return;
		}
		int idx = -1;
		int min_idx = 0;
		while (f.read((char*)&s, sizeof(T))) {
			idx++;
			if (!z_is_empty) {
				if (Less(s, z, true)) continue;
			}
			if (mins_is_empty) {
				mins = s;
				min_idx = idx;
				mins_is_empty = false;
			}
			if (Less(s, mins, false)) {
				mins = s;
				min_idx = idx;
			}
		}
		f.close();
		if (mins_is_empty) break;
		z = mins;
		z_is_empty = false;
		g.write((char*)&min_idx, sizeof(min_idx));
	} while (true);
}

void Print_I(char* fname, char* gname) {
	ifstream fin(fname, ios::binary);
	if (!fin) {
		cout << "Print_I: file opening fails. " << fname << endl;
		return;
	}
	ifstream gin(gname, ios::binary);
	if (!gin) {
		cout << "Print_I: file opening fails. " << gname << endl;
		return;
	}
	cout << "file name: " << fname << endl;
	cout << "index file name: " << gname << endl;
	cout << "==========================================================================================================" << endl;
	cout << "| � |   �������   |  ����  |      ������������      | Գ���� | ���������� | ����������� | ������� ��� |" << endl;
	cout << "----------------------------------------------------------------------------------------------------------" << endl;
	Student student;
	int i = 1;
	int idx = 0;
	while (gin.read((char*)&idx, sizeof(idx))) {
		student = f_Read<Student>(fin, idx);
		cout << "| " << i << " ";
		cout << "| " << setw(12) << left << student.prizv << " ";
		cout << "|    " << student.kurs << "   ";
		cout << "| " << setw(23) << left << spesialnistStr[student.spesialnist] << " ";
		cout << "| " << setw(6) << right << student.mark_phys << " ";
		cout << "| " << setw(10) << right << student.mark_math << " ";
		cout << "| " << setw(11) << right << student.mark_inf << " ";
		cout << "| " << setw(12) << right << Average_Mark(student) << " ";
		cout << "|" << endl;
		i++;
	}
	cout << "==========================================================================================================" << endl;
	cout << endl;
}

int BinSearch(char* fname, char* gname, char* prizv, const int kurs, const double average) {
	// ������� ������ ���������� �������� ��� -1, ���� ������� ������� �������
	ifstream fin(fname, ios::binary);
	if (!fin) {
		cout << "BinSearch: file opening fails. " << fname << endl;
		return -1;
	}
	ifstream gin(gname, ios::binary);
	if (!gin) {
		cout << "BinSearch: file opening fails. " << gname << endl;
		return -1;
	}
	fin.seekg(0, ios::end);
	int N = fin.tellg()/sizeof(Student);
	int L = 0, R = N - 1, m;
	do {
		m = (L + R) / 2;
		int idx_m = f_Read<int>(gin, m);
		Student student_m = f_Read<Student>(fin, idx_m);
		double average_m = Average_Mark(student_m);
		if (strcmp(student_m.prizv, prizv) == 0 && student_m.kurs == kurs && average_m == average) {
			return m;
		}
		if (
			(average_m < average) ||
			((average_m == average) && (student_m.kurs > kurs)) ||
			((average_m == average) && (student_m.kurs == kurs) && (strcmp(student_m.prizv, prizv) == -1))
			) {
			R = m - 1;
		}
		else {
			L = m + 1;
		}
	} while (L <= R);
	return -1;
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	char fname[100] = "";
	char gname[100];
	int choice;
	int found;
	do {
		cout << "������ ����� ������� �������:" << endl << endl;
		cout << " [1] - ������ ����" << endl;
		cout << " [2] - ������ ������ ��������" << endl;
		cout << " [3] - ��������� �������" << endl;
		cout << " [4] - ������ �������� (������� �����) " << endl;
		cout << endl;
		cout << " [0] - ����� �� ��������" << endl << endl;
		cout << "������ ����� �������: ";
		cin >> choice;
		cout << endl;
		switch (choice) {
		case 1:
			cout << "������ ����� �����: ";
			cin >> fname;
			cout << "������ ����� ����� �������: ";
			cin >> gname;
			Print(fname);
			Print_I(fname, gname);
			break;
		case 2:
			if (strcmp(fname,"")==0) {
				cout << "������ ����� �����: ";
				cin >> fname;
				cout << "������ ����� ����� �������: ";
				cin >> gname;
			}
			CreateBIN(fname);
			Print(fname);
			Build_Index<Student>(fname, gname);
			Print_I(fname, gname);
			break;
		case 3:
			if (strcmp(fname, "") == 0) {
				cout << "���� �� �� �������." << endl;
				break;
			}
			SortBIN_direct<Student>(fname);
			Build_Index<Student>(fname, gname);
			Print(fname);
			break;
		case 4:
			if (strcmp(fname, "") == 0) {
				cout << "���� �� �� �������." << endl;
				break;
			}
			char prizv[64];
			cout << "������ �������: ";
			cin.get();
			cin.sync();
			cin >> prizv;
			int kurs;
			cout << "������ ����: ";
			cin >> kurs;
			double average;
			cout << "������ ������� ���: ";
			cin >> average;
			found = BinSearch(fname, gname, prizv, kurs, average);
			if (found == -1) {
				cout << "�������� �� ��������" << endl;
			}
			else {
				ifstream fin(fname, ios::binary);
				if (!fin) {
					cout << "main: file opening fails. " << fname << endl;
					return -1;
				}
				Student student = f_Read<Student>(fin, found);
				cout << "�������� �������� � ������� " << found + 1 << endl;
				cout << "   ������������: " << student.spesialnist << endl;
				cout << "   ������ � ������: " << student.mark_phys << endl;
				cout << "   ������ � ����������: " << student.mark_math << endl;
				cout << "   ������ � �����������: " << student.mark_inf << endl;
			}
			break;
		case 0:
			break;
		default:
			cout << "�� ����� ������������ ����� ������� " << endl;
		}
	} while (choice != 0);
	return 0;
}
