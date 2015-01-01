/*--License:
        Sprite Decomposer
        Copyright Nicolas Smeets (Creacore Software) 2010-2011
        http://www.creacore.be/blog/2011/06/sprite-decomposer-2/
        www.sourceforge.net/projects/spritedecompose

        Sprite Decomposer is provided under the Creative Commons License.

  -- Creative Commons License

http://creativecommons.org/licenses/by-nc-sa/3.0/

You are free:

    to Share — to copy, distribute and transmit the work
    to Remix — to adapt the work

Under the following conditions:

    Attribution — You must attribute the work in the manner specified by the author
    or licensor (but not in any way that suggests that they endorse you or your use of the work).

    Noncommercial — You may not use this work for commercial purposes.

    Share Alike — If you alter, transform, or build upon this work, you may distribute the
    resulting work only under the same or similar license to this one.
*/
#ifndef CUT_H
#define CUT_H

#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

class Cut : public QGraphicsWidget
{
    Q_OBJECT
public:
    // Constructor
    explicit Cut(const QRectF &geometry, int row = 1, int column = 1);

    // Setter
    void setWidth(qreal width);
    void setHeight(qreal height);
    void setRow(int row);
    void setColumn(int column);
    void setCellWidth(int cellWidth);
    void setCellHeight(int cellHeight);
    void setReady(bool enabled);

    // Getter
    int frameCount() const;
    bool isReady() const;
    int row() const;
    int column() const;
    qreal cellWidth() const;
    qreal cellHeight() const;
    qreal width() const;
    qreal height() const;

    // Specific methods
    QRect cutFrame(int index) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    bool m_ready;           // Cut is ready to be used with magnetism
    int m_row;
    int m_column;
};

#endif // CUT_H
