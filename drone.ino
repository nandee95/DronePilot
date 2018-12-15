#include <Servo.h>
#include <SFML_Packet.h>

#define SERIAL_BAUD 115200
#define ESC_MIN_THROTTLE 1000
#define ESC_MAX_THROTTLE 2000

class Comm
{
public:
    void initialize()
    {
        Serial.begin(SERIAL_BAUD);
    }

    size_t available()
    {
        return Serial.available();
    }

    void read(char* data,size_t len)
    {
        Serial.readBytes((char*)data,len);
    }

    void write(byte* data,int len)
    {
        Serial.write(data,len);
    }
};

class ESC : public Servo
{
public:
    void calibrate(int pin)
    {
        this->attach(pin,ESC_MIN_THROTTLE,ESC_MAX_THROTTLE);
    }
    void set(int percent)
    {
        this->writeMicroseconds(map(percent,0,100,ESC_MIN_THROTTLE,ESC_MAX_THROTTLE));
    }

};

enum Protocol
{
    Protocol_Error,
    Protocol_Ping
};


class Drone
{
private:
    ESC esc;
    Comm comm;
public:


    void initialize()
    {
        comm.initialize();
        esc.calibrate(PB6);
    }

    void run()
    {
        if(comm.available() > 0)
        {
            unsigned char* data;
            int len = comm.available();
            data = new unsigned char[len];
            comm.read((char*)data,len);
            sf::Packet packet(data,len);
            unsigned char protocol = 0;
            packet >> protocol;
            if(protocol == 1)
            {
              comm.write(data,len);
            }
             if(protocol == 2)
            {
              unsigned char throttle;
              packet >> throttle;
              esc.set(throttle);
              digitalWrite(PC13,throttle>50);
            }
        }
    }
};


Drone drone;

void setup() {
    drone.initialize();
    pinMode(PC13,OUTPUT);
}

void loop() {
    drone.run();
}
