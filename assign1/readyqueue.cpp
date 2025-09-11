#include <iostream>
#include "readyqueue.h"

using namespace std;

//You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
// Remember to add sufficient comments to your code
int ReadyQueue::getPriority(const PCB* p) {
    if (!p) return 1;
    int pr = static_cast<int>(p->priority); // PCB currently exposes public 'priority'
    if (pr < 1)  pr = 1;
    if (pr > 50) pr = 50;
    return pr;
}

void ReadyQueue::swap(PCB*& a, PCB*& b) {
    PCB* t = a; a = b; b = t;
}

void ReadyQueue::ensureCapacity() {
    if (size_ < capacity) return;
    int newCap = (capacity == 0 ? 16 : capacity * 2);
    PCB** newHeap = new PCB*[newCap];
    for (int i = 0; i < size_; ++i) newHeap[i] = heap_[i];
    for (int i = size_; i < newCap; ++i) newHeap[i] = nullptr;
    delete[] heap_;
    heap_ = newHeap;
    capacity = newCap;
}

void ReadyQueue::siftUp(int idx) {
    // 0-based max-heap
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (getPriority(heap_[idx]) > getPriority(heap_[parent])) {
            swap(heap_[idx], heap_[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

void ReadyQueue::siftDown(int idx) {
        while (true) {
        int left  = idx * 2 + 1;
        int right = idx * 2 + 2;
        int largest = idx;

        if (left  < size_ && getPriority(heap_[left])  > getPriority(heap_[largest])) largest = left;
        if (right < size_ && getPriority(heap_[right]) > getPriority(heap_[largest])) largest = right;

        if (largest == idx) break;           // early-exit first
        swap(heap_[idx], heap_[largest]);
        idx = largest;
    }

}

/**
 * @brief Constructor for the ReadyQueue class.
 */
 ReadyQueue::ReadyQueue() : heap_(nullptr), size_(0), capacity(0)  {
     //TODO: add your code here
     ensureCapacity();
 }

/**
 *@brief Destructor
*/
ReadyQueue::~ReadyQueue() {
    //TODO: add your code to release dynamically allocate memory
    // Do NOT delete PCB objects (owned by PCBTable). Only free our array.
    delete[] heap_;
    heap_ = nullptr;
    size_ = 0;
    capacity = 0;
}

/**
 * @brief Add a PCB representing a process into the ready queue.
 *
 * @param pcbPtr: the pointer to the PCB to be added
 */
void ReadyQueue::addPCB(PCB *pcbPtr) {
    //TODO: add your code here
    // When adding a PCB to the queue, you must change its state to READY.
   if (!pcbPtr) {
    cerr << "[DEBUG] Tried to add nullptr PCB" << endl;
    return;
   }

   // Debug print
    cerr << "[DEBUG] Adding PCB id=" << pcbPtr->id
         << " prio=" << pcbPtr->priority
         << " current size=" << size_ << endl;

    pcbPtr->setState(ProcState::READY);

    ensureCapacity();

    ++size_;                           // make room
    heap_[size_ - 1] = pcbPtr;         // place PCB at new last slot
    siftUp(size_ - 1);                 // restore heap property
}

/**
 * @brief Remove and return the PCB with the highest priority from the queue
 *
 * @return PCB*: the pointer to the PCB with the highest priority
 */
PCB* ReadyQueue::removePCB() {
    //TODO: add your code here
    // When removing a PCB from the queue, you must change its state to RUNNING.
    if (size_ == 0) return nullptr;

    PCB* top = heap_[0];

    // Debug print
    cerr << "[DEBUG] Removing PCB id=" << (top ? top->id : -1)
         << " prio=" << (top ? top->priority : -1)
         << " size before=" << size_ << endl;

    // Move last element to root and restore heap order
    --size_;
    heap_[0] = heap_[size_];
    heap_[size_] = nullptr;

    if (size_ > 0) siftDown(0);

    // Set state to RUNNING when removed to run on CPU
    if (top) top->setState(ProcState::RUNNING);

    return top;
}

/**
 * @brief Returns the number of elements in the queue.
 *
 * @return int: the number of PCBs in the queue
 */
int ReadyQueue::size() {
    //TODO: add your code here
    return size_;
}

/**
 * @brief Display the PCBs in the queue.
 */
void ReadyQueue::displayAll() {
    //TODO: add your code here
    // Print highest→lowest priority WITHOUT modifying the real heap.
    if (size_ == 0) return;

    // Copy current heap
    PCB** tmp = new PCB*[size_];
    for (int i = 0; i < size_; ++i) tmp[i] = heap_[i];
    int tmpSize = size_;

    auto tmpGetPrio = [&](int i) -> int {
        if (!tmp[i]) return 1;
        int pr = static_cast<int>(tmp[i]->priority);
        if (pr < 1)  pr = 1;
        if (pr > 50) pr = 50;
        return pr;
    };
    auto tmpSwap = [&](int a, int b) {
        PCB* t = tmp[a]; tmp[a] = tmp[b]; tmp[b] = t;
    };
    auto tmpSiftDown = [&](int idx) {
        while (true) {
            int left  = idx * 2 + 1;
            int right = idx * 2 + 2;
            int largest = idx;
            if (left  < tmpSize && tmpGetPrio(left)  > tmpGetPrio(largest)) largest = left;
            if (right < tmpSize && tmpGetPrio(right) > tmpGetPrio(largest)) largest = right;
            if (largest != idx) { tmpSwap(idx, largest); idx = largest; }
            else break;
        }
    };

    // Heapify copy (it’s already a heap, but safe if future changes touch invariants)
    for (int i = (tmpSize / 2) - 1; i >= 0; --i) tmpSiftDown(i);

    // Pop max repeatedly and print
    while (tmpSize > 0) {
        PCB* top = tmp[0];
        cout << "ID=" << top->id << " Priority=" << tmpGetPrio(0) << "\n";
        --tmpSize;
        tmp[0] = tmp[tmpSize];
        tmp[tmpSize] = nullptr;
        if (tmpSize > 0) tmpSiftDown(0);
    }

    delete[] tmp;
}