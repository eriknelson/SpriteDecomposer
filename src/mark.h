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
#ifndef MARK_H
#define MARK_H

#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QPainter>

#include <QDebug>

class Mark : public QGraphicsObject
{
    Q_OBJECT
    Q_ENUMS(MarkOrientation);
public:
    enum MarkOrientation {MarkHorizontal=0,MarkVertical=1};

    // Constructor
    explicit Mark(MarkOrientation orientation,int position, int sceneWidth, int sceneHeight, QGraphicsItem *parent = 0);

    // Getter  / Setter
    void setOrientation(Mark::MarkOrientation orientation);
    Mark::MarkOrientation orientation() const;

    void setPosition(int position);
    int position() const;

    void setSceneSize(int width, int height);

    // Methods
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

private:
    // Methods
    QPoint beginPoint() const;
    QPoint endPoint() const;

    // Members
    MarkOrientation m_orientation;
    int m_position;
    int m_sceneWidth;
    int m_sceneHeight;

signals:
    void positionChange();

public slots:

};

#endif // MARK_H
