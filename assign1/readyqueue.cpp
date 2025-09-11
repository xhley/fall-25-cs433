#include <iostream>
#include "readyqueue.h"

using namespace std;

//You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
// Remember to add sufficient comments to your code
int ReadyQueue::getPriority(const PCB* p) {
    if (p == nullptr) return -1; // lower than any valid [1..50] priority
    if (p->getState() != ProcState::READY) return -1;
    return static_cast<int>(p->getPriority());


}

void ReadyQueue::swap(PCB*& a, PCB*& b) {
    PCB* t = a; 
    a = b; 
    b = t;
}

void ReadyQueue::ensureCapacity() {
    if (size_ < capacity) return;  // still space available, nothing to do
    int newCap = (capacity == 0 ? 16 : capacity * 2);
    PCB** newHeap = new PCB*[newCap];

    // Copy over the old elements
    for (int i = 0; i < size_; ++i) {
        newHeap[i] = heap_[i];
    }

    // Initialize any extra slots to nullptr (optional, but safer)
    for (int i = size_; i < newCap; ++i) {
        newHeap[i] = nullptr;
    }

    // Free old storage
    delete[] heap_;

    heap_ = newHeap;
    capacity = newCap;
}

void ReadyQueue::shiftUp(int idx) {
    // 0-based max-heap
    // Continue until we reach the root (index 0)
    while (idx > 0) {
        int parent = (idx - 1) / 2; // parent index in 0-based heap

        // If current node has higher priority than its parent → swap
        if (getPriority(heap_[idx]) > getPriority(heap_[parent])) {
            swap(heap_[idx], heap_[parent]);
            idx = parent; // move up to parent's position
        } else {
            break; // heap property satisfied
        }
    }
}

void ReadyQueue::shiftDown(int idx) {
        while (true) {
        int left  = 2 * idx + 1; // left child
        int right = 2 * idx + 2; // right child
        int largest = idx;

        // Compare with left child
        if (left < size_ && getPriority(heap_[left]) > getPriority(heap_[largest])) {
            largest = left;
        }

        // Compare with right child
        if (right < size_ && getPriority(heap_[right]) > getPriority(heap_[largest])) {
            largest = right;
        }

        // If parent is larger than both children → heap property satisfied
        if (largest == idx) break;

        // Otherwise swap and continue down the tree
        swap(heap_[idx], heap_[largest]);
        idx = largest;
    }

}

/**
 * @brief Constructor for the ReadyQueue class.
 */
 ReadyQueue::ReadyQueue() : heap_(nullptr), size_(0), capacity(0)  {
     //TODO: add your code here
     // All slots are set to nullptr inside ensureCapacity().
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
   if (!pcbPtr) return; // ignore null inserts

    // Any process added to the ready queue must be marked READY
    pcbPtr->setState(ProcState::READY);

    // Make sure there is space in the heap
    ensureCapacity();

    // Insert at the end
    heap_[size_] = pcbPtr;

    // Restore heap property by sifting up
    siftUp(size_);

    // Update size
    ++size_;
}

/**
 * @brief Remove and return the PCB with the highest priority from the queue
 *
 * @return PCB*: the pointer to the PCB with the highest priority
 */
PCB* ReadyQueue::removePCB() {
    //TODO: add your code here
    // When removing a PCB from the queue, you must change its state to RUNNING.
    if (size_ == 0) {
        return nullptr; // queue empty
    }

    // Highest-priority PCB is always at the root (index 0)
    PCB* top = heap_[0];

    // Replace root with last element
    --size_;
    if (size_ > 0) {
        heap_[0] = heap_[size_];
        shiftDown(0); // restore heap property
    }

    // Mark this PCB as now RUNNING
    top->setState(ProcState::RUNNING);

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
    if (size_ == 0) {
        std::cout << "[ReadyQueue is empty]\n";
        return;
    }

    // Copy the current heap
    int tempSize = size_;
    PCB** tempHeap = new PCB*[tempSize];
    for (int i = 0; i < tempSize; ++i) tempHeap[i] = heap_[i];

    auto getP = [this](PCB* p) -> int {
        // same semantics as your ReadyQueue::getPriority (no <climits>)
        if (p == nullptr) return -1;
        if (p->getState() != ProcState::READY) return -1;
        return static_cast<int>(p->getPriority());
    };

    // Local sift-down that ONLY uses tempSize/tempHeap
    auto shiftDownTemp = [&](int idx) {
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int largest = idx;

            if (left < tempSize && getP(tempHeap[left]) > getP(tempHeap[largest])) {
                largest = left;
            }
            if (right < tempSize && getP(tempHeap[right]) > getP(tempHeap[largest])) {
                largest = right;
            }
            if (largest == idx) break;

            // Use your member swap to swap pointers
            swap(tempHeap[idx], tempHeap[largest]);
            idx = largest;
        }
    };

    std::cout << "[ReadyQueue contents: highest → lowest priority]\n";
    while (tempSize > 0) {
        // Extract max from temp heap
        PCB* top = tempHeap[0];
        if (top) top->display(); else std::cout << "(null)\n";

        // Move last to root and reduce temp size
        --tempSize;
        if (tempSize > 0) {
            tempHeap[0] = tempHeap[tempSize];
            shiftDownTemp(0);
        }
    }

    delete[] tempHeap;
}