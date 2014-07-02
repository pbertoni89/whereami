#include "parser.h"

Parser::Parser(){
	vector<string> righe = parseLines();	
	parsed= parseSecond(righe);
	output= getQRtablr(parsed);
	}

vector<parsedQr> Parser::getParsedTable(){
	//return output;
	return parsed;
	}

/** Prende il file e costruisce un vettore di stringhe in cui ognuna è una riga del file. */
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

/** Per ogni riga del file (che è diventata una stringa) estrae la terna di valori in QR1(x,y,z) */
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


