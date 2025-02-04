#include "InternalTests.h"

#include <ConnectedComponentsAlgorithm.h>
#include <DijkstraAlgorithm.h>
#include <map>
#include <MSTAlgorithm.h>
#include <qstring.h>
#include <StronglyConnectedComponentsAlgorithm.h>
#include <TopologicalSortAlgorithm.h>

#include "DictionaryIterator.h"
#include "HashTable.h"
#include "IDictionary.h"
#include "HashTableDictionary.h"
#include "DirectedGraph.h"
#include "UndirectedGraph.h"
#include "Vertex.h"

#include "Student.h"
#include "TestRunner.h"
#include "MutableListSequence.h"
#include "UniquePtr.h"
#include "SharedPtr.h"
#include "WeakPtr.h"
#include "LinkedList.h"
#include "DynamicArray.h"
#include "MutableArraySequence.h"



namespace internal_tests {
    // Вспомогательная функция для сравнения LinkedList
    template<typename T>
    bool linkedListsEqual(const LinkedList<T> *list1, const LinkedList<T> *list2) {
        if (list1->getLength() != list2->getLength()) {
            return false;
        }
        for (int i = 0; i < list1->getLength(); ++i) {
            if (list1->getByIndex(i) != list2->getByIndex(i)) {
                return false;
            }
        }
        return true;
    }


    // Вспомогательная функция для сравнения DynamicArray
    template<typename T>
    bool dynamicArraysEqual(const DynamicArray<T> *arr1, const DynamicArray<T> *arr2) {
        if (arr1->getSize() != arr2->getSize()) {
            return false;
        }
        for (int i = 0; i < arr1->getSize(); ++i) {
            if (arr1->getByIndex(i) != arr2->getByIndex(i)) {
                return false;
            }
        }
        return true;
    }

    // Вспомогательная функция для сравнения Sequence
    template<typename T>
    bool sequencesEqual(const MutableSequence<T> *seq1, const MutableSequence<T> *seq2) {
        if (seq1->getLength() != seq2->getLength()) {
            return false;
        }
        for (int i = 0; i < seq1->getLength(); ++i) {
            if (seq1->get(i) != seq2->get(i)) {
                return false;
            }
        }
        return true;
    }

    void testHashTable() {
        TestRunner runner;

        // Тест: add и get
        runner.expectNoException("HashTable::add and get", []() {
            HashTable<int, std::string> hashTable;
            hashTable.add(1, "one");
            hashTable.add(2, "two");
            hashTable.add(3, "three");

            if (hashTable.get(1) != "one") throw std::runtime_error("Incorrect get(1).");
            if (hashTable.get(2) != "two") throw std::runtime_error("Incorrect get(2).");
            if (hashTable.get(3) != "three") throw std::runtime_error("Incorrect get(3).");
        });

        // Тест: Перезапись существующего элемента
        runner.expectNoException("HashTable::Overwrite existing element", []() {
            HashTable<int, std::string> hashTable;
            hashTable.add(1, "one");
            hashTable.add(1, "uno");
            if (hashTable.get(1) != "uno") throw std::runtime_error("Element not overwritten.");
        });

        // Тест: containsKey
        runner.expectNoException("HashTable::containsKey", []() {
            HashTable<int, std::string> hashTable;
            hashTable.add(1, "one");
            hashTable.add(2, "two");
            if (!hashTable.containsKey(1)) throw std::runtime_error("containsKey(1) returned false.");
            if (!hashTable.containsKey(2)) throw std::runtime_error("containsKey(2) returned false.");
            if (hashTable.containsKey(999)) throw std::runtime_error("containsKey(999) returned true.");
        });

       // Тест: Удаление элемента
       runner.expectNoException("HashTable::remove element", []() {
            HashTable<int, std::string> hashTable;
            hashTable.add(1, "one");
            hashTable.add(2, "two");
            hashTable.remove(2);
            if(hashTable.containsKey(2))
              throw std::runtime_error("containsKey(2) returned true after removing the element");
       });

        // Тест: Исключение при получении несуществующего элемента
        runner.expectException<std::runtime_error>("HashTable::get non-existent element", []() {
             HashTable<int, std::string> hashTable;
             hashTable.get(2);
        });

         // Тест: Исключение при удалении несуществующего элемента
         runner.expectException<std::runtime_error>("HashTable::remove non-existent element", []() {
             HashTable<int, std::string> hashTable;
             hashTable.remove(999);
        });
       // Тест: Итератор
        runner.expectNoException("HashTable::Iterator", []() {
              HashTable<int, std::string> hashTable;
              hashTable.add(1, "one");
              hashTable.add(2, "two");
              hashTable.add(3, "three");
              hashTable.add(4, "four");
              hashTable.add(5, "five");

              int count = 0;
              for (const auto& item : hashTable) {
                  count++;
              }
              if (count != 5) throw std::runtime_error("Incorrect iterator count");
       });

        // Тест: Итератор после удаления
        runner.expectNoException("HashTable::Iterator after remove", []() {
            HashTable<int, std::string> hashTable;
            hashTable.add(1, "one");
            hashTable.add(2, "two");
            hashTable.add(3, "three");
            hashTable.add(4, "four");
            hashTable.add(5, "five");
            hashTable.remove(5);
            int count = 0;
            for (const auto& item : hashTable) {
               count++;
            }
            if(count != 4) throw std::runtime_error("Incorrect iterator count after remove");
        });

        // Тест: Увеличение таблицы (резайз) - граничные случаи
        runner.expectNoException("HashTable::Resize table (boundary cases)", []() {
            // Пустая таблица
            HashTable<int, std::string> hashTableEmpty;
            if (hashTableEmpty.getCount() != 0) throw std::runtime_error("Incorrect count for empty table");
            if (hashTableEmpty.getCapacity() <= 0) throw std::runtime_error("Incorrect capacity for empty table");

            // Таблица с одним элементом
            HashTable<int, std::string> hashTableOne;
            hashTableOne.add(1, "one");
            if (hashTableOne.getCount() != 1) throw std::runtime_error("Incorrect count for table with one element");
            if (hashTableOne.getCapacity() <= 0) throw std::runtime_error("Incorrect capacity for table with one element");
        });

      // Тест: getAllItems
       runner.expectNoException("HashTable::getAllItems", [](){
            HashTable<int, std::string> hashTable;
            hashTable.add(1, "one");
            hashTable.add(2, "two");
            hashTable.add(3, "three");

            auto allItems = hashTable.getAllItems();
            if(allItems->getLength() != hashTable.getCount()) throw std::runtime_error("Incorrect length from getAllItems");
            for (int i = 0; i < allItems->getLength(); ++i) {
                auto item = allItems->get(i);
                if (hashTable.get(item.first) != item.second) throw std::runtime_error("Incorrect elements from getAllItems");
            }
       });

        // Тест: Удаление элемента из начала таблицы
        runner.expectNoException("HashTable::remove element from beginning", [](){
            HashTable<int, std::string> hashTable;
            hashTable.add(100, "hundred");
            hashTable.remove(100);
            if (hashTable.containsKey(100)) throw std::runtime_error("containsKey(100) returned true after removing the element from beginning.");
        });

        // Тест: Коллизии
        runner.expectNoException("HashTable::Collision handling", [](){
            struct CustomHash {
                int operator()(int key) const {
                    return key % 10; // Намеренно создаем коллизии
                }
            };
            HashTable<int, std::string, CustomHash> hashTableWithCollisions;
            hashTableWithCollisions.add(10, "ten");
            hashTableWithCollisions.add(20, "twenty");
            hashTableWithCollisions.add(30, "thirty");
            if (hashTableWithCollisions.get(10) != "ten") throw std::runtime_error("Incorrect get(10) after collisions.");
            if (hashTableWithCollisions.get(20) != "twenty") throw std::runtime_error("Incorrect get(20) after collisions.");
            if (hashTableWithCollisions.get(30) != "thirty") throw std::runtime_error("Incorrect get(30) after collisions.");
            hashTableWithCollisions.remove(20);
            if (hashTableWithCollisions.containsKey(20)) throw std::runtime_error("containsKey(20) after removing an element after collisions returned true.");
            if (hashTableWithCollisions.get(10) != "ten") throw std::runtime_error("Incorrect get(10) after removing.");
            if (hashTableWithCollisions.get(30) != "thirty") throw std::runtime_error("Incorrect get(30) after removing.");

         });

         // Тест: Перезапись при коллизиях
        runner.expectNoException("HashTable::Overwrite after collisions", [](){
            struct CustomHash {
               int operator()(int key) const {
                    return key % 10; // Намеренно создаем коллизии
                }
            };
            HashTable<int, std::string, CustomHash> hashTableWithCollisions;
            hashTableWithCollisions.add(30, "thirty");
            hashTableWithCollisions.add(30, "new_thirty");
            if (hashTableWithCollisions.get(30) != "new_thirty") throw std::runtime_error("Incorrect overwrite after collisions.");
        });

        // Тест: Удаление всех элементов
        runner.expectNoException("HashTable::remove all", [](){
            HashTable<int, std::string> hashTable;
            for (int i = 1; i < 30; ++i) {
                hashTable.add(i, "value" + std::to_string(i));
            }
            hashTable.removeAll();
            if (hashTable.getCount() != 0) throw std::runtime_error("Incorrect count after removing all elements.");
            for (int i = 1; i < 30; ++i) {
               if (hashTable.containsKey(i)) throw std::runtime_error("containsKey returned true after removing all elements");
            }
        });
    }

