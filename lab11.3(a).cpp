#include <iostream> 
#include <fstream> 
#include <string> 
#include <iomanip>
#include <math.h>
#include <Windows.h>

using namespace std;

enum Spesialnist { KN, INF, ME, FI, TN };

string spesialnistStr[] = { "Комп’ютерні науки", "Інформатика", "Математика та економіка", "Фізика та інформатика", "Трудове навчання" };

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
		cout << "Студент № " << n << ":" << endl;
		cin.get();  // очищуємо буфер клавіатури, бо залишаються символи
		cin.sync(); // "кінець рядка", які не дають ввести наступний літерний рядок
		cout << "    прізвище: ";
		cin >> student.prizv;
		cout << "    курс: ";
		cin >> student.kurs;
		cout << "   спеціальність: (0 - Комп’ютерні науки, 1 - Інформатика, 2 - Математика та економіка,";
		cout << " 3 - Фізика та інформатика, 4 - Трудове навчання): ";
		cin >> spes_number;
		student.spesialnist = (Spesialnist)spes_number;
		cout << "    оцінка з фізики: ";
		cin >> student.mark_phys;
		cout << "    оцінка з математики: ";
		cin >> student.mark_math;
		cout << "    оцінка з інформатики: ";
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
	cout << "| № |   Прізвище   |  Курс  |      Спеціальність      | Фізика | Математика | Інформатика | Середній бал |" << endl;
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
	cout << "| № |   Прізвище   |  Курс  |      Спеціальність      | Фізика | Математика | Інформатика | Середній бал |" << endl;
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
	// повертає індекс знайденого елемента або -1, якщо шуканий елемент відсутній
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
		cout << "Оберіть номер потрібної команди:" << endl << endl;
		cout << " [1] - Обрати файл" << endl;
		cout << " [2] - Ввести список студентів" << endl;
		cout << " [3] - Сортувати фізично" << endl;
		cout << " [4] - Знайти студента (бінарний пошук) " << endl;
		cout << endl;
		cout << " [0] - Вихід із програми" << endl << endl;
		cout << "Введіть номер команди: ";
		cin >> choice;
		cout << endl;
		switch (choice) {
		case 1:
			cout << "Введіть назву файла: ";
			cin >> fname;
			cout << "Введіть назву файла індексу: ";
			cin >> gname;
			Print(fname);
			Print_I(fname, gname);
			break;
		case 2:
			if (strcmp(fname,"")==0) {
				cout << "Введіть назву файла: ";
				cin >> fname;
				cout << "Введіть назву файла індексу: ";
				cin >> gname;
			}
			CreateBIN(fname);
			Print(fname);
			Build_Index<Student>(fname, gname);
			Print_I(fname, gname);
			break;
		case 3:
			if (strcmp(fname, "") == 0) {
				cout << "Файл ще не обраний." << endl;
				break;
			}
			SortBIN_direct<Student>(fname);
			Build_Index<Student>(fname, gname);
			Print(fname);
			break;
		case 4:
			if (strcmp(fname, "") == 0) {
				cout << "Файл ще не обраний." << endl;
				break;
			}
			char prizv[64];
			cout << "Введіть прізвище: ";
			cin.get();
			cin.sync();
			cin >> prizv;
			int kurs;
			cout << "Введіть курс: ";
			cin >> kurs;
			double average;
			cout << "Введіть середній бал: ";
			cin >> average;
			found = BinSearch(fname, gname, prizv, kurs, average);
			if (found == -1) {
				cout << "Студента не знайдено" << endl;
			}
			else {
				ifstream fin(fname, ios::binary);
				if (!fin) {
					cout << "main: file opening fails. " << fname << endl;
					return -1;
				}
				Student student = f_Read<Student>(fin, found);
				cout << "Знайдено студента з номером " << found + 1 << endl;
				cout << "   спеціальність: " << student.spesialnist << endl;
				cout << "   оцінка з фізики: " << student.mark_phys << endl;
				cout << "   оцінка з математики: " << student.mark_math << endl;
				cout << "   оцінка з інформатики: " << student.mark_inf << endl;
			}
			break;
		case 0:
			break;
		default:
			cout << "Ви ввели неправильний номер команди " << endl;
		}
	} while (choice != 0);
	return 0;
}
