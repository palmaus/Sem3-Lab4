#include "GUI.h"
#include <limits>
#include <iostream>
#include "DirectedGraphGenerator.h"
#include "UndirectedGraphGenerator.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>


IVertex<int, int>* GraphVisualizer::findVertexAt(const QPointF& pos) {
    for (QGraphicsItem* item : scene_->items(pos)) {
        if (QGraphicsEllipseItem* circle = dynamic_cast<QGraphicsEllipseItem*>(item)) {
            for (int vertexId : vertexPositions_.keys()) {
                QPointF vertexPos = vertexPositions_.value(vertexId);
                if (QRectF(vertexPos.x() - 15, vertexPos.y() - 15, 30, 30).contains(pos)) {
                    return currentGraph_->getVertexById(vertexId);
                }
            }
        }
    }
    return nullptr;
}

bool GraphVisualizer::eventFilter(QObject* obj, QEvent* event) {
    if (obj == view_->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QPointF pos = view_->mapToScene(mouseEvent->pos());
                startVertex_ = findVertexAt(pos);
                if (startVertex_) {
                    isDrawingEdge_ = true;
                    currentEdge_ = scene_->addLine(QLineF(vertexPositions_[startVertex_->getId()], pos), QPen(Qt::black));
                    return true;
                } else {
                    showContextMenu(pos);
                }
            }
        } else if (event->type() == QEvent::MouseMove) {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            if (isDrawingEdge_) {
                QPointF pos = view_->mapToScene(mouseEvent->pos());
                currentEdge_->setLine(QLineF(vertexPositions_[startVertex_->getId()], pos));
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && isDrawingEdge_) {
                QPointF pos = view_->mapToScene(mouseEvent->pos());
                IVertex<int, int>* endVertex = findVertexAt(pos);
                if (endVertex && endVertex != startVertex_) {
                    bool ok;
                    int weight = QInputDialog::getInt(this, "Добавить ребро", "Введите вес ребра:", 1, 1, std::numeric_limits<int>::max(), 1, &ok);
                    if (ok) {
                        currentGraph_->addEdge(startVertex_, endVertex, weight);
                        shortestPath_.reset();
                        updateGraphVisualization();
                    }
                }
                currentEdge_ = nullptr;
                startVertex_ = nullptr;
                isDrawingEdge_ = false;
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void GraphVisualizer::onGraphTypeChanged(int index) {
    currentGraph_ = (index == 0) ? static_cast<IGraph<int, int>*>(directedGraph_.get()) : static_cast<IGraph<int, int>*>(undirectedGraph_.get());
    updateGraphVisualization();
}

void GraphVisualizer::showContextMenu(const QPointF& pos) {
    QMenu contextMenu;
    QAction* addVertexAction = contextMenu.addAction("Добавить вершину");
    QAction* selectedAction = contextMenu.exec(view_->mapToGlobal(view_->mapFromScene(pos)));

    if (selectedAction == addVertexAction) {
        addVertex(pos);
    }
}

void GraphVisualizer::addVertex(const QPointF& pos) {
    bool ok;
    int vertexID = QInputDialog::getInt(this, "Добавить вершину", "Введите ID вершины:", 0, 0, 1000, 1, &ok);
    if (ok) {
        IVertex<int, int>* vertex = new Vertex<int, int>(vertexID);
        currentGraph_->addVertex(vertex);
        vertexPositions_[vertexID] = pos;
        shortestPath_.reset();
        updateGraphVisualization();
    }
}

void GraphVisualizer::clearGraph() {
    scene_->clear();
    directedGraph_ = MakeShared<DirectedGraph<int, int>>();
    undirectedGraph_ = MakeShared<UndirectedGraph<int, int>>();
    currentGraph_ = directedGraph_.get();
    vertexPositions_.clear();
    shortestPath_ = std::nullopt;
}

void GraphVisualizer::useGraphMethod()
{
    QStringList methods;
    methods << "Найти кратчайший путь" << "Найти компоненты связности" << "Найти минимальное остовное дерево" << "Найти топологическую сортировку" << "Найти компоненты сильной связности";
        bool ok;
    QString selectedMethod = QInputDialog::getItem(this, "Выбор метода", "Выберите метод:", methods, 0, false, &ok);
     if(ok) {
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


void GraphVisualizer::generateRandomGraph() {
    bool ok;
    int numVertices = QInputDialog::getInt(this, "Генерация графа", "Количество вершин:", 10, 1, 20, 1, &ok);
    if (!ok) return;

    double edgeProbability = QInputDialog::getDouble(this, "Генерация графа", "Вероятность ребра (0.0 - 1.0):", 0.3, 0.0, 1.0, 2, &ok);
    if (!ok) return;

    clearGraph();

    bool isDirected = (graphTypeComboBox_->currentIndex() == 0);

    try {
        if (isDirected) {
            DirectedGraphGenerator<int, int> generator(numVertices, edgeProbability);
            directedGraph_ = SharedPtr<DirectedGraph<int, int>>(dynamic_cast<DirectedGraph<int, int> *>(generator.generate()));
            currentGraph_ = directedGraph_.get();
        } else {
            UndirectedGraphGenerator<int, int> generator(numVertices, edgeProbability);
            undirectedGraph_ = SharedPtr<UndirectedGraph<int, int>>(dynamic_cast<UndirectedGraph<int, int> *>(generator.generate()));
            currentGraph_ = undirectedGraph_.get();
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", e.what());
        return;
    }

    vertexPositions_.clear();
    const double radius = numVertices * 20;
    const QPointF center(view_->width() / 2.0, view_->height() / 2.0);

    auto vertices = currentGraph_->getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        double angle = 2 * M_PI * i / vertices.getLength();
        int vertexId = vertices.get(i)->getId();
        QPointF pos(center.x() + radius * cos(angle), center.y() + radius * sin(angle));
        vertexPositions_[vertexId] = pos;
    }

     updateGraphVisualization();

     scene_->setSceneRect(scene_->itemsBoundingRect());
     qreal maxScale = 2.0;
     if (view_->transform().m11() > maxScale)
     {
        view_->scale(maxScale / view_->transform().m11(), maxScale / view_->transform().m22());
     } else {
          if (sender() == findChild<QPushButton*>("generateButton"))
          {
              view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
          }
     }
}


void GraphVisualizer::findShortestPath()
{
    bool ok1, ok2;
    int from = QInputDialog::getInt(this, "Кратчайший путь", "Начальная вершина:", 0, 0, 1000, 1, &ok1);
    int to = QInputDialog::getInt(this, "Кратчайший путь", "Конечная вершина:", 0, 0, 1000, 1, &ok2);

     if (ok1 && ok2) {
        IVertex<int, int>* fromVertex = currentGraph_->getVertexById(from);
        IVertex<int, int>* toVertex = currentGraph_->getVertexById(to);
        if (!fromVertex || !toVertex) {
            QMessageBox::warning(this, "Ошибка", "Одна из вершин не найдена.");
            return;
        }
        try {
            DijkstraAlgorithm<int, int> dijkstra;
            auto dijkstraResult = dijkstra.execute(currentGraph_, fromVertex, toVertex);
            auto distances = dijkstraResult->first;
            shortestPath_ = dijkstraResult->second;

            auto vertices = currentGraph_->getVertices();
            int toVertexIndex = -1;
            for (size_t i = 0; i < vertices.getLength(); ++i) {
                if (vertices.get(i)->getId() == toVertex->getId()) {
                    toVertexIndex = i;
                    break;
                }
            }

             if (toVertexIndex == -1) {
                QMessageBox::warning(this, "Ошибка", "Конечная вершина не найдена в результатах Дейкстры.");
                return;
            }
            if(distances.get(toVertexIndex) == std::numeric_limits<int>::max()){
                 QMessageBox::information(this, "Результат", "Путь не существует.");
            } else {
                QString result = "Кратчайший путь: " + QString::number(distances.get(toVertexIndex));
                 QMessageBox::information(this, "Результат", result);
            }

            updateGraphVisualization();


         } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", e.what());
         }
    }
}

void GraphVisualizer::findConnectedComponents()
{
     try {
        ConnectedComponentsAlgorithm<int, int> ccAlgo;
        IGraph<int, int>* undirectedGraph = dynamic_cast<UndirectedGraph<int, int>*>(currentGraph_);
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

void GraphVisualizer::findMST()
{
    try {
        MSTAlgorithm<int, int> mstAlgo;
        IGraph<int, int>* undirectedGraph = dynamic_cast<UndirectedGraph<int, int>*>(currentGraph_);
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

void GraphVisualizer::findTopologicalSort()
{
      try {
            TopologicalSortAlgorithm<int, int> topologicalSort;
            IGraph<int, int>* directedGraph = dynamic_cast<DirectedGraph<int, int>*>(currentGraph_);
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

void GraphVisualizer::findStronglyConnectedComponents()
{
    try {
        StronglyConnectedComponentsAlgorithm<int, int> stronglyConnectedComponentsAlgorithm;
        IGraph<int, int>* directedGraph = dynamic_cast<DirectedGraph<int, int>*>(currentGraph_);
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
    }  catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void GraphVisualizer::updateGraphVisualization() {
     scene_->clear();

    QColor pathColor = Qt::red;

    auto vertices = currentGraph_->getVertices();
    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex<int, int>* from = vertices.get(i);
        if (vertexPositions_.contains(from->getId())) {
            auto edges = currentGraph_->getEdges(from);
            for (size_t j = 0; j < edges.getLength(); ++j) {
                IVertex<int, int>* to = edges.get(j)->getTo();
                int weight = edges.get(j)->getWeight();
                if (vertexPositions_.contains(to->getId())) {
                    QPointF fromPos = vertexPositions_[from->getId()];
                    QPointF toPos = vertexPositions_[to->getId()];
                    QPen pen(Qt::black);
                    if (shortestPath_ && shortestPath_->containsEdge(currentGraph_, edges.get(j))) {
                        pen.setColor(pathColor);
                        pen.setWidth(2);
                    }
                    QGraphicsLineItem* line = scene_->addLine(QLineF(fromPos, toPos), pen);
                    QGraphicsTextItem* weightText = scene_->addText(QString::number(weight));
                    weightText->setPos((fromPos + toPos) / 2);
                }
            }
        }
    }

    for (size_t i = 0; i < vertices.getLength(); ++i) {
        IVertex<int, int>* vertex = vertices.get(i);
        if (vertexPositions_.contains(vertex->getId())) {
            QPointF pos = vertexPositions_[vertex->getId()];
            QBrush brush(Qt::lightGray);
            if (shortestPath_ && shortestPath_->containsVertex(vertex)) {
                brush.setColor(pathColor);
            }
            const qreal ellipseRadius = 15.0;
            QGraphicsEllipseItem* circle = scene_->addEllipse(pos.x() - ellipseRadius, pos.y() - ellipseRadius, 2 * ellipseRadius, 2 * ellipseRadius, QPen(Qt::black), brush);
            QGraphicsTextItem* text = scene_->addText(QString::number(vertex->getId()));
            text->setPos(pos.x() - 10, pos.y() - 10);
        }
    }
    scene_->setSceneRect(scene_->itemsBoundingRect());
    qreal maxScale = 2.0;
    if (view_->transform().m11() > maxScale) {
          view_->scale(maxScale / view_->transform().m11(), maxScale / view_->transform().m22());
      }
}