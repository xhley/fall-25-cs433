/**
 * Assignment 1: priority queue of processes
 * @file pcbtable.h
 * @author Elias Estacion and Meliton Rojas
 * @brief This is the implementation file for the PCBTable class.
 * //You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
 * // Remember to add sufficient comments to your code
 */

#include "pcbtable.h"

/**
 * @brief Construct a new PCBTable object of the given size (number of PCBs)
 *
 * @param size: the capacity of the PCBTable
 */
PCBTable::PCBTable(int size)
   : pcbArray(nullptr), tableSize(size) {
    if (tableSize < 1) tableSize = 1;
    pcbArray = new PCB*[tableSize];
    for (int i = 0; i < tableSize; ++i) {
        pcbArray[i] = nullptr;
    }
}

/**
 * @brief Destroy the PCBTable object. Make sure to delete all the PCBs in the table.
 *
 */
PCBTable::~PCBTable() {
   if (pcbArray) {

    for (int i = 1; i <= tableSize; ++i) {
        delete pcbArray[i];
        pcbArray[i] = nullptr;
    }
    delete[] pcbArray;
    pcbArray = nullptr;
   }
   tableSize = 0;
}

/**
 * @brief Get the PCB at index "idx" of the PCBTable.
 *
 * @param idx: the index of the PCB to get
 * @return PCB*: pointer to the PCB at index "idx"
 */
PCB* PCBTable::getPCB(unsigned int idx) {
    if (!pcbArray || idx >= static_cast<unsigned int>(tableSize)) {
        return nullptr;
    }
    return pcbArray[idx];
}

/**
 * @brief Add a PCB pointer to the PCBTable at index idx.
 *
 * @param pcb: the PCB to add
 */
void PCBTable::addPCB(PCB *pcb, unsigned int idx) {
    if (!pcbArray || idx >= static_cast<unsigned int>(tableSize)) {
        delete pcb;
        return;
    }

    delete pcbArray[idx];

    pcbArray[idx] = pcb;
}