    void testHashTableDictionary() {
        TestRunner runner;

        // Тест создания словаря
        runner.expectNoException("HashTableDictionary::Create dictionary", []() {
            HashTableDictionary<int, std::string> dict;
        });

        // Тест добавления элементов
        runner.expectNoException("HashTableDictionary::Add elements", []() {
            HashTableDictionary<int, std::string> dict;
            dict.add(1, "one");
            dict.add(2, "two");
        });

        // Тест получения элементов
        runner.expectNoException("HashTableDictionary::Get element", []() {
            HashTableDictionary<int, std::string> dict;
            dict.add(1, "one");
            if (dict.get(1) != "one") throw std::runtime_error("Incorrect value for key 1");
        });

        // Тест containsKey
        runner.expectNoException("HashTableDictionary::containsKey", []() {
            HashTableDictionary<int, std::string> dict;
            dict.add(1, "one");
            if (!dict.containsKey(1)) throw std::runtime_error("Key 1 should be present");
            if (dict.containsKey(2)) throw std::runtime_error("Key 2 should not be present");
        });

        // Тест remove
        runner.expectNoException("HashTableDictionary::Remove element", []() {
            HashTableDictionary<int, std::string> dict;
            dict.add(1, "one");
            dict.remove(1);
            if (dict.containsKey(1)) throw std::runtime_error("Key 1 should have been removed");
        });

        // Тест getAllItems
        runner.expectNoException("HashTableDictionary::getAllItems", []() {
            HashTableDictionary<int, std::string> dict;
            dict.add(1, "one");
            dict.add(2, "two");
            auto items = dict.getAllItems();
            if (items->getLength() != 2) throw std::runtime_error("Incorrect number of items");
        });

        // Тест итератора
        runner.expectNoException("HashTableDictionary::Iterator", []() {
            HashTableDictionary<int, std::string> dict;
            dict.add(1, "one");
            dict.add(2, "two");
            int count = 0;
            for (auto it = dict.begin(); it != dict.end(); ++it) {
                count++;
            }
            if (count != 2) throw std::runtime_error("Incorrect iterator count");
        });
    }

