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
#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QTimerEvent>
#include <QList>
#include <QGraphicsScene>

#include "cut.h"
#include "mark.h"
#include "frame.h"

#include <QDebug>

class Animation : public QObject
{
    Q_OBJECT
public:
    // Constructor
    explicit Animation(QObject *parent = 0);

    // Setter
    void setTitle(const QString &title);
    void setBackground(QPixmap background);
    void setMagnetism(bool magnetism);
    void setMarks(QList<Mark*> *marks);     // Workarea Marks pointer
    void addFrameCount(int count);          // Add <count> Frames in Animation
    void addCut(Cut* cut);

    // Getter
    bool magnetism() const;
    QString title() const;
    QGraphicsScene* scene() const;
    QList<Cut*>* cuts();
    int frameCount() const;
    int speed() const;

    // Public Methods
    void magnetize(QPointF *p);              // Determine where to magnetize the point
    void magnetize(QRect *r);               // Determine where to magnetize the Cut

    void deleteCut(Cut *cut);

private:
    bool intersectMargin(int pos);          // Number is in margin
    void timerEvent(QTimerEvent* event);
    void showFrame();
    Frame getFrame(int frameId);

private:
    QString m_title;                        // Animation Title
    QPixmap m_background;                   // Original SpriteSheet
    QGraphicsScene *m_scene;                // Scene
// Magnetism
    int m_magnetMargin;                     // Magnetism margin
    bool m_magnetism;                       // Magnetism active
// Animation Preview
    bool m_play;                            // Animation is playing
    int m_animationFrameIndex;              // Current Animation Frame Index
    QGraphicsItem *m_animationFrameItem;    // Current Animation Frame Item
    int m_animationTimerId;                 // Animation Timer
    int m_frameCount;                       // Total Frames in Animation
    int m_speed;                            // Animation play speed
// Resources
    QList<Cut*> m_cuts;                     // Cut List
    QList<Mark*> *m_marks;                  // WorkArea Marks' List

signals:
    void frameCountUpdated(int frameCount);

public slots:
    void playAnimation(bool play);
    void showNextFrame();
    void showPreviousFrame();
    void showLastFrame();
    void showFirstFrame();
    void setSpeed(int speed);

};

#endif // ANIMATION_H
