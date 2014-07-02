#include "parser.h"


typedef struct PatPoint {
	int x, y;
} Point2D;

typedef struct QRTable {
	Point2D qr_coords[100];
	string qr_label[100];
} QRTable;

QRTable createStaticTable(vector<parsedQr> array){
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
	return table;
}

int main()
{
	Parser parser;
	QRTable staticTable=createStaticTable(parser.getParsedTable());
}

