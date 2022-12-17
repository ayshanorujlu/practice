#include <iostream>
#include<vector>
using namespace std;

//Bu kodlarda verilen tapshiriga elave olaraq diskin yazma sureti qeyd olunsun.
//(meselen : byte / saniye vahidi ile, deyek ki, yazilan faylin 10 byte olchusu var demeli fayl 1 saniyeye yazilacaq
//
//	bu emeliyyati hem createFile hem de setFileContent methodunda edin.
//
//	deleteFileByFilename(string filename) //eger file yoxdursa exception atsin, varsa file diskden silinsin 
//
//	updateFilename(string oldfilename, string newfilename) //eger fayl varsa adi deyishsin, yoxdursa exception atsin
//
//	hashFile(string filename); //bu method fayli hash variante chevirir istenilen hash algorithminden istifade ede bilersiniz,
//(internetden arashdirin) //bu method chagirilandan sonra, eger ichinde Hello World yazilmishdisa, artiq hash codelar gorunsun.
//
//ELAVE OLARAQ : SSD KLASI YARADIN VE ARTIQ HDD ILE OLAN ORTAQ EMELIYYATLARI "DISC" KLASINA CHIXARIN
//HDD VE SSD DISKDEN YARANSIN; //SSD SURETI HDD DEN 10 DEFE CHOX OLSUN
//
//
//FILE = > CLASINA ELAVE OLARAQ Faylin YARANMA VAXTI, EN SON DEYISHILME VAXTI SAXLANILSIN
//
//DISC = > ShowAllFiles METHODU OLSUN HANSIKI BUTUN FAYLLAR GORUNSUN VE FAYLIN BUTUN MELUMATLARI GORUNSUN



class MyException : public exception {
public:
	string message;
	MyException(string message) { this->message = message; }
	string Message() const { return message; }
};

class NoneFileException : public MyException {
public:
	NoneFileException(string message) : MyException(message) {}

	const char* what() const override {
		return message.c_str();
	}
};

class OutOfRangeException : public MyException {
public:
	OutOfRangeException(string message) : MyException(message) {}
	const char* what() const override {
		return message.c_str();
	}
};

class AlreadyExistException : public MyException {
public:
	AlreadyExistException(string message) : MyException(message) {}
	const char* what() const override {
		return message.c_str();
	}

};
class ArgumentNullException : public MyException {
public:
	ArgumentNullException(string message) : MyException(message) {}
	const char* what() const override {
		return message.c_str();
	}
};



namespace MemoryPlaces {

	class Disc {

	protected:
		double current_size = 0;

		class File {
		private:

			double file_size;
		public:
			string filename;
			string content;
			double getfile_size() {
				return file_size;
			}

			File(string filename, string content = "") {
				this->filename = filename;
				this->content = content;
				file_size = content.size();
			};
		};

		vector<File*> files;
		int file_count;

	public:
		const static double max_size;
		virtual File& getFile(string filename) {
			for (File* file : files) {
				if (file->filename == filename) {
					return *file;
				}
			}
			throw NoneFileException(" Bu adli fayl tapilmadi !\n");
		};

		virtual bool isExistsFile(string filename) {
			for (File* file : files) {
				if (file->filename == filename) {
					return true;
				}
			}
			return false;
		};

		virtual void createFile(string filename, string content = "") = 0 {
			if (filename == "") {
				throw ArgumentNullException(" Adsiz fayl yaratmaq olmaz");
			}

			if (isExistsFile(filename)) {
				throw AlreadyExistException(" Eyni adli fayl yaratmaq olmaz\n");
			}
			File* newFile = new File(filename, content);
			if (max_size < current_size + newFile->getfile_size()) {
				throw OutOfRangeException(" Bu fayl ucun bos yer yoxdur");
			}
			files.push_back(newFile);
			current_size += newFile->getfile_size();
		};

		virtual void setFileContent(string filename, string content = "") = 0 {
			if (!isExistsFile(filename)) {
				throw NoneFileException(" Bu adli fayl yoxdur\n");
			}

			for (File* file : files) {
				if (file->filename == filename) {
					current_size -= content.size();
					if (max_size < current_size + sizeof(content))
						throw OutOfRangeException(" Faylda bu content ucun yer yoxdur");

					file->content = content;
					current_size += content.size();
				}
			}

		};

