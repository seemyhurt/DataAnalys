#include "chartview.h"

using namespace QtCharts;

ChartView::ChartView(QWidget *parent) : QChartView(parent)
{

}

ChartView::ChartView(QChart *chart, QWidget *parent) : QChartView(chart, parent)
{
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() != 0)
    {
        if (event->angleDelta().y() > 0 && currentZoom < 1.0)
        {
            currentZoom += 0.1;
            chart()->zoomIn();
        }

        else if (event->angleDelta().y() < 0 && currentZoom > 0.1)
        {
            currentZoom -= 0.1;
            chart()->zoomOut();
        }

        event->accept();
    }
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = true;
        lastMousePos = event->pos();
    }
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging)
    {
        QPoint delta = event->pos() - lastMousePos;
        chart()->scroll(-delta.x(), delta.y());
        lastMousePos = event->pos();
    }
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = false;
    }
}
