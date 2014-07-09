#include "worldKB.h"

Landmark::Landmark(Point2D _qr_coords, string _qr_label)
{
	this->coords = _qr_coords;
	this->label = _qr_label;
	this->distance = 0;
	this->delta_angle = 0;
	this->recognized = false;
}

Landmark::Landmark() {;}
Landmark::~Landmark() {;}

Point2D Landmark::getCoords()
{
	return this->coords;
}
double Landmark::getX()
{
	return this->coords.x;
}
double Landmark::getY()
{
	return this->coords.y;
}
string Landmark::getLabel()
{
	return this->label;
}
double Landmark::getDistance()
{
	return this->distance;
}

double Landmark::getDeltaAngle()
{
	return this->delta_angle;
}

void Landmark::recognize(double _distance, double _delta_angle)
{
	this->recognized = true;
	this->distance = _distance;
	this->delta_angle = _delta_angle;
}

bool Landmark::isRecognized()
{
	return this->recognized;
}

void Landmark::print()
{
	cout << endl << "Landmark " << this->label /*<< "\t address " << this <<*/;
	cout << ", Coords: x = " << this->coords.x << ", y = " << this->coords.y;
	if(this->recognized)
		cout << ", Recognized at distance = " << this->distance << ", delta_angle = " << this->delta_angle;
	else
		cout << ", Not yet recognized.";
	cout << endl << endl;
}

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

WorldKB::WorldKB()
{
	this->cameraAngle = 0;
	this->parseStaticKB(KB_FILE);
	this->parseParameters(PARAM_FILE);
	this->printKB();
}

int WorldKB::getRecognizedQRs()
{
	int found = 0;
	vector<Landmark>::iterator it = this->kb.begin();
	while(it != this->kb.end()) {
		if((*it).isRecognized())
			found++;
		it++;
	}
	return found;
}

vector<Landmark> WorldKB::getKB()
{
	return this->kb;
}

void WorldKB::pushQR(string _label, double _distance, double _delta_angle)
{
	cout << "Pushing QR \"" << _label << "\" as recognized !!!" << endl;
	this->getLandmark(_label)->recognize(_distance, _delta_angle);
}

int WorldKB::getCameraAngle() {
	return this->cameraAngle;
}

void WorldKB::setCameraAngle(int _camera_angle) {
	this->cameraAngle = _camera_angle;
}

void WorldKB::incrementCameraAngle(int angle)  {
	this->setCameraAngle(this->getCameraAngle() + angle);
}

bool WorldKB::isQRInKB(string label, bool* isRecognized) {
	string tmp;
	*isRecognized = false;
	vector<Landmark>::iterator it = this->kb.begin();
	while(it != this->kb.end()) {
		tmp = string((*it).getLabel());
		if(label.compare(tmp)==0) {
			if((*it).isRecognized())
				*isRecognized = true;
			return true;
		}
		it++;
	}
	return false;
}

Landmark* WorldKB::getLandmark(string label)
{
	string tmp;
	Landmark lm;
	vector<Landmark>::iterator it = this->kb.begin();
	while(it != this->kb.end()) {
		tmp = string((*it).getLabel());
		if(label.compare(tmp)==0)
			return &(*it);
		it++;
	}
	return NULL;
}

Landmark* WorldKB::getLandmark(unsigned int pos)
{
	if( (pos >= this->kb.size()) || (pos < 0))
		return NULL;
	return &(this->kb.at(pos));
}

void WorldKB::parseStaticKB(string filename)
{
	ifstream in(filename.c_str());
	vector<string> lines;

	while(in){
		string line;
		getline(in,line);
		if(line.size()>0)
			lines.push_back(line);
	}

	string str, str1, s, delimiter = ",", token, parsed[3];
	int pos1, pos2, count;
	Landmark temp;
	size_t pos = 0;
	vector<string>::iterator it = lines.begin();

	while(it != lines.end())
	{
		count=0;

		str = (*it);
		pos1 = str.find("(")+1;
		str1 = str.substr (pos1);
		pos2 = str.find(")");
		s = str1.substr (0,pos2-pos1);

		while ((pos = s.find(delimiter)) != string::npos) {
			token = s.substr(0, pos);
			parsed[count]=token;
			s.erase(0, pos + delimiter.length());
			count++;
		}
		parsed[count]=s;

		Point2D tmpPoint2D;
		tmpPoint2D.x = atof(parsed[1].c_str());
		tmpPoint2D.y = atof(parsed[2].c_str());
		Landmark tmpLandmark(tmpPoint2D, parsed[0]);
		this->kb.push_back(tmpLandmark);
		it++;
	}
}

