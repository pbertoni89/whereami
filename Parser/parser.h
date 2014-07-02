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
