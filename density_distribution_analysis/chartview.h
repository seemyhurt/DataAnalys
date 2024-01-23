#ifndef CHARTVIEW_H
#define CHARTVIEW_H
#include <QChartView>

using namespace QtCharts;

class ChartView : public QChartView
{
    Q_OBJECT

public:
    ChartView(QWidget * parent = nullptr);
    ChartView(QChart * chart, QWidget * parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool isDragging = false;
    QPoint lastMousePos;
    double currentZoom = 0.5;
};

#endif // CHARTVIEW_H