    void testDirectedGraph() {
    TestRunner runner;

    // Тест создания графа
    runner.expectNoException("DirectedGraph::Create graph", []() {
        DirectedGraph<int> graph;
    });

    // Тест добавления вершин
    runner.expectNoException("DirectedGraph::Add vertices", []() {
        DirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
    });

    // Тест добавления ребер
    runner.expectNoException("DirectedGraph::Add edges", []() {
        DirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        IVertex* v3 = new Vertex(3);
        graph.addVertex(v1);
        graph.addVertex(v2);
        graph.addVertex(v3);
        graph.addEdge(v1, v2, 10);
        graph.addEdge(v2, v3, 5);
    });

    // Тест получения вершин
    runner.expectNoException("DirectedGraph::Get vertices", []() {
        DirectedGraph<int> graph;
         IVertex* v1 = new Vertex(1);
         IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
        auto vertices = graph.getVertices();
        if (vertices.getLength() != 2) {
            throw std::runtime_error("Incorrect number of vertices");
        }
    });

    // Тест получения ребер
    runner.expectNoException("DirectedGraph::Get edges", []() {
        DirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        IVertex* v3 = new Vertex(3);
        graph.addVertex(v1);
        graph.addVertex(v2);
        graph.addVertex(v3);
        graph.addEdge(v1, v2, 10);
        graph.addEdge(v1, v3, 5);
        auto edges = graph.getEdges(v1);
        if (edges.getLength() != 2) {
            throw std::runtime_error("Incorrect number of edges");
        }
    });

    // Тест hasVertex
    runner.expectNoException("DirectedGraph::hasVertex", []() {
        DirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        if (!graph.hasVertex(v1)) throw std::runtime_error("Vertex should be present");
        if (graph.hasVertex(v2)) throw std::runtime_error("Vertex should not be present");
    });

    // Тест hasEdge
    runner.expectNoException("DirectedGraph::hasEdge", []() {
        DirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
        graph.addEdge(v1, v2, 1);
        if (!graph.hasEdge(v1, v2)) throw std::runtime_error("Edge should be present");
        if (graph.hasEdge(v2, v1)) throw std::runtime_error("Edge should not be present (directed)");
        auto edges = graph.getEdges(v1);
    });

     // Тест getVertexById
        runner.expectNoException("DirectedGraph::getVertexById", []() {
            DirectedGraph<int> graph;
             IVertex* v1 = new Vertex(1);
            graph.addVertex(v1);
            IVertex* foundVertex = graph.getVertexById(1);
            if (!foundVertex) throw std::runtime_error("Vertex not found");
            if (foundVertex->getId() != 1) throw std::runtime_error("Incorrect vertex ID");
        });
    }

    void testUndirectedGraph() {
     TestRunner runner;

    // Тест создания графа
    runner.expectNoException("UndirectedGraph::Create graph", []() {
        UndirectedGraph<int> graph;
    });

    // Тест добавления вершин
    runner.expectNoException("UndirectedGraph::Add vertices", []() {
        UndirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
    });

    // Тест добавления ребер
    runner.expectNoException("UndirectedGraph::Add edges", []() {
        UndirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
        graph.addEdge(v1, v2, 10);
    });

    // Тест получения вершин
    runner.expectNoException("UndirectedGraph::Get vertices", []() {
        UndirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
        auto vertices = graph.getVertices();
        if (vertices.getLength() != 2) {
            throw std::runtime_error("Incorrect number of vertices");
        }
    });

    // Тест получения ребер
    runner.expectNoException("UndirectedGraph::Get edges", []() {
        UndirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
        graph.addEdge(v1, v2, 10);
        auto edges = graph.getEdges(v1);
        if (edges.getLength() != 1) {
            throw std::runtime_error("Incorrect number of edges");
        }
    });

    // Тест hasVertex
    runner.expectNoException("UndirectedGraph::hasVertex", []() {
        UndirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        if (!graph.hasVertex(v1)) throw std::runtime_error("Vertex should be present");
        if (graph.hasVertex(v2)) throw std::runtime_error("Vertex should not be present");
    });

    // Тест hasEdge
    runner.expectNoException("UndirectedGraph::hasEdge", []() {
        UndirectedGraph<int> graph;
        IVertex* v1 = new Vertex(1);
        IVertex* v2 = new Vertex(2);
        graph.addVertex(v1);
        graph.addVertex(v2);
        graph.addEdge(v1, v2, 1);
        if (!graph.hasEdge(v1, v2)) throw std::runtime_error("Edge should be present");
        if (!graph.hasEdge(v2, v1)) throw std::runtime_error("Edge should be present (undirected)");
    });

     // Тест getVertexById
        runner.expectNoException("UndirectedGraph::getVertexById", []() {
            UndirectedGraph<int> graph;
           IVertex* v1 = new Vertex(1);
            graph.addVertex(v1);
            IVertex* foundVertex = graph.getVertexById(1);
            if (!foundVertex) throw std::runtime_error("Vertex not found");
            if (foundVertex->getId() != 1) throw std::runtime_error("Incorrect vertex ID");
        });
    }

    MutableArraySequence<IVertex*> createVertices(std::vector<int> ids) {
        MutableArraySequence<IVertex*> vertices;
        for (int id : ids) {
            vertices.append(new Vertex(id));
        }
        return vertices;
    }

