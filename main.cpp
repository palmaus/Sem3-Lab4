#include <InternalTests.h>
#include <iostream>
#include <TestRunner.h>
#include "GUI.h"

void runAllTests() {
    TestRunner runner;

    runner.runTestGroup("SharedPtr Tests", {
        internal_tests::testSharedPtr
    });
    runner.runTestGroup("UniquePtr Tests", {
        internal_tests::testUniquePtr
    });
    runner.runTestGroup("WeakPtr Tests", {
        internal_tests::testWeakPtr
    });

    runner.runTestGroup("LinkedList Tests", {
        internal_tests::testLinkedList
    });

    runner.runTestGroup("DynamicArray Tests", {
        internal_tests::testDynamicArray
    });

    runner.runTestGroup("MutableListSequence Tests", {
        internal_tests::testMutableListSequence
    });

    runner.runTestGroup("MutableArraySequence Tests", {
        internal_tests::testMutableArraySequence
    });

    runner.runTestGroup("HashTableDictionary Tests", {
         internal_tests::testHashTableDictionary
    });

    runner.runTestGroup("DirectedGraph Tests", {
         internal_tests::testDirectedGraph
    });

    runner.runTestGroup("UndirectedGraph Tests", {
         internal_tests::testUndirectedGraph
    });

    runner.runTestGroup("Graph Algorithms Tests", { // Добавлена группа тестов для алгоритмов
        internal_tests::testMSTAlgorithm,
        internal_tests::testDijkstraAlgorithm,
        internal_tests::testConnectedComponentsAlgorithm,
        internal_tests::testStronglyConnectedComponentsAlgorithm,
    });

    runner.runTestGroup("HashTable Tests", {
         internal_tests::testHashTable
    });

    runner.runTestGroup("HashTableDictionary Tests", {
         internal_tests::testHashTableDictionary
    });

    runner.printResults();
}

int main(int argc, char* argv[]) {
    int testMode = 1;
    if (testMode == 1) {
        runAllTests();
    } else {
        QApplication app(argc, argv);
        GraphVisualizer visualizer;
        visualizer.show();
        return app.exec();
    }
}