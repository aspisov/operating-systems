#include <iostream>
#include <vector>
#include <semaphore.h>
#include <thread>

class CircularBuffer {
private:
    std::vector<int> buffer;
    size_t capacity;
    size_t head = 0;
    size_t tail = 0;
    sem_t emptySlots;
    sem_t fullSlots;
    sem_t mutexWriter;
    sem_t mutexReader;

public:
    CircularBuffer(size_t size) : capacity(size), buffer(size) {
        sem_init(&emptySlots, 0, capacity);
        sem_init(&fullSlots, 0, 0);
        sem_init(&mutexWriter, 0, 1);
        sem_init(&mutexReader, 0, 1);
    }

    ~CircularBuffer() {
        sem_destroy(&emptySlots);
        sem_destroy(&fullSlots);
        sem_destroy(&mutexWriter);
        sem_destroy(&mutexReader);
    }

    void write(int item) {
        sem_wait(&emptySlots);
        sem_wait(&mutexWriter);

        buffer[tail] = item;
        tail = (tail + 1) % capacity;

        sem_post(&mutexWriter);
        sem_post(&fullSlots);
    }

    int read() {
        sem_wait(&fullSlots);
        sem_wait(&mutexReader);

        int item = buffer[head];
        head = (head + 1) % capacity;

        sem_post(&mutexReader);
        sem_post(&emptySlots);
        return item;
    }
};

void writer(CircularBuffer& cb) {
    for (int i = 0; i < 100; ++i) {
        cb.write(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void reader(CircularBuffer& cb, int readerId) {
    for (int i = 0; i < 50; ++i) {
        int item = cb.read();
        std::cout << "Reader " << readerId << " read: " << item << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    CircularBuffer cb(10);

    std::thread writerThread(writer, std::ref(cb));
    std::thread readerThread1(reader, std::ref(cb), 1);
    std::thread readerThread2(reader, std::ref(cb), 2);

    writerThread.join();
    readerThread1.join();
    readerThread2.join();

    return 0;
}