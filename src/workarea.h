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
#ifndef WORKAREA_H
#define WORKAREA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QBitmap>
#include <QPixmap>
#include <QKeyEvent>
#include <QRubberBand>

#include "animation.h"
#include "cut.h"
#include "mark.h"

#include <QDebug>

class Interface;

class WorkArea : public QGraphicsScene
{
    Q_OBJECT
    Q_ENUMS(Tool)
    Q_ENUMS(CutType)

public:
    enum Tool {ToolNormal,ToolCutRect,ToolCutGrid,ToolMarkH,ToolMarkV,ToolPicker,ToolCutAuto};
    enum CutType {TypeCutRect,TypeCutGrid};

    // Constructor / Destructor
    explicit WorkArea(Interface *interface, QObject *parent = 0);

    // Setter
    void setBackground(const QString &filename);
    void setTool(WorkArea::Tool tool);
    void setAnimation(Animation* animation);
    // Getter
    QPixmap background() const;
    Animation* animation();

    // Mouse Event
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    // Key Event
    void keyReleaseEvent(QKeyEvent *event);

    // Public Methods
    void setMaskColor(QRgb color);
    Mark* addMark(int position, Mark::MarkOrientation orientation);

    Cut* addCut(const QRectF &geometry, WorkArea::CutType cutType = TypeCutRect);
    void deleteCut(Cut* cut);

    void removeAnimation();

private:
    void refreshAll();
    void autoCut(const QRect &area);

private:
    Interface *m_interface;
    QGraphicsView *m_view;              // Interface view for this workarea
    QPixmap m_background;               // Background image (sprite sheet)
    QPixmap m_background_masked;        // Background with mask
    QGraphicsItem *m_backgroundItem;    // Background Item
    Tool m_tool;                        // Actual tool mode
    Animation* m_animation;             // Current Animation
    Cut* m_currentDrawing;              // Current Cut Drawing
    Mark* m_currentMark;                // Current Mark Positionning
    QPointF m_originDrawing;            // Origin Drawing

    // Selection
    QList<QGraphicsItem*> m_selectedCuts;         // List of Selected Cuts
    QRubberBand *m_selectionRect;       // Draw a selection rectangle
    QPoint m_selectionOrigin;           // Origin of selection rectangle
    bool isSelecting;                   // Selection in progress
    QGraphicsItemGroup *m_selectionGroup;   // Item contains all selected cuts

    QList<Mark*> m_marks;               // Marks list
    // Cut Grid Params
    int m_cutGrid_row;                  // Interface Row Param
    int m_cutGrid_column;               // Interface Column Param
    // Cut Auto Params
    int m_cutAuto_tolerance;            // AutoCut tolerance
    bool m_constraintHeight,            // Force all auto cut to have same height
         m_constraintWidth;             // Force all auto cut to have same width

signals:
    // To Interface
    void pickerColor(QRgb color);
    void itemSelected(Cut* selection);
    void multipleSelection();

public slots:
    // From Interface
    void setItemWidth(int width);
    void setItemHeight(int height);
    void setItemX(int x);
    void setItemY(int y);
    void setItemRow(int row);
    void setItemColumn(int column);
    void setItemCellWidth(int cellWidth);
    void setItemCellHeight(int cellHeight);
    void toggledMagnet(bool checked);

    // Alignement
    void alignLeft();
    void alignRight();
    void alignCenterH();
    void alignTop();
    void alignBottom();
    void alignCenterV();

    void alignContentLeft();
    void alignContentRight();
    void alignContentCenterH();
    void alignContentTop();
    void alignContentBottom();
    void alignContentCenterV();

    int contentMaxX(Cut *cut);
    int contentMinX(Cut *cut);
    int contentMaxY(Cut *cut);
    int contentMinY(Cut *cut);

    // Auto Cut Params
    void changeTolerance(int tolerance);
    void changeConstraintHeight(bool enabled);
    void changeConstraintWidth(bool enabled);

    // Animations
    void setAnimationTitle(const QString &title);

    void deleteSelection();

};

#endif // WORKAREA_H
