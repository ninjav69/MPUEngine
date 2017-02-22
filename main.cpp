#include <cstdlib>
#include <string>
#include <iostream>

using namespace std;

class Serial {
public:
    void print(string str);
    void print(char ch);
};


void Serial::print(string str) {
    cout << str;
}

void Serial::print(char ch) {
    cout << (int)ch;
}

Serial port;

class MPU {
public:
    MPU();
    MPU(char id);

    void update();
    void transmit(Serial port);
private:
    char id;
    char ypr[3];
};

MPU::MPU() {
    MPU(0);
}

MPU::MPU(char id) {
    this->id = id;
    ypr[0] = 0;
    ypr[1] = 0;
    ypr[2] = 0;
}

void MPU::update() {
    cout << "Reading the status of the sensor" << endl;
    ypr[0] = rand() % 360;
    ypr[1] = rand() % 360;
    ypr[2] = rand() % 360;
}

void MPU::transmit(Serial port) {
    port.print(id);
    port.print(":");
    port.print(ypr[0]);
    port.print(":");
    port.print(ypr[1]);
    port.print(":");
    port.print(ypr[2]);
    port.print("\n");
}

#define MPU_COUNT 2
MPU mpus[MPU_COUNT];

#define QUEUE_SIZE 50
class Work {
public:
    Work();

    MPU *get();
    void put(MPU* mpu);
    char size();
    
private:
    MPU* queue[QUEUE_SIZE];
    char head;
    char tail;
};

Work::Work() {
    this->head = 0;
    this->tail = 0;
}

MPU* Work::get() {
    return queue[head++];
    if (head >= QUEUE_SIZE) {
        head = 0;
    }
}

char Work::size() {
    return tail - head;
}

void Work::put(MPU* mpu) {
    queue[tail++] = mpu;
    if (tail >= QUEUE_SIZE) {
        tail = 0;
    }
}

Work queue;

// Interrupt handlers
void interruptMpu1() {
    queue.put(&mpus[0]);
}

void interruptMpu2() {
    queue.put(&mpus[1]);
}

void init() {
    mpus[0] = MPU(1);
    mpus[1] = MPU(2);
}

void loop() {
    while (queue.size() > 0) {
        MPU* mpu = queue.get();
        mpu->update();
        mpu->transmit(port);
    }
}

int main(int argc, char** argv) {
    init();

    // Fake some work
    interruptMpu1();
    interruptMpu2();
    interruptMpu1();
    interruptMpu1();
    interruptMpu2();

    loop();
    return 0;
}

