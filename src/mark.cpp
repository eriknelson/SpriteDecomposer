#include "mark.h"

Mark::Mark(MarkOrientation orientation, int position, int sceneWidth, int sceneHeight, QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    m_orientation = orientation;
    m_position = position;
    m_sceneWidth = sceneWidth;
    m_sceneHeight = sceneHeight;
}

/*
 * Getter / Setter
 */
void Mark::setOrientation(Mark::MarkOrientation orientation)
{
    m_orientation = orientation;
}
Mark::MarkOrientation Mark::orientation() const
{
    return m_orientation;
}

void Mark::setPosition(int position)
{
    prepareGeometryChange();
    m_position = position;
    emit positionChange();
}
int Mark::position() const
{
    return m_position;
}
void Mark::setSceneSize(int width, int height)
{
    m_sceneWidth = width;
    m_sceneHeight = height;
}

/*
 * Public Methods
 */
QRectF Mark::boundingRect() const
{
    return QRectF(beginPoint(),endPoint()).adjusted(-1,-1,1,1);
}

void Mark::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(180,0,0));

    painter->setPen(pen);
    painter->drawLine(beginPoint(),endPoint());
}

/*
 * Private Methods
 */
QPoint Mark::beginPoint() const
{
    if(m_orientation == MarkHorizontal)
        return QPoint(0,m_position);
    else
        return QPoint(m_position,0);
}
QPoint Mark::endPoint() const
{
    if(m_orientation == MarkHorizontal)
        return QPoint(m_sceneWidth,m_position);
    else
        return QPoint(m_position,m_sceneHeight);
}
