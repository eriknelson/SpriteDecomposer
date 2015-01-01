#include "cut.h"

Cut::Cut(const QRectF &geometry, int row, int column)
{
    // Initialisation
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);

    m_ready = true;
    m_row = row;
    m_column = column;

    setGeometry(geometry);
}

/*
 * Getter / Setter
 */
void Cut::setWidth(qreal width) { setGeometry(geometry().adjusted(0,0,width-this->width(),0)); }
void Cut::setHeight(qreal height) { setGeometry(geometry().adjusted(0,0,0,height-this->height())); }
void Cut::setRow(int row) { m_row = row; }
void Cut::setColumn(int column) { m_column = column; }
void Cut::setCellWidth(int cellWidth) { setWidth(m_column*cellWidth); }
void Cut::setCellHeight(int cellHeight) { setHeight(m_row*cellHeight); }
void Cut::setReady(bool enabled) { m_ready = enabled; }

int Cut::frameCount() const { return m_row * m_column; }
bool Cut::isReady() const { return m_ready; }
int Cut::row() const { return m_row; }
int Cut::column() const { return m_column; }
qreal Cut::cellWidth() const { return width() / m_column; }
qreal Cut::cellHeight() const { return height() / m_row; }
qreal Cut::width() const { return geometry().width(); }
qreal Cut::height() const { return geometry().height(); }

/*
 * Public Method
 */
QRect Cut::cutFrame(int index) const
{
    int col = (index-1)%m_column;
    int row = (index-1)/m_column;

    return mapRectToScene(col*cellWidth(),row*cellHeight(),cellWidth(),cellHeight()).toRect();
}

void Cut::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QGraphicsItem *parentGroup = this->parentItem();
    qreal cellW = cellWidth(), cellH = cellHeight();
    QPen pen;

    pen.setStyle(Qt::SolidLine);
    if((parentGroup && parentGroup->isSelected()) || isSelected())
        pen.setColor(QColor(Qt::red));
    else
        pen.setColor(QColor(Qt::black));

    painter->setPen(pen);

    if(m_ready)
        painter->setBrush(Qt::NoBrush);
    else
        painter->setBrush(QBrush(QColor(200,200,200,128)));

    for(int col=0;col<m_column;col++)
    {
        for(int row=0;row<m_row;row++)
        {
            painter->drawRect(col*cellW,row*cellH,cellW,cellH);
        }
    }
}
