#include "animation.h"

Animation::Animation(QObject *parent) :
    QObject(parent)
{
    m_magnetMargin = 3;
    m_magnetism = true;

    m_scene = new QGraphicsScene(this);
    m_play = false;
    m_animationFrameItem = 0;
    m_animationFrameIndex = 0;
    m_frameCount = 0;
    m_background = QPixmap();
    m_cuts = QList<Cut*>();
    m_speed = 150;
}

/*
 * Getter / Setter
 */
void Animation::setTitle(const QString &title) { m_title = title; }
void Animation::setBackground(QPixmap background) { m_background = background; }
void Animation::setMagnetism(bool magnetism) { m_magnetism = magnetism; }
void Animation::setMarks(QList<Mark *> *marks) { m_marks = marks; }
void Animation::addFrameCount(int count)
{
    m_frameCount += count;
    emit frameCountUpdated(m_frameCount);
}
void Animation::addCut(Cut *cut){ m_cuts.append(cut); }
bool Animation::magnetism() const { return m_magnetism; }
QString Animation::title() const { return m_title; }
QGraphicsScene* Animation::scene() const { return m_scene; }
QList<Cut*>* Animation::cuts() { return &m_cuts; }
int Animation::frameCount() const { return m_frameCount; }
int Animation::speed() const { return m_speed; }

/*
 * Public Methods
 */
void Animation::magnetize(QPointF *p)
{
    // Magnetism disabled
    if(!m_magnetism) return;

    /* Magnet Marks */
    int mdx = m_magnetMargin+1, mdy = m_magnetMargin+1,
            mH = 0, mV = 0;
    Mark *mark = 0;
    for(int i=0;i<m_marks->size();i++)
    {
        mark = m_marks->at(i);

        mH = p->x()-mark->position();
        mV = p->y()-mark->position();

        if(intersectMargin(mH) && qAbs(mH) < mdx)
        {
            // Magnet from Left
            if(mH > 0)
                p->setX(mark->position());
            else
                p->setX(mark->position()-1);

            mdx = mH;
        }
        else if(intersectMargin(mV) && qAbs(mV) < mdy)
        {
            if(mV > 0)
                p->setY(mark->position());
            else
                p->setY(mark->position()-1);

            mdy = mV;
        }
    }

    /* Magnet other Cuts */

    // Closer element
    int dx = m_magnetMargin+1,dy = m_magnetMargin+1,
            // Distance between point and element
            mRight = 0, mLeft = 0, mTop = 0, mBottom = 0;

    // Temp Cut
    Cut *cut = 0;

    for(int i=0;i<m_cuts.size();i++)
    {
        cut = m_cuts.at(i);
        if(!cut->isReady()) continue;

        mLeft = cut->x() - p->x();
        mRight = cut->x() + cut->width() - p->x();
        mTop = cut->y() - p->y();
        mBottom = cut->y() + cut->height() - p->y();

        if(intersectMargin(mLeft) && qAbs(mLeft) < dx && p->y() <= cut->y()+cut->height() && p->y() >= cut->y())
        {
            p->setX(cut->x());
            dx = qAbs(mLeft);
        }
        else if(intersectMargin(mRight) && qAbs(mRight) < dx && p->y() <= cut->y()+cut->height() && p->y() >= cut->y())
        {
            p->setX(cut->x()+cut->width());
            dx = qAbs(mRight);
        }
        if(intersectMargin(mTop) && qAbs(mTop) < dy && p->x() <= cut->x()+cut->width() && p->x() >= cut->x())
        {
            p->setY(cut->y());
            dy = qAbs(mTop);
        }
        else if(intersectMargin(mBottom) && qAbs(mBottom) < dy && p->x() <= cut->x()+cut->width() && p->x() >= cut->x())
        {
            p->setY(cut->y()+cut->height());
            dy = qAbs(mBottom);
        }
    }
}

void Animation::magnetize(QRect *r)
{
    // Magnetism disabled
    if(!m_magnetism) return;

    /* Magnet Marks */
    int mdx = m_magnetMargin+1, mdy = m_magnetMargin+1,
            mH = 0, mV = 0, mH2 = 0, mV2 = 0;
    Mark *mark = 0;
    for(int i=0;i<m_marks->size();i++)
    {
        mark = m_marks->at(i);

        mH = r->x()-mark->position();
        mH2 = r->x()+r->width()-mark->position();
        mV = r->y()-mark->position();
        mV2 = r->y()+r->height()-mark->position();

        if(intersectMargin(mH) && qAbs(mH) < mdx)
        {
            r->setX(mark->position());
            mdx = mH;
        }
        else if(intersectMargin(mV) && qAbs(mV) < mdy)
        {
            r->setY(mark->position());
            mdy = mV;
        }
        else if(intersectMargin(mH2) && qAbs(mH2) < mdx)
        {
            r->setX(mark->position()-r->width());
            mdx = mH2;
        }
        else if(intersectMargin(mV2) && qAbs(mV2) < mdy)
        {
            r->setY(mark->position()-r->height());
            mdy = mV2;
        }
    }

    /* Magnet other Cuts */

    // Closer element
    int dx = m_magnetMargin+1,dy = m_magnetMargin+1,
            // Distance between elements
            mRight = 0, mLeft = 0, mTop = 0, mBottom = 0;

    // Temp Cut
    Cut *cut = 0;

    for(int i=0;i<m_cuts.size();i++)
    {
        cut = m_cuts.at(i);
        if(!cut->isReady()) continue;

        mLeft = cut->x() - r->x() - r->width();
        mRight = cut->x() + cut->boundingRect().width() - r->x();
        mTop = cut->y() - r->y() - r->height();
        mBottom = cut->y() + cut->boundingRect().height() - r->y();

        if(intersectMargin(mLeft) && qAbs(mLeft) < dx  && r->y() <= cut->boundingRect().height()+cut->y() && r->y() >= cut->y()-r->height() )
        {
            r->setX(cut->x() - r->width());
            dx = qAbs(mLeft);
        }
        else if(intersectMargin(mRight) && qAbs(mRight) < dx && r->y() <= cut->boundingRect().height()+cut->y() && r->y() >= cut->y()-r->height() )
        {
            r->setX(cut->x() + cut->boundingRect().width());
            dx = qAbs(mRight);
        }
        if(intersectMargin(mTop) && qAbs(mTop) < dy && r->x() <= cut->boundingRect().width()+cut->x() &&  r->x() >= cut->x()-r->width() )
        {
            r->setY(cut->y() - r->height());
            dy = qAbs(mTop);
        }
        else if(intersectMargin(mBottom) && qAbs(mBottom) < dy && r->x() <= cut->boundingRect().width()+cut->x() &&  r->x() >= cut->x()-r->width() )
        {
            r->setY(cut->y() + cut->boundingRect().height());
            dy = qAbs(mBottom);
        }
    }
}

void Animation::deleteCut(Cut *cut)
{
    for(int i=0;i<m_cuts.size();i++)
    {
        if(m_cuts.at(i) == cut)
        {
            addFrameCount(-cut->frameCount());
            m_cuts.removeAt(i);
            delete cut;
            break;
        }
    }
}

/*
 * Private Methods
 */

bool Animation::intersectMargin(int pos)
{
    return (pos >= -m_magnetMargin && pos <= m_magnetMargin);
}

void Animation::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_animationTimerId && m_frameCount)
    {
        showNextFrame();
    }
}

Frame Animation::getFrame(int frameId)
{
    if(m_background.isNull()) return Frame();

    int framecount = 0;
    Cut* cut = 0;

    // Search the frame
    for(int i=0;i<m_cuts.size();i++)
    {
        framecount += m_cuts.at(i)->frameCount();
        // We found the frame
        if(framecount >= frameId)
        {
            cut = m_cuts.at(i);
            break;
        }
    }

    // No cuts found
    if(!cut) return Frame();

    QRect cutframe = cut->cutFrame(frameId-framecount+cut->frameCount());
    return Frame(m_background.copy(cutframe),frameId);
}

void Animation::showFrame()
{
    // Remove previous Frame
    if(m_animationFrameItem) m_scene->removeItem(m_animationFrameItem);

    if(m_frameCount)
    {
        Frame frame = getFrame(m_animationFrameIndex);
        m_animationFrameItem = m_scene->addPixmap(frame.image());
        m_scene->setSceneRect(frame.image().rect());
    }
}

/*
 * Public Slots
 */
void Animation::playAnimation(bool play)
{
    m_play = play;
    if(play)
        m_animationTimerId = startTimer(m_speed);
    else if(m_animationTimerId)
        killTimer(m_animationTimerId);
}
void Animation::showNextFrame()
{
    if(++m_animationFrameIndex > m_frameCount) m_animationFrameIndex = 1;
    showFrame();
}
void Animation::showPreviousFrame()
{
    if(--m_animationFrameIndex <= 0) m_animationFrameIndex = m_frameCount;
    showFrame();
}
void Animation::showLastFrame()
{
    m_animationFrameIndex = m_frameCount;
    showFrame();
}
void Animation::showFirstFrame()
{
    m_animationFrameIndex = 1;
    showFrame();
}
void Animation::setSpeed(int speed)
{
    m_speed = speed;
    if(m_animationTimerId) killTimer(m_animationTimerId);
    playAnimation(m_play);
}
