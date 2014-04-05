#include "parser.hxx"
#include <iostream>
#include <cstdlib>

void Parser::Parse(std::istream& stream, Beam::vector& beams, Sensor::vector& sensors)
{
    std::streampos pos;
#define ERROR(MSG) do{ \
    std::cerr << "Syntax error around file position " << pos << ". " << MSG << std::endl; \
    exit(2); \
}while(0)
#define INPUT \
    pos = stream.tellg(); \
    stream
    while(!stream.eof()) {
        std::string s;
        INPUT >> s;
        if(s.compare("BUILDING") == 0) {
            Point3D p1, p2;
            INPUT >> p1.x >> p1.y >> p1.z;
            if(!stream) ERROR("Failed to read coordinates");
            INPUT >> p2.x >> p2.y >> p2.z;
            if(!stream) ERROR("Failed to read coordinates");
            beams.push_back(Beam(p1, p2));
        } else if(s.compare("SENSOR") == 0) {
            std::string stype;
            Point3D p;
            float range;
            Sensor::SensorType t;
            INPUT >> stype;
            if(!stream) ERROR("Failed to read sensor type");
            if(stype.compare("CENTRAL") == 0) {
                t = Sensor::CENTRAL;
            } else if(stype.compare("ROUTER") == 0) {
                t = Sensor::ROUTER;
            } else if(stype.compare("SENSOR") == 0) {
                t = Sensor::SENSOR;
            } else {
                ERROR("Expecting CENTRAL|ROUTER|SENSOR");
            }
            INPUT >> p.x >> p.y >> p.z;
            if(!stream) ERROR("Failed to read coordinates");
            INPUT >> range;
            if(!stream) ERROR("Failed to read range");
            sensors.push_back(Sensor(p, t, range));
        } else if(s.compare("REM") == 0) {
            do {
                int c = stream.get();
                if(stream.eof()) break;
                if((char)c == '\n') break;
            } while(1);
        } else {
            if(stream.eof()) return;

            ERROR("Expecting BUILDING|SENSORS|REM");
            exit(2);
        }
    }
}
