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
#include <QColor>
#include <QGraphicsItem>
#include <QHBoxLayout>

#include "DirectedGraph.h"
#include "UndirectedGraph.h"
#include "DijkstraAlgorithm.h"
#include "MSTAlgorithm.h"
#include "ConnectedComponentsAlgorithm.h"
#include "StronglyConnectedComponentsAlgorithm.h"
#include "TopologicalSortAlgorithm.h"
#include "IVertex.h"
#include "Vertex.h"
#include "Edge.h"
#include "SharedPtr.h"
#include "GraphPath.h"
#include <optional>
#include "DirectedGraphGenerator.h"
#include "UndirectedGraphGenerator.h"

class GraphVisualizer : public QMainWindow {

public:
    GraphVisualizer(QWidget* parent = nullptr) : QMainWindow(parent) {
        auto centralWidget = new QWidget(this);
        auto layout = new QVBoxLayout(centralWidget);

        scene_ = new QGraphicsScene(this);
        scene_->setBackgroundBrush(QBrush(Qt::white));
        view_ = new QGraphicsView(scene_, this);

        view_->scale(1, 1);
        view_->setRenderHint(QPainter::Antialiasing);
        view_->setRenderHint(QPainter::TextAntialiasing);
        view_->setRenderHint(QPainter::SmoothPixmapTransform);
        view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        view_->setResizeAnchor(QGraphicsView::AnchorViewCenter);
        view_->setDragMode(QGraphicsView::ScrollHandDrag);

        layout->addWidget(view_);

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


        auto generateButton = new QPushButton("Генерировать граф", this);
        connect(generateButton, &QPushButton::clicked, this, &GraphVisualizer::generateRandomGraph);
        controlsLayout->addWidget(generateButton);
		  generateButton->setObjectName("generateButton");

        layout->addLayout(controlsLayout);
        setCentralWidget(centralWidget);

        directedGraph_ = MakeShared<DirectedGraph<int, int>>();
        undirectedGraph_ = MakeShared<UndirectedGraph<int, int>>();
        currentGraph_ = directedGraph_.get();

        view_->setMouseTracking(true);
        view_->viewport()->installEventFilter(this);
    }

    virtual ~GraphVisualizer() {
        scene_->clear();
        delete scene_;
        directedGraph_ = nullptr;
        undirectedGraph_ = nullptr;
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onGraphTypeChanged(int index);
    void showContextMenu(const QPointF& pos);
    void addVertex(const QPointF& pos);
    void addEdge(const QPointF&);
    void clearGraph();
    void useGraphMethod();
    void findShortestPath();
    void findConnectedComponents();
    void findMST();
    void findTopologicalSort();
    void findStronglyConnectedComponents();
    void updateGraphVisualization();
    void generateRandomGraph();

private:
    QGraphicsView* view_;
    QGraphicsScene* scene_;
    QComboBox* graphTypeComboBox_;

    SharedPtr<DirectedGraph<int, int>> directedGraph_;
    SharedPtr<UndirectedGraph<int, int>> undirectedGraph_;
    IGraph<int, int>* currentGraph_;

    QMap<int, QPointF> vertexPositions_;
    std::optional<GraphPath<int, int>> shortestPath_;

    IVertex<int, int>* startVertex_ = nullptr;
    bool isDrawingEdge_ = false;
    QGraphicsLineItem* currentEdge_;

    IVertex<int, int>* findVertexAt(const QPointF& pos);
};

#endif // GUI_H