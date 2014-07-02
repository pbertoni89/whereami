#include "worldKB.h"

RecognizedLandmark::RecognizedLandmark(QRInfos _qr_info) {
	this->qr_info = _qr_info;
	this->phi_angle = 0;
}
RecognizedLandmark::~RecognizedLandmark() { ; }

QRInfos RecognizedLandmark::getQRInfos() {
	return this->qr_info;
}
void RecognizedLandmark::setPhiAngle(int _phi_angle) {
	this->phi_angle = _phi_angle;
}
int RecognizedLandmark::getPhiAngle() {
	return this->phi_angle;
}

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

WorldKB::WorldKB() {
	this->camera_angle = CAMERA_INIT_ANGLE;
	this->parseStaticKB("staticKB.txt");
	this->printStaticKB();
}

WorldKB::~WorldKB() { ; }

int WorldKB::get_qr_found() {
	return this->dynamicKB.size();
}

void WorldKB::pushQR(QRInfos* qr_info) {
	RecognizedLandmark temp = RecognizedLandmark(*qr_info);
	//printf("qr info just copied to be passed. its payload is %s\n", temp.getQRInfos().qr_message);
	this->dynamicKB.push_back(temp);
	//printf("qr info just passed. its payload is %s\n", this->landmarks.back().getQRInfos().qr_message);
}

int WorldKB::getCameraAngle() {
	return this->camera_angle;
}

void WorldKB::setCameraAngle(int _camera_angle) {
	this->camera_angle = _camera_angle;
}

void WorldKB::incrementCameraAngle()  {
	this->setCameraAngle(this->getCameraAngle() + CAMERA_STEP_ANGLE);
}

bool WorldKB::isQRInDynamicKB(string label) {
	string tmp;
	vector<RecognizedLandmark>::iterator it = this->dynamicKB.begin();
	while(it != this->dynamicKB.end()) {
		tmp = string((*it).getQRInfos().qr_message);
		if(label.compare(tmp)==0)
			return true;
		it++;
	}
	return false;
}

bool WorldKB::isQRInStaticKB(string label) {
	string tmp;
	vector<Landmark>::iterator it = this->staticKB.begin();
	while(it != this->staticKB.end()) {
		tmp = string((*it).qr_label);
		if(label.compare(tmp)==0)
			return true;
		it++;
	}
	return false;
}

/** Per ogni riga del file (che è diventata una stringa) estrae la terna di valori <payload,x,y> in QR1(payload,x,y) */
void WorldKB::parseStaticKB(string filename) {

	ifstream in(filename.c_str());
	int i=0;
	vector<string> lines;

	while(in){
		string line;
		getline(in,line);
		if(line.size()>0){
			lines.push_back(line);
					cout<<lines[i]<<" with size "<<lines[i].size()<<"\n";
			i++;
		}
	}

	string str;
	string str1;
	int pos2;
	int pos1;

	string s;
	string delimiter = ",";

	string parsed[3] ;
	Landmark temp;

	size_t pos = 0;
	string token;

	cout<<"\n";
	int count;
	for(i=0; i< lines.size(); i++) {
		count=0;

		str=lines[i];
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

		temp.qr_label = parsed[0];
			cout<<"temp payload e' :"<<temp.qr_label<<"\n";
		temp.qr_points.x = std::stoi(parsed[1]);
			cout<<"temp x e' :"<<temp.qr_points.x<<"\n";
		temp.qr_points.y = stoi(parsed[2]);
			cout<<"temp y e' :"<<temp.qr_points.y <<"\n";
		this->staticKB.push_back(temp);
		}
	}

void WorldKB::printStaticKB() {

	cout << "Output is\n";
	for(int z=0; z<this->staticKB.size(); z++){
		cout<<z<<"):\n";
		cout<<"payload: "<<this->staticKB[z].qr_label<<"\n";
		cout<<"x: "<<this->staticKB[z].qr_points.x<<"\n";
		cout<<"y: "<<this->staticKB[z].qr_points.x<<"\n";
	}
}
