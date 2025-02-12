#pragma once
#include <string>

template<typename T>
class LinkedList;

template<typename T>
class DynamicArray;

namespace internal_tests {
    template<typename T>
    bool linkedListsEqual(const LinkedList<T> *list1, const LinkedList<T> *list2);
    template<typename T>
    bool dynamicArraysEqual(const DynamicArray<T> *arr1, const DynamicArray<T> *arr2);

    void testHashTable();
    void testUndirectedGraph();
    void testDirectedGraph();
    void testDijkstraAlgorithm();
    void testMSTAlgorithm();
    void testConnectedComponentsAlgorithm();
    void testStronglyConnectedComponentsAlgorithm();
    void testHashTableDictionary();
    void testMutableArraySequence();
    void testDynamicArray();
    void testLinkedList();
    void testMutableListSequence();
    void testSharedPtr();
    void testUniquePtr();
    void testWeakPtr();
    void testGraphPath();
}