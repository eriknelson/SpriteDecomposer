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
#ifndef FRAME_H
#define FRAME_H

#include <QPixmap>

class Frame
{
public:
    // Constructor
    Frame();
    Frame(QPixmap image,int id);

    // Setter
    void setImage(QPixmap image);
    void setId(int id);

    // Getter
    QPixmap image();
    int id();

private:
    QPixmap m_image;
    int m_id;
};

#endif // FRAME_H