		virtual void deleteFileContent(string filename) {
			if (!isExistsFile(filename)) {
				throw NoneFileException(" Bu adli fayl yoxdur\n");
			}
			for (File* file : files) {
				if (file->filename == filename) {
					file->content = "";
					current_size -= file->content.size();
				}
			}
		};

		virtual void deleteFileByFilename(string filename) {
			if (!isExistsFile(filename)) {
				throw NoneFileException(" Bu adli fayl yoxdur\n");
			}
			for (int i = 0; i < files.size(); i++) {

				if (files[i]->filename == filename) {
					current_size -= files[i]->getfile_size();
					files.erase(files.begin() + i);
				}
			}
		}

		virtual void updateFilename(string oldfilename, string newfilename) {
			if (oldfilename == "") {
				throw ArgumentNullException(" Adsiz fayl cagirmaq olmur");
			}

			if (!isExistsFile(oldfilename)) {
				throw NoneFileException(" Bu adli fayl yoxdur\n");
			}
			for (File* file : files) {
				if (file->filename == oldfilename) {
					file->filename = newfilename;
				}
			}
		};




		virtual int ContentToHashCode(string filename) {

			if (!isExistsFile(filename)) {
				throw NoneFileException(" Bu adli fayl yoxdur\n");
			}

			for (File* file : files) {
				if (file->filename == filename) {



					int hash = 0, strlen = file->content.length(), i;
					char character;

					if (strlen == 0)
						return hash;

					for (i = 0; i < strlen; i++) {
						character = file->content.at(i);
						hash = (31 * hash) + (int(character));

					}

					int test = hash;
					cout << endl << file->filename << " faylinin " << file->content << " contentninin hash code'u : " << test << endl;

					return hash;
				}
			}
		}



		virtual ~Disc() = 0
		{

		}

	};
};

using namespace MemoryPlaces;

class SSD : public Disc {
private:
	double speedOfSSD;

public:


	void createFile(string filename, string content = "") override {
		speedOfSSD = double(content.length()) / 10;
		cout << "\n --> Creating procces of " << filename << " file took " << speedOfSSD << " seconds\n";
		return Disc::createFile(filename, content);
	};


	void setFileContent(string filename, string content = "") override {
		speedOfSSD = double(content.length()) / 10;
		cout << "\n --> Setting procces of " << filename << " file took " << speedOfSSD << " seconds\n";
		return Disc::setFileContent(filename, content);
	};



};

class HDD : public Disc {
private:
	double speedOfHDD;

public:



	void createFile(string filename, string content = "") override {
		speedOfHDD = double(content.length());
		cout << "\n --> Creating procces of " << filename << " file took " << speedOfHDD << " seconds\n";
		return Disc::createFile(filename, content);
	};


	void setFileContent(string filename, string content = "") override {
		speedOfHDD = double(content.length());
		cout << "\n --> Setting procces of " << filename << " file took " << speedOfHDD << " seconds\n";
		return Disc::setFileContent(filename, content);
	};


};



const double Disc::max_size = 1024;

class OperationSystem {
	OperationSystem() {}
public:
	static void controlPC();
};

void OperationSystem::controlPC() {
	HDD hd;
	SSD sd;

	try
	{
		hd.createFile("newtext.txt", "Salam Aleykum vsem");
		hd.getFile("newtext.txt");
		hd.isExistsFile("newtext.txt");
		hd.setFileContent("newtext.txt", "Salam Aleykum vsem");
		hd.ContentToHashCode("newtext.txt");
		//hd.updateFilename("newtext.txt", "Mirhav.txt");
		hd.deleteFileContent("newtext.txt");

		sd.createFile("textForSSD.txt", "KAVKAZ");
		sd.getFile("textForSSD.txt");
		sd.isExistsFile("textForSSD.txt");
		sd.setFileContent("textForSSD.txt", "KAVKAZ");
		sd.ContentToHashCode("textForSSD.txt");
		//sd.updateFilename("newtext.txt", "Mirhav.txt");
		sd.deleteFileContent("textForSSD.txt");

	}
	catch (const exception& ex)
	{
		cout << "\n --> Error : " << ex.what() << endl;
	}
}

void main() {
	OperationSystem::controlPC();
}