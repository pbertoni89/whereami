#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>


using namespace std;


typedef struct qrstruct
{
  string payload;
  int x,y;
}qrstruct;

typedef struct parsedQr
{
  string payload,x,y;
}parsedQr;


typedef struct PatPoint {
	int x, y;
} Point2D;

typedef struct QRTable {
	Point2D qr_coords[100];
	string qr_label[100];
} QRTable;


class Parser{
	public:
		vector<string> righe;
		vector<parsedQr> parsed;
		vector<qrstruct> output;
		vector<string> parseLines();
		vector<parsedQr> parseSecond(vector<string> array);
		vector<qrstruct> getQRtablr(vector<parsedQr> array);
	
		Parser();
		//vector<qrstruct> getParsedTable();
		vector<parsedQr> getParsedTable();
		//TIRIAMO LE SOMME
		void setTable(vector<parsedQr> array);
	};
	
Parser::Parser(){
	vector<string> righe = parseLines();	
	parsed= parseSecond(righe);
	output= getQRtablr(parsed);
	}

vector<parsedQr> Parser::getParsedTable(){
	//return output;
	return parsed;
	}


vector<string> Parser::parseLines(){
std::ifstream in("splitlines.txt");
int i=0;
vector<string> array;

while(in){
	std::string line;
	std::getline(in,line);
	if(line.size()>0){
		array.push_back(line);
		cout<<array[i]<<" with size "<<array[i].size()<<"\n";
		i++;
		}
	}
cout<<"Array delle righe creato. Ora in ogni riga c'e scritto:\n";
return array;
}

vector<parsedQr> Parser::parseSecond(vector<string> array){
// dati per la seconda parte
cout<<"--------------------------\nPARTE SECONDA\n-------------------------------\n";

cout<<"cosa è entrato?\n";
int ff=0;
for(ff=0; ff<array.size(); ff++){
	cout<<"1) " <<array[ff]<<"\n";
	}

// tutto bene per ora...


int i;
string str;
string str1;
int pos2;
int pos1;

string s;
string delimiter = ",";

vector<parsedQr> output;
string parsed [3] ;
parsedQr info;

size_t pos = 0;
string token;

cout<<"\n";
int count;
for(i=0; i< array.size(); i++){
	count=0;
	
	str=array[i];
	pos1 = str.find("(")+1;     
	str1 = str.substr (pos1);   
	pos2 = str.find(")");     
	s = str1.substr (0,pos2-pos1);   
	cout<< "Stringa riconosciuta :";
	cout << s<<"\n";
	
	cout<< "Parsing della stringa :\n";
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		cout << "Token is: ";
		parsed[count]=token;
		std::cout << token << "con count = "<< count<<"\n";
		s.erase(0, pos + delimiter.length());
		count++;
	}
	parsed[count]=s;
	cout << "in the end: ";
	cout << s << std::endl;

	info.payload=parsed[0];
	cout<<"info payload e' :"<<info.payload<<"\n";
	info.x=parsed[1];
	cout<<"info x e' :"<<info.x<<"\n";
	info.y=parsed[2];
	cout<<"info y e' :"<<info.y<<"\n";
	output.push_back(info);
	
	}
	return output;
}

vector<qrstruct> Parser::getQRtablr(vector<parsedQr> array){
	vector<qrstruct> output;
	qrstruct parsed;
	int z;
	for(z=0; z<array.size(); z++){
	
	parsed.payload=array[z].payload;
	istringstream buf(array[z].x);
	buf >> parsed.x;
	istringstream buf2(array[z].y);
	buf2 >> parsed.y;
	output.push_back(parsed); 
	}
	
	cout << "Stampa per verifica FINALE:\n";
	
	cout << "Output is\n";
	for(z=0; z<array.size(); z++){
		cout<<z<<"):\n";
		cout<<"payload: "<<output[z].payload<<"\n";
		cout<<"x: "<<output[z].x<<"\n";
		cout<<"y: "<<output[z].y<<"\n";
		}	
	return output;
}

void Parser::setTable(vector<parsedQr> array){
	QRTable table;
	int z;
	for(z=0; z<array.size(); z++){
	
	table.qr_label[z]=array[z].payload;
	istringstream buf(array[z].x);
	buf >> table.qr_coords[z].x;
	istringstream buf2(array[z].y);
	buf2 >> table.qr_coords[z].y;
	}
	
	cout << "Stampa per verifica FINALE:\n";
	z=0;
	while(table.qr_label[z].size()>0){
		cout << "This is a QR:\n";
		cout <<"\tpayload: "<<table.qr_label[z]<<"\n";
		cout <<"\tx: "<<table.qr_coords[z].x<<"\n";
		cout <<"\ty: "<<table.qr_coords[z].y<<"\n";
		z++;
		}

}


int main()
{
	Parser parser;
	parser.setTable(parser.getParsedTable());
}