void WorldKB::parseParameters(string filename)
{
	cout << "Parsing Parameters."<<endl;
	ifstream in(filename.c_str());
	vector<string> lines;

	while(in){
		string line;
		getline(in, line);
		if(line.size()>0 && line.c_str()[0]!='#')
			lines.push_back(line);
	}

	string str, s;
	int pos1, i;
	i = 0;
	vector<string>::iterator it = lines.begin();

	while(it != lines.end()) {
		str = (*it);
		pos1 = str.find("=")+1;
		s = str.substr(pos1);
		const char* val = s.c_str();
		switch(i) {
			case 0:
				this->p_cameraID = atoi(val); break;
			case 1:
				this->p_startAngle = atoi(val); break;
			case 2:
				this->p_stepAngle_rough = atoi(val); break;
			case 3:
				this->p_endAngle = atoi(val); break;
			case 4:
				this->p_stepSleep_rough = atof(val); break;
			case 5:
				this->p_startSleep = atof(val); break;
			case 6:
				this->p_centerTolerance = atoi(val); break;
			case 7:
				this->p_Ntry_rough = atoi(val); break;
			case 8:
				this->p_stepAngle_fine = atoi(val); break;
			case 9:
				this->p_stepSleep_fine = atof(val); break;
			case 10:
				this->p_Ntry_fine = atoi(val); break;
			case 11:
				this->p_bwTresh = atoi(val); break;
			default:
				cout << "Some error occured while parsing parameters." << endl;
		}
		it++;
		i++;
	}
}

int WorldKB::getpCameraID()
{
	return this->p_cameraID;
}
int WorldKB::getpBwTresh()
{
	return this->p_bwTresh;
}
float WorldKB::getpStartSleep()
{
	return this->p_startSleep;
}
float WorldKB::getpStepSleep(bool mode)
{
	if(mode==ROUGH)
		return this->p_stepSleep_rough;
	else
		return this->p_stepSleep_fine;
}
int WorldKB::getpStartAngle()
{
	return this->p_startAngle;
}
int WorldKB::getpStepAngle(bool mode)
{
	if(mode==ROUGH)
		return this->p_stepAngle_rough;
	else
		return this->p_stepAngle_fine;
}
int WorldKB::getpEndAngle()
{
	return this->p_endAngle;
}
int WorldKB::getpCenterTolerance()
{
	return this->p_centerTolerance;
}
int WorldKB::getpNtry(bool mode)
{
	if(mode==ROUGH)
		return this->p_Ntry_rough;
	else
		return this->p_Ntry_fine;
}

void WorldKB::printKB()
{
	cout << "Printing KB" << endl;
	vector<Landmark>::iterator it = this->kb.begin();

	while(it != this->kb.end()) {
		(*it).print();
		it++;
	}
}

bool WorldKB::isInRange()
{
	//cout << "check in range: " << this->p_startAngle <<", " << this->cameraAngle <<", " << this->p_endAngle << endl;
	return (this->p_startAngle <= this->cameraAngle && this->cameraAngle < this->p_endAngle);
}

void WorldKB::triangleTest()
{
	// exact angles does NOT care, since core algorithm just computes the difference between them
	/** test 1 PASSED : R(x,y) = (2,1) 	| L(x,y,dist,angle) = (A, 1, 5, 4.12311, 0)   ; (B, 4, 4, 3.60555, 47.72631) */
	this->getLandmark(0)->recognize(4.12311, 0);
	this->getLandmark(1)->recognize(3.60555, 47.72631);
	/** test 2 PASSED : R(x,y) = (4,2) 	| L(x,y,dist,angle) = (C, 3, 3, 1.41421, -45) ;	(D, 5, 4, 2.23606, 26.56505) */
	this->getLandmark(2)->recognize(1.41421, -45);
	this->getLandmark(3)->recognize(2.23606, 26.56505);
}