    DirectedGraph<int> createDirectedGraphForTests() {
        DirectedGraph<int> graph;
        MutableArraySequence<IVertex*> vertices = createVertices({1, 2, 3, 4, 5});
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            graph.addVertex(vertices.get(i));
        }
        graph.addEdge(vertices.get(0), vertices.get(1), 10); // 1 -> 2
        graph.addEdge(vertices.get(0), vertices.get(3), 5);  // 1 -> 4
        graph.addEdge(vertices.get(1), vertices.get(2), 1);  // 2 -> 3
        graph.addEdge(vertices.get(1), vertices.get(3), 2);  // 2 -> 4
        graph.addEdge(vertices.get(2), vertices.get(4), 4);  // 3 -> 5
        graph.addEdge(vertices.get(3), vertices.get(2), 3);  // 4 -> 3
        graph.addEdge(vertices.get(3), vertices.get(4), 2);  // 4 -> 5
        graph.addEdge(vertices.get(4), vertices.get(1), 1);  // 5 -> 2
        return graph;
    }

    UndirectedGraph<int> createUndirectedGraphForTests() {
        UndirectedGraph<int> graph;
        MutableArraySequence<IVertex*> vertices = createVertices({1, 2, 3, 4, 5});
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            graph.addVertex(vertices.get(i));
        }
        graph.addEdge(vertices.get(0), vertices.get(1), 4); // 1 -- 2
        graph.addEdge(vertices.get(0), vertices.get(2), 1); // 1 -- 3
        graph.addEdge(vertices.get(1), vertices.get(2), 2); // 2 -- 3
        graph.addEdge(vertices.get(1), vertices.get(3), 5); // 2 -- 4
        graph.addEdge(vertices.get(2), vertices.get(3), 1); // 3 -- 4
        graph.addEdge(vertices.get(3), vertices.get(4), 3); // 4 -- 5
        return graph;
    }

    void testDijkstraAlgorithm() {
        TestRunner runner;

        runner.expectNoException("DijkstraAlgorithm::Find shortest path", []() {
            DirectedGraph<int> graph = createDirectedGraphForTests();
            DijkstraAlgorithm<int> dijkstra;

            IVertex* startVertex = graph.getVertexById(1);
            IVertex* endVertex = graph.getVertexById(5);

            if (!startVertex || !endVertex) {
                throw std::runtime_error("Start or end vertex not found in graph.");
            }

            auto distances = dijkstra.execute(&graph, startVertex, endVertex);
            if (!distances) {
                throw std::runtime_error("Dijkstra's algorithm returned nullptr.");
            }
            std::map<int, int> expectedDistances = {
                {1, 0}, {2, 8}, {3, 8}, {4, 5}, {5, 7}
            };

            auto vertices = graph.getVertices();
            for (size_t i = 0; i < vertices.getLength(); ++i) {
                int vertexId = vertices.get(i)->getId();
                if (distances->get(i) != expectedDistances[vertexId]) {
                    throw std::runtime_error("Incorrect shortest path distance for vertex " + std::to_string(vertexId) +
                                           ", expected: " + std::to_string(expectedDistances[vertexId]) +
                                           ", actual: " + QString::number(distances->get(i)).toStdString());
                }
            }
        });
    }

    void testMSTAlgorithm() {
         TestRunner runner;
         runner.expectNoException("MSTAlgorithm::Find MST", []() {
              UndirectedGraph<int> graph = createUndirectedGraphForTests();
              MSTAlgorithm<int> mstAlgo;
              auto mstEdges = mstAlgo.execute(&graph);
              if(!mstEdges) {
                   throw std::runtime_error("MST Algorithm returned nullptr");
              }
         });
    }

    void testConnectedComponentsAlgorithm() {
         TestRunner runner;
         runner.expectNoException("ConnectedComponentsAlgorithm::Find CC", []() {
              UndirectedGraph<int> graph = createUndirectedGraphForTests();
              ConnectedComponentsAlgorithm<int> ccAlgo;
              auto components = ccAlgo.execute(&graph);
               if(!components) {
                    throw std::runtime_error("Connected Components Algorithm returned nullptr");
               }
        });
    }

    void testStronglyConnectedComponentsAlgorithm() {
         TestRunner runner;
          runner.expectNoException("StronglyConnectedComponentsAlgorithm::Find SCC", []() {
              DirectedGraph<int> graph = createDirectedGraphForTests();
              StronglyConnectedComponentsAlgorithm<int> sccAlgo;
               auto components = sccAlgo.execute(&graph);
                if(!components) {
                    throw std::runtime_error("Strongly Connected Components Algorithm returned nullptr");
                }
        });
    }

    void testLinkedList() {
        TestRunner runner;

        // Тест создания пустого списка
        runner.expectNoException("testLinkedList::Empty list creation", []() {
            LinkedList<int> list;
        });

        // Тест добавления элементов
        runner.expectNoException("testLinkedList::Appending elements", []() {
            LinkedList<int> list;
            list.append(1);
            list.append(2);
            list.append(3);
        });

        // Тест получения элементов
        runner.expectNoException("testLinkedList::Getting elements", []() {
            LinkedList<int> list;
            list.append(1);
            list.append(2);
            list.append(3);
            if (list.getFirst() != 1) throw std::runtime_error("Incorrect first element.");
            if (list.getLast() != 3) throw std::runtime_error("Incorrect last element.");
            if (list.getByIndex(1) != 2) throw std::runtime_error("Incorrect element at index 1.");
        });

        // Тест получения подсписка
        runner.expectNoException("testLinkedList::Getting sublist", []() {
            LinkedList<int> list;
            list.append(1);
            list.append(2);
            list.append(3);
            list.append(4);
            auto sublist = list.getSubList(1, 2);
            if (sublist->getLength() != 2) throw std::runtime_error("Incorrect sublist length.");
            if (sublist->getFirst() != 2) throw std::runtime_error("Incorrect first element of sublist.");
            if (sublist->getLast() != 3) throw std::runtime_error("Incorrect last element of sublist.");
        });

        // Тест вставки элемента
        runner.expectNoException("testLinkedList::Inserting element", []() {
            LinkedList<int> list;
            list.append(1);
            list.append(3);
            list.insertAt(1, 2);
            if (list.getByIndex(1) != 2) throw std::runtime_error("Incorrect element at index 1 after insertion.");
        });

        // Тест конкатенации
        runner.expectNoException("testLinkedList::Concatenating lists", []() {
            LinkedList<int> list1;
            list1.append(1);
            list1.append(2);
            LinkedList<int> list2;
            list2.append(3);
            list2.append(4);

            // Создаем ожидаемый список
            int expectedData[] = {1, 2, 3, 4};
            LinkedList<int> expectedList;
            for (int i : expectedData) {
                expectedList.append(i);
            }

            auto *concatenated = list1.concatenate(list2);
            if (!linkedListsEqual(concatenated, &expectedList))
                throw std::runtime_error("Incorrect concatenated list.");
            delete concatenated;
        });

        // Тест исключений
        runner.expectException<std::out_of_range>("testLinkedList::GetFirst from empty list", []() {
            LinkedList<int> list;
            list.getFirst();
        });
        runner.expectException<std::out_of_range>("testLinkedList::GetLast from empty list", []() {
            LinkedList<int> list;
            list.getLast();
        });
        runner.expectException<std::out_of_range>("testLinkedList::GetByIndex with invalid index", []() {
            LinkedList<int> list;
            list.append(1);
            list.getByIndex(1);
        });
        runner.expectException<std::out_of_range>("testLinkedList::GetSubList with invalid indexes", []() {
            LinkedList<int> list;
            list.append(1);
            list.getSubList(0, 1);
        });

        // Тест копирующего конструктора
        runner.expectNoException("testLinkedList::Copy constructor", []() {
            LinkedList<int> list1;
            list1.append(1);
            list1.append(3);
            list1.append(2);
            LinkedList<int> list2(list1); // Вызов копирующего конструктора
            if (!linkedListsEqual(&list1, &list2))
                throw std::runtime_error("Lists are not equal after copy construction.");
        });

        // Тест оператора присваивания
        runner.expectNoException("testLinkedList::Assignment operator", []() {
            LinkedList<int> list1;
            list1.append(1);
            list1.append(2);
            LinkedList<int> list2;
            list2 = list1; // Вызов оператора присваивания
            if (!linkedListsEqual(&list1, &list2))
                throw std::runtime_error("Lists are not equal after assignment.");
        });
    }

    void testDynamicArray() {
        TestRunner runner;

        // Тест создания пустого массива
        runner.expectNoException("testDynamicArray::Empty array creation", []() {
            DynamicArray<int> array;
        });

        // Тест создания массива заданного размера
        runner.expectNoException("testDynamicArray::Sized array creation", []() {
            DynamicArray<int> array(5);
        });

        // Тест создания массива из существующего массива
        runner.expectNoException("testDynamicArray::Array from existing array creation", []() {
            int data[] = {1, 2, 3};
            DynamicArray<int> array(data, 3);
        });

        // Тест получения размера массива
        runner.expectNoException("testDynamicArray::Getting array size", []() {
            DynamicArray<int> array(5);
            if (array.getSize() != 5) throw std::runtime_error("Incorrect array size.");
        });

        // Тест получения элемента по индексу
        runner.expectNoException("testDynamicArray::Getting element by index", []() {
            int data[] = {1, 2, 3};
            DynamicArray<int> array(data, 3);
            if (array.getByIndex(1) != 2) throw std::runtime_error("Incorrect element at index 1.");
        });

        // Тест изменения размера массива
        runner.expectNoException("testDynamicArray::Resizing array", []() {
            DynamicArray<int> array(3);
            array.setSize(5);
            if (array.getSize() != 5) throw std::runtime_error("Incorrect array size after resizing.");
        });

        // Тест вставки элемента
        runner.expectNoException("testDynamicArray::Inserting element", []() {
            DynamicArray<int> array(3);
            array.insertAt(1, 5);
            if (array.getByIndex(1) != 5) throw std::runtime_error("Incorrect element at index 1 after insertion.");
        });

        // Тест исключений
        runner.expectException<std::invalid_argument>("testDynamicArray::Creation with negative size", []() {
            DynamicArray<int> array(-1);
        });
        runner.expectException<std::out_of_range>("testDynamicArray::GetByIndex with invalid index", []() {
            DynamicArray<int> array;
            array.getByIndex(0);
        });
        runner.expectException<std::out_of_range>("testDynamicArray::InsertAt with invalid index", []() {
            DynamicArray<int> array;
            array.insertAt(1, 1);
        });

        // Тест установки некорректного размера массива (отрицательный)
        runner.expectException<std::invalid_argument>("testDynamicArray::Setting invalid array size (negative)", []() {
            DynamicArray<int> array;
            array.setSize(-1);
        });

        // Тест копирующего конструктора
        runner.expectNoException("testDynamicArray::Copy constructor", []() {
            DynamicArray<int> array1(3);
            array1.insertAt(0, 1);
            array1.insertAt(1, 2);
            array1.insertAt(2, 3);
            DynamicArray<int> array2(array1); // Вызов копирующего конструктора
            if (!dynamicArraysEqual(&array1, &array2))
                throw std::runtime_error("Arrays are not equal after copy construction.");
        });

        // Тест оператора присваивания
        runner.expectNoException("testDynamicArray::Assignment operator", []() {
            DynamicArray<int> array1(3);
            array1.insertAt(0, 1);
            array1.insertAt(1, 2);
            array1.insertAt(2, 3);
            DynamicArray<int> array2;
            array2 = array1; // Вызов оператора присваивания
            if (!dynamicArraysEqual(&array1, &array2))
                throw std::runtime_error("Arrays are not equal after assignment.");
        });
    }

    void testMutableArraySequence() {
        TestRunner runner;

        // Тест создания пустой последовательности
        runner.expectNoException("testMutableArraySequence::Empty sequence creation", []() {
            MutableArraySequence<int> seq;
        });

        // Тест создания последовательности из существующего массива
        runner.expectNoException("testMutableArraySequence::Sequence from existing array creation", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
        });

        // Тест добавления элементов
        runner.expectNoException("testMutableArraySequence::Appending elements", []() {
            MutableArraySequence<int> seq;
            seq.append(1);
            seq.append(2);
            seq.append(3);
        });

        // Тест получения элементов
        runner.expectNoException("testMutableArraySequence::Getting elements", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            if (seq.getFirst() != 1) throw std::runtime_error("Incorrect first element.");
            if (seq.getLast() != 3) throw std::runtime_error("Incorrect last element.");
            if (seq.get(1) != 2) throw std::runtime_error("Incorrect element at index 1.");
        });

        // Тест вставки элемента
        runner.expectNoException("testMutableArraySequence::Inserting element", []() {
            int data[] = {1, 3};
            MutableArraySequence<int> seq(data, 2);
            seq.insertAt(1, 2);
            if (seq.get(1) != 2) throw std::runtime_error("Incorrect element at index 1 after insertion.");
        });

        // Тест конкатенации
        runner.expectNoException("testMutableArraySequence::Concatenating sequences", []() {
            int data1[] = {1, 2};
            MutableArraySequence<int> seq1(data1, 2);
            int data2[] = {3, 4};
            seq1.concat(SharedPtr<MutableSequence<int>>(new MutableArraySequence<int>(data2, 2)));
            int expectedData[] = {1, 2, 3, 4};
            MutableArraySequence<int> expectedSeq(expectedData, 4);
            if (!sequencesEqual(&seq1, &expectedSeq))
                throw std::runtime_error("Incorrect concatenated sequence.");
        });

        // Тесты на пустые последовательности
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetFirst from empty sequence", []() {
            MutableArraySequence<int> seq;
            seq.getFirst();
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetLast from empty sequence", []() {
            MutableArraySequence<int> seq;
            seq.getLast();
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::Get from empty sequence", []() {
            MutableArraySequence<int> seq;
            seq.get(0);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetSubSequence from empty sequence", []() {
            MutableArraySequence<int> seq;
            seq.getSubSequence(0, 0);
        });

        // Тесты на отрицательные индексы
        runner.expectException<std::out_of_range>("testMutableArraySequence::Get with negative index", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.get(-1);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::InsertAt with negative index", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.insertAt(-1, 4);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetSubSequence with negative start index", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.getSubSequence(-1, 1);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetSubSequence with negative end index", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.getSubSequence(0, -1);
        });

        // Тесты на индексы вне диапазона
        runner.expectException<std::out_of_range>("testMutableArraySequence::Get with index out of range", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.get(3);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::InsertAt with index out of range", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.insertAt(4, 4);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetSubSequence with start index out of range", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.getSubSequence(3, 2);
        });
        runner.expectException<std::out_of_range>("testMutableArraySequence::GetSubSequence with end index out of range", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);
            seq.getSubSequence(0, 4);
        });

        // Тест копирования мутабельной последовательности
        runner.expectNoException("testMutableArraySequence::Copy mutable sequence", []() {
            int data[] = {1, 2};
            MutableArraySequence<int> seq1(data, 2);
            MutableArraySequence<int> seq2(seq1);

            // Проверяем, что seq2 - копия seq1
            if (!sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Sequences are not equal after copy.");

            // Изменяем seq2 и проверяем, что seq1 не изменилась
            seq2.append(3);
            if (sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Original sequence changed after modifying the copy.");
        });

        // Тест присваивания мутабельной последовательности
        runner.expectNoException("testMutableArraySequence::Assign mutable sequence", []() {
            int data[] = {1, 2};
            MutableArraySequence<int> seq1(data, 2);

            MutableArraySequence<int> seq2;
            seq2 = seq1;

            // Проверяем, что seq2 - копия seq1
            if (!sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Sequences are not equal after assignment.");

            // Изменяем seq2 и проверяем, что seq1 не изменилась
            seq2.append(3);
            if (sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Original sequence changed after modifying the assigned sequence.");
        });

        // Тест методов tryGet, tryGetFirst, tryGetLast
        runner.expectNoException("testMutableArraySequence::TryGet methods", []() {
            int data[] = {1, 2, 3};
            MutableArraySequence<int> seq(data, 3);

            // tryGet
            auto opt1 = seq.tryGet(1);
            if (opt1.isNone()) throw std::runtime_error("tryGet(1) returned None for existing element.");
            if (opt1.getValue() != 2) throw std::runtime_error("tryGet(1) returned incorrect value.");

            auto opt2 = seq.tryGet(3);
            if (opt2.isSome()) throw std::runtime_error("tryGet(3) returned Some for non-existing element.");

            // tryGetFirst
            auto opt3 = seq.tryGetFirst();
            if (opt3.isNone()) throw std::runtime_error("tryGetFirst returned None for non-empty sequence.");
            if (opt3.getValue() != 1) throw std::runtime_error("tryGetFirst returned incorrect value.");

            // tryGetLast
            auto opt4 = seq.tryGetLast();
            if (opt4.isNone()) throw std::runtime_error("tryGetLast returned None for non-empty sequence.");
            if (opt4.getValue() != 3) throw std::runtime_error("tryGetLast returned incorrect value.");
        });
    }

    void testMutableListSequence() {
        TestRunner runner;

        // Тест создания пустой последовательности
        runner.expectNoException("testMutableListSequence::Empty sequence creation", []() {
            MutableListSequence<int> seq;
        });

        // Тест создания последовательности из существующего массива
        runner.expectNoException("testMutableListSequence::Sequence from existing array creation", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
        });

        // Тест добавления элементов
        runner.expectNoException("testMutableListSequence::Appending elements", []() {
            MutableListSequence<int> seq;
            seq.append(1);
            seq.append(2);
            seq.append(3);
        });

        // Тест получения элементов
        runner.expectNoException("testMutableListSequence::Getting elements", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
            if (seq.getFirst() != 1) throw std::runtime_error("Incorrect first element.");
            if (seq.getLast() != 3) throw std::runtime_error("Incorrect last element.");
            if (seq.get(1) != 2) throw std::runtime_error("Incorrect element at index 1.");
        });

        // Тест получения подпоследовательности
        runner.expectNoException("testMutableListSequence::Getting subsequence", []() {
            int data[] = {1, 2, 3, 4, 5};
            MutableListSequence<int> seq(data, 5);
            auto subseq = seq.getSubSequence(1, 3);
            int expectedData[] = {2, 3, 4};
            MutableListSequence<int> expectedSeq(expectedData, 3);
            if (!sequencesEqual(subseq.get(), &expectedSeq))
                throw std::runtime_error("Incorrect subsequence.");
        });

        // Тест вставки элемента
        runner.expectNoException("testMutableListSequence::Inserting element", []() {
            int data[] = {1, 3};
            MutableListSequence<int> seq(data, 2);
            seq.insertAt(1, 2);
            if (seq.get(1) != 2) throw std::runtime_error("Incorrect element at index 1 after insertion.");
        });

        // Тест конкатенации
        runner.expectNoException("testMutableListSequence::Concatenating sequences", []() {
            int data1[] = {1, 2};
            auto seq1 = SharedPtr<MutableSequence<int>>(new MutableListSequence<int>(data1, 2));
            int data2[] = {3, 4};
            auto seq2 = SharedPtr<MutableSequence<int>>(new MutableListSequence<int>(data2, 2));
            seq1->concat(seq2);
            int expectedData[] = {1, 2, 3, 4};
            MutableListSequence<int> expectedSeq(expectedData, 4);
            if (!sequencesEqual(seq1.get(), &expectedSeq))
                throw std::runtime_error("Incorrect concatenated sequence.");
        });

        // Тесты на пустые последовательности
        runner.expectException<std::out_of_range>("testMutableListSequence::GetFirst from empty sequence", []() {
            MutableListSequence<int> seq;
            seq.getFirst();
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::GetLast from empty sequence", []() {
            MutableListSequence<int> seq;
            seq.getLast();
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::Get from empty sequence", []() {
            MutableListSequence<int> seq;
            seq.get(0);
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::GetSubSequence from empty sequence", []() {
            MutableListSequence<int> seq;
            seq.getSubSequence(0, 0);
        });

        // Тесты на отрицательные индексы
        runner.expectException<std::out_of_range>("testMutableListSequence::Get with negative index", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
            seq.get(-1);
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::GetSubSequence with negative start index", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
            seq.getSubSequence(-1, 1);
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::GetSubSequence with negative end index", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
            seq.getSubSequence(0, -1);
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::GetSubSequence with start index out of range", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
            seq.getSubSequence(3, 2);
        });
        runner.expectException<std::out_of_range>("testMutableListSequence::GetSubSequence with end index out of range", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);
            seq.getSubSequence(0, 4);
        });

        // Тест копирования мутабельной последовательности
        runner.expectNoException("testMutableListSequence::Copy mutable sequence", []() {
            int data[] = {1, 2};
            MutableListSequence<int> seq1(data, 2);
            MutableListSequence<int> seq2(seq1);

            // Проверяем, что seq2 - копия seq1
            if (!sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Sequences are not equal after copy.");

            // Изменяем seq2 и проверяем, что seq1 не изменилась
            seq2.append(3);
            if (sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Original sequence changed after modifying the copy.");
        });

        // Тест присваивания мутабельной последовательности
        runner.expectNoException("testMutableListSequence::Assign mutable sequence", []() {
            int data[] = {1, 2};
            MutableListSequence<int> seq1(data, 2);

            MutableListSequence<int> seq2;
            seq2 = seq1;

            // Проверяем, что seq2 - копия seq1
            if (!sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Sequences are not equal after assignment.");

            // Изменяем seq2 и проверяем, что seq1 не изменилась
            seq2.append(3);
            if (sequencesEqual(&seq1, &seq2)) throw std::runtime_error("Original sequence changed after modifying the assigned sequence.");
        });

        // Тест методов tryGet, tryGetFirst, tryGetLast
        runner.expectNoException("testMutableListSequence::TryGet methods", []() {
            int data[] = {1, 2, 3};
            MutableListSequence<int> seq(data, 3);

            // tryGet
            auto opt1 = seq.tryGet(1);
            if (opt1.isNone()) throw std::runtime_error("tryGet(1) returned None for existing element.");
            if (opt1.getValue() != 2) throw std::runtime_error("tryGet(1) returned incorrect value.");

            auto opt2 = seq.tryGet(3);
            if (opt2.isSome()) throw std::runtime_error("tryGet(3) returned Some for non-existing element.");

            // tryGetFirst
            auto opt3 = seq.tryGetFirst();
            if (opt3.isNone()) throw std::runtime_error("tryGetFirst returned None for non-empty sequence.");
            if (opt3.getValue() != 1) throw std::runtime_error("tryGetFirst returned incorrect value.");

            // tryGetLast
            auto opt4 = seq.tryGetLast();
            if (opt4.isNone()) throw std::runtime_error("tryGetLast returned None for non-empty sequence.");
            if (opt4.getValue() != 3) throw std::runtime_error("tryGetLast returned incorrect value.");
        });
    }

    void testSharedPtr() {
        TestRunner runner;

        // 1.  Тест создания и уничтожения SharedPtr
        runner.expectNoException("SharedPtr::Basic creation and destruction", []() {
            SharedPtr<int> ptr(new int(10));
        });

        // 2.  Тест подсчета ссылок (use_count)
        runner.expectNoException("SharedPtr::Reference counting", []() {
            SharedPtr<int> ptr1(new int(10));
            {
                SharedPtr<int> ptr2 = ptr1;
                if (ptr1.use_count() != 2 || ptr2.use_count() != 2) {
                    throw std::runtime_error("Incorrect reference count.");
                }
            } // ptr2 выходит за область видимости
            if (ptr1.use_count() != 1) {
                throw std::runtime_error("Incorrect reference count after destruction.");
            }
        });

        // 3.  Тест оператора разыменования (*)
        runner.expectNoException("SharedPtr::Dereference operator", []() {
            SharedPtr<int> ptr(new int(10));
            if (*ptr != 10) {
                throw std::runtime_error("Incorrect dereference value.");
            }
        });

        // 4.  Тест оператора стрелка (->)
        runner.expectNoException("SharedPtr::Arrow operator", []() {
            SharedPtr<std::string> ptr(new std::string("hello"));
            if (ptr->size() != 5) {
                throw std::runtime_error("Incorrect arrow operator access.");
            }
        });

        // 5.  Тест get()
        runner.expectNoException("SharedPtr::get() method", []() {
            SharedPtr<int> ptr(new int(10));
            int* rawPtr = ptr.get();
            if (*rawPtr != 10) {
                throw std::runtime_error("Incorrect raw pointer access.");
            }
        });

        // 6.  Тест reset()
        runner.expectNoException("SharedPtr::reset() method", []() {
            SharedPtr<int> ptr(new int(10));
            ptr.reset();
            if (ptr.get() != nullptr || ptr.use_count() != 0) {
                throw std::runtime_error("Incorrect reset behavior.");
            }
        });

        // 7.  Тест reset() с новым указателем
        runner.expectNoException("SharedPtr::reset() with new pointer", []() {
            SharedPtr<int> ptr(new int(10));
            ptr.reset(new int(20));
            if (*ptr != 20 || ptr.use_count() != 1) {
                throw std::runtime_error("Incorrect reset with new pointer behavior.");
            }
        });

        // 8.  Тест конструктора копирования
        runner.expectNoException("SharedPtr::Copy constructor", []() {
            SharedPtr<int> ptr1(new int(10));
            SharedPtr<int> ptr2(ptr1);
            if (*ptr1 != 10 || *ptr2 != 10 || ptr1.use_count() != 2 || ptr2.use_count() != 2) {
                throw std::runtime_error("Incorrect copy constructor behavior.");
            }
        });

        // 9.  Тест оператора присваивания
        runner.expectNoException("SharedPtr::Assignment operator", []() {
            SharedPtr ptr1(new int(10));
            SharedPtr<int> ptr2 = ptr1;
            if (*ptr1 != 10 || *ptr2 != 10 || ptr1.use_count() != 2 || ptr2.use_count() != 2) {
                throw std::runtime_error("Incorrect assignment operator behavior.");
            }
        });

        // 10. Тест MakeShared (для одиночных объектов)
        runner.expectNoException("SharedPtr::MakeShared (single object)", []() {
            auto ptr = MakeShared<int>(30);
            if (*ptr != 30 || ptr.use_count() != 1) {
                throw std::runtime_error("Incorrect MakeShared behavior (single object).");
            }
        });
    }
    void testUniquePtr() {
        TestRunner runner;

        // 1. Базовый тест создания и уничтожения
        runner.expectNoException("UniquePtr::Basic creation and destruction", []() {
            UniquePtr<int> ptr(new int(5));
        });

        // 2. Тест оператора разыменования
        runner.expectNoException("UniquePtr::Dereference operator", []() {
            UniquePtr<int> ptr(new int(5));
            if (*ptr != 5) {
                throw std::runtime_error("Incorrect dereference value.");
            }
        });

        // 3. Тест оператора стрелка
        runner.expectNoException("UniquePtr::Arrow operator", []() {
            UniquePtr<std::string> ptr(new std::string("hello"));
            if (ptr->length() != 5) {
                throw std::runtime_error("Incorrect arrow operator usage.");
            }
        });

        // 4. Тест get()
        runner.expectNoException("UniquePtr::get() method", []() {
           UniquePtr<int> ptr(new int(5));
           if (*(ptr.get()) != 5) {
               throw std::runtime_error("Incorrect get() usage.");
           }
        });

        // 5. Тест release()
        runner.expectNoException("UniquePtr::release() method", []() {
            UniquePtr<int> ptr(new int(5));
            int* rawPtr = ptr.release();
            if (ptr.get() != nullptr) {
                throw std::runtime_error("Pointer not released.");
            }
            if (*rawPtr != 5) {
                throw std::runtime_error("Incorrect released pointer value.");
            }
            delete rawPtr; // Необходимо удалить вручную после release()
        });

        // 6. Тест reset()
        runner.expectNoException("UniquePtr::reset() method", []() {
           UniquePtr<int> ptr(new int(5));
           ptr.reset(new int(10));
           if (*ptr != 10) {
               throw std::runtime_error("Incorrect reset() usage.");
           }
        });

        // 7. Тест reset() с nullptr
        runner.expectNoException("UniquePtr::reset() with nullptr", []() {
            UniquePtr<int> ptr(new int(5));
            ptr.reset();
            if (ptr.get() != nullptr) {
                throw std::runtime_error("Pointer not reset to nullptr.");
            }
        });


        // 8. Тест конструктора перемещения
        runner.expectNoException("UniquePtr::Move constructor", []() {
            UniquePtr<int> ptr1(new int(5));
            UniquePtr<int> ptr2(std::move(ptr1));
            if (ptr1.get() != nullptr) {
                throw std::runtime_error("Original pointer not nulled after move.");
            }
            if (*ptr2 != 5) {
                throw std::runtime_error("Incorrect moved value.");
            }
        });

        // 9. Тест оператора перемещения
        runner.expectNoException("UniquePtr::Move assignment operator", []() {
            UniquePtr<int> ptr1(new int(5));
            UniquePtr<int> ptr2;
            ptr2 = std::move(ptr1);
            if (ptr1.get() != nullptr) {
                throw std::runtime_error("Original pointer not nulled after move assignment.");
            }
            if (*ptr2 != 5) {
                throw std::runtime_error("Incorrect moved value.");
            }
        });

        // 10. Тест make_unique
        runner.expectNoException("UniquePtr::make_unique test", []() {
            auto ptr = make_unique<int>(42);
            if (*ptr != 42 || ptr.get() == nullptr) {
                throw std::runtime_error("Incorrect make_unique behavior.");
            }
        });
    }

    void testWeakPtr() {
        TestRunner runner;

        // 1. Базовый тест создания из SharedPtr
        runner.expectNoException("WeakPtr::Basic creation from SharedPtr", []() {
            SharedPtr<int> sharedPtr(new int(10));
            WeakPtr<int> weakPtr(sharedPtr);
        });

        // 2. Тест expired()
        runner.expectNoException("WeakPtr::expired() method", []() {
            SharedPtr<int> sharedPtr(new int(10));
            WeakPtr<int> weakPtr(sharedPtr);
            if (weakPtr.expired()) {
                throw std::runtime_error("WeakPtr should not be expired.");
            }
            sharedPtr.reset();
            if (!weakPtr.expired()) {
                throw std::runtime_error("WeakPtr should be expired.");
            }
        });

        // 3. Тест use_count()
        runner.expectNoException("WeakPtr::use_count() method", []() {
            SharedPtr<int> sharedPtr(new int(10));
            WeakPtr<int> weakPtr1(sharedPtr);
            if (weakPtr1.use_count() != 1) {
                throw std::runtime_error("Incorrect use_count().");
            }
            {
                SharedPtr<int> sharedPtr2 = sharedPtr;
                WeakPtr<int> weakPtr2(sharedPtr);
                if (weakPtr1.use_count() != 2) {
                    throw std::runtime_error("Incorrect use_count().");
                }
            }
            if (weakPtr1.use_count() != 1) {
                throw std::runtime_error("Incorrect use_count() after SharedPtr destruction.");
            }
        });
    }
}
