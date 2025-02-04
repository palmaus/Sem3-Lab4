#ifndef GUI_H
#define GUI_H

#include <QApplication>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QInputDialog>
#include <QMouseEvent>
#include <QMenu>
#include <QPointF>
#include <QMessageBox>

#include "DirectedGraph.h"
#include "UndirectedGraph.h"
#include "DijkstraAlgorithm.h"
#include "MSTAlgorithm.h"
#include "ConnectedComponentsAlgorithm.h"
#include "StronglyConnectedComponentsAlgorithm.h"
#include "TopologicalSortAlgorithm.h"
#include "IVertex.h"
#include "Edge.h"
#include "SharedPtr.h"

class GraphVisualizer : public QMainWindow {
    // Q_OBJECT

public:
    GraphVisualizer(QWidget* parent = nullptr) : QMainWindow(parent) {
        // Установка главного виджета
        auto centralWidget = new QWidget(this);
        auto layout = new QVBoxLayout(centralWidget);

        // Область рисования
        scene_ = new QGraphicsScene(this);
        scene_->setBackgroundBrush(QBrush(Qt::white));
        view_ = new QGraphicsView(scene_, this);
        layout->addWidget(view_);

        // Элементы управления
        auto controlsLayout = new QHBoxLayout();

        auto graphTypeLabel = new QLabel("Тип графа:", this);
        controlsLayout->addWidget(graphTypeLabel);

        graphTypeComboBox_ = new QComboBox(this);
        graphTypeComboBox_->addItem("Ориентированный");
        graphTypeComboBox_->addItem("Неориентированный");
        connect(graphTypeComboBox_, &QComboBox::currentIndexChanged, this, &GraphVisualizer::onGraphTypeChanged);
        controlsLayout->addWidget(graphTypeComboBox_);

        auto clearButton = new QPushButton("Очистить граф", this);
        connect(clearButton, &QPushButton::clicked, this, &GraphVisualizer::clearGraph);
        controlsLayout->addWidget(clearButton);

        auto methodButton = new QPushButton("Использовать метод графа", this);
        connect(methodButton, &QPushButton::clicked, this, &GraphVisualizer::useGraphMethod);
        controlsLayout->addWidget(methodButton);

        layout->addLayout(controlsLayout);
        setCentralWidget(centralWidget);

        // Создание графов
        directedGraph_ = MakeShared<DirectedGraph<int>>();
        undirectedGraph_ = MakeShared<UndirectedGraph<int>>();
        currentGraph_ = directedGraph_.get();

        // Настройка мыши
        view_->setMouseTracking(true);
        view_->viewport()->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (obj == view_->viewport()) {
            if (event->type() == QEvent::MouseButtonPress) {
                auto mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    QPointF pos = view_->mapToScene(mouseEvent->pos());
                    showContextMenu(pos);
                }
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }

private slots:
    void onGraphTypeChanged(int index) {
        currentGraph_ = (index == 0) ? static_cast<IGraph<int>*>(directedGraph_.get()) : static_cast<IGraph<int>*>(undirectedGraph_.get());
        updateGraphVisualization();
    }

    void showContextMenu(const QPointF& pos) {
        QMenu contextMenu;
        QAction* addVertexAction = contextMenu.addAction("Добавить вершину");
        QAction* addEdgeAction = contextMenu.addAction("Добавить ребро");
        QAction* selectedAction = contextMenu.exec(view_->mapToGlobal(view_->mapFromScene(pos)));

        if (selectedAction == addVertexAction) {
            addVertex(pos);
        } else if (selectedAction == addEdgeAction) {
            addEdge(pos);
        }
    }

    void addVertex(const QPointF& pos) {
        bool ok;
        int vertexID = QInputDialog::getInt(this, "Добавить вершину", "Введите ID вершины:", 0, 0, 1000, 1, &ok);
        if (ok) {
            IVertex* vertex = new Vertex(vertexID);

            currentGraph_->addVertex(vertex);
            vertexPositions_[vertexID] = pos;
            updateGraphVisualization();
            //delete vertex;
        }
    }

    void addEdge(const QPointF&) {
        bool ok1, ok2, ok3;
        int from = QInputDialog::getInt(this, "Добавить ребро", "Введите начальную вершину:", 0, 0, 1000, 1, &ok1);
        int to = QInputDialog::getInt(this, "Добавить ребро", "Введите конечную вершину:", 0, 0, 1000, 1, &ok2);
        int weight = QInputDialog::getInt(this, "Добавить ребро", "Введите вес ребра:", 1, 1, 1000, 1, &ok3);

        if (ok1 && ok2 && ok3) {
            IVertex* fromVertex = currentGraph_->getVertexById(from);
            IVertex* toVertex = currentGraph_->getVertexById(to);
            if (fromVertex && toVertex) {
                currentGraph_->addEdge(fromVertex, toVertex, weight);
                updateGraphVisualization();
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось добавить ребро, так как одной из вершин не существует.");
            }
        }
    }

    void clearGraph() {
        scene_->clear();
        directedGraph_ = MakeShared<DirectedGraph<int>>();
        undirectedGraph_ = MakeShared<UndirectedGraph<int>>();
        currentGraph_ = directedGraph_.get();
        vertexPositions_.clear();
    }

    void useGraphMethod() {
        QStringList methods;
        methods << "Найти кратчайший путь" << "Найти компоненты связности" << "Найти минимальное остовное дерево" << "Найти топологическую сортировку" << "Найти компоненты сильной связности";
        bool ok;
        QString selectedMethod = QInputDialog::getItem(this, "Выбор метода", "Выберите метод:", methods, 0, false, &ok);
        if (ok) {
            if (selectedMethod == "Найти кратчайший путь") {
                findShortestPath();
            } else if (selectedMethod == "Найти компоненты связности") {
                findConnectedComponents();
            } else if (selectedMethod == "Найти минимальное остовное дерево") {
                findMST();
            } else if (selectedMethod == "Найти топологическую сортировку") {
                findTopologicalSort();
            } else if (selectedMethod == "Найти компоненты сильной связности") {
                findStronglyConnectedComponents();
            }
        }
    }

    void findShortestPath() {
        bool ok1, ok2;
        int from = QInputDialog::getInt(this, "Кратчайший путь", "Начальная вершина:", 0, 0, 1000, 1, &ok1);
        int to = QInputDialog::getInt(this, "Кратчайший путь", "Конечная вершина:", 0, 0, 1000, 1, &ok2);

        if (ok1 && ok2) {
            IVertex* fromVertex = currentGraph_->getVertexById(from);
            IVertex* toVertex = currentGraph_->getVertexById(to);
            if (!fromVertex || !toVertex) {
                QMessageBox::warning(this, "Ошибка", "Одна из вершин не найдена.");
                return;
            }
            try {
                DijkstraAlgorithm<int> dijkstra;
                auto distances = dijkstra.execute(currentGraph_, fromVertex, toVertex);

                // Find the index of toVertex in the vertices sequence
                auto vertices = currentGraph_->getVertices();
                int toVertexIndex = -1;
                for (int i = 0; i < vertices.getLength(); ++i) {
                    if (vertices.get(i)->getId() == toVertex->getId()) {
                        toVertexIndex = i;
                        break;
                    }
                }

                if (toVertexIndex == -1) {
                    QMessageBox::warning(this, "Ошибка", "Конечная вершина не найдена в результатах Дейкстры.");
                    return;
                }

                // Use the index to get the correct distance
                QString result = "Кратчайший путь: " + QString::number(distances->get(toVertexIndex));
                QMessageBox::information(this, "Результат", result);

            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Ошибка", e.what());
            }
        }
    }

    void findConnectedComponents() {
        try {
            ConnectedComponentsAlgorithm<int> ccAlgo;
            IGraph<int>* undirectedGraph = dynamic_cast<UndirectedGraph<int>*>(currentGraph_);
            if (!undirectedGraph) {
                QMessageBox::warning(this, "Ошибка", "Компоненты связности можно искать только в неориентированном графе.");
                return;
            }
            auto components = ccAlgo.execute(undirectedGraph);
            QString result = "Компоненты связности:\n";
            for (size_t i = 0; i < components->getLength(); ++i) {
                result += "{ ";
                for (size_t j = 0; j < components->get(i).getLength(); ++j) {
                    result += QString::number(components->get(i).get(j)->getId()) + " ";
                }
                result += "}\n";
            }
            QMessageBox::information(this, "Результат", result);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", e.what());
        }
    }

    void findMST() {
        try {
            MSTAlgorithm<int> mstAlgo;
            IGraph<int>* undirectedGraph = dynamic_cast<UndirectedGraph<int>*>(currentGraph_);
            if (!undirectedGraph) {
                QMessageBox::warning(this, "Ошибка", "Минимальное остовное дерево можно искать только в неориентированном графе.");
                return;
            }
            auto mst = mstAlgo.execute(undirectedGraph);
            QString result = "Минимальное остовное дерево:\n";
            for (size_t i = 0; i < mst->getLength(); ++i) {
                result += "(" + QString::number(mst->get(i)->getFrom()->getId()) + ", " + QString::number(mst->get(i)->getTo()->getId()) + ")\n";
            }
            QMessageBox::information(this, "Результат", result);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", e.what());
        }
    }

    void findTopologicalSort() {
        try {
            TopologicalSortAlgorithm<int> topologicalSort;
            IGraph<int>* directedGraph = dynamic_cast<DirectedGraph<int>*>(currentGraph_);
            if (!directedGraph) {
                QMessageBox::warning(this, "Ошибка", "Топологическую сортировку можно искать только в ориентированном графе.");
                return;
            }
            auto result = topologicalSort.execute(directedGraph);
            QString output = "Топологическая сортировка:\n";
            for (size_t i = 0; i < result->getLength(); ++i) {
                output += QString::number(result->get(i)->getId()) + " ";
            }
            QMessageBox::information(this, "Результат", output);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", e.what());
        }
    }

    void findStronglyConnectedComponents() {
        try {
            StronglyConnectedComponentsAlgorithm<int> stronglyConnectedComponentsAlgorithm;
            IGraph<int>* directedGraph = dynamic_cast<DirectedGraph<int>*>(currentGraph_);
            if (!directedGraph) {
                QMessageBox::warning(this, "Ошибка", "Компоненты сильной связности можно искать только в ориентированном графе.");
                return;
            }
            auto components = stronglyConnectedComponentsAlgorithm.execute(directedGraph);
            QString result = "Компоненты сильной связности:\n";
            for (size_t i = 0; i < components->getLength(); ++i) {
                result += "{ ";
                for (size_t j = 0; j < components->get(i).getLength(); ++j) {
                    result += QString::number(components->get(i).get(j)->getId()) + " ";
                }
                result += "}\n";
            }
            QMessageBox::information(this, "Результат", result);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", e.what());
        }
    }

    void updateGraphVisualization() {
        scene_->clear();

        // Отображение вершин
        auto vertices = currentGraph_->getVertices();
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex* vertex = vertices.get(i);
            if (vertexPositions_.contains(vertex->getId())) {
                QPointF pos = vertexPositions_[vertex->getId()];
                QGraphicsEllipseItem* circle = scene_->addEllipse(pos.x() - 15, pos.y() - 15, 30, 30, QPen(Qt::black), QBrush(Qt::lightGray));
                QGraphicsTextItem* text = scene_->addText(QString::number(vertex->getId()));
                text->setPos(pos.x() - 10, pos.y() - 10);
            }
        }

        // Отображение ребер
        for (size_t i = 0; i < vertices.getLength(); ++i) {
            IVertex* from = vertices.get(i);
            if (vertexPositions_.contains(from->getId())) {
                auto edges = currentGraph_->getEdges(from);
                for (size_t j = 0; j < edges.getLength(); ++j) {
                    IVertex* to = edges.get(j)->getTo();
                    int weight = edges.get(j)->getWeight();
                    if (vertexPositions_.contains(to->getId())) {
                        QPointF fromPos = vertexPositions_[from->getId()];
                        QPointF toPos = vertexPositions_[to->getId()];
                        QGraphicsLineItem* line = scene_->addLine(QLineF(fromPos, toPos), QPen(Qt::black));
                        QGraphicsTextItem* weightText = scene_->addText(QString::number(weight));
                        weightText->setPos((fromPos.x() + toPos.x()) / 2, (fromPos.y() + toPos.y()) / 2);
                    }
                }
            }
        }
    }

private:
    QGraphicsView* view_;
    QGraphicsScene* scene_;
    QComboBox* graphTypeComboBox_;

    SharedPtr<DirectedGraph<int>> directedGraph_;
    SharedPtr<UndirectedGraph<int>> undirectedGraph_;
    IGraph<int>* currentGraph_;

    QMap<int, QPointF> vertexPositions_;
};

#endif //GUI_H