#include "frame.h"

/*
 * Constructors
 */
Frame::Frame()
{
    m_image = QPixmap();
    m_id = 0;
}

Frame::Frame(QPixmap image, int id)
{
    m_image = image;
    m_id = id;
}

/*
 * Getter / Setter
 */
void Frame::setImage(QPixmap image) { m_image = image; }
void Frame::setId(int id) { m_id = id; }
QPixmap Frame::image() { return m_image; }
int Frame::id() { return m_id; }
