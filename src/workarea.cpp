#include "workarea.h"
#include "interface.h"

WorkArea::WorkArea(Interface *interface, QObject *parent) :
    QGraphicsScene(parent)
{
    // Initialisation
    m_interface = interface;
    m_tool = ToolNormal;
    m_background = QPixmap();
    m_background_masked = QPixmap();
    m_animation = 0;
    m_currentDrawing = 0;
    isSelecting = false;
    m_currentMark = 0;
    m_marks = QList<Mark*>();
    m_cutGrid_row = 3;
    m_cutGrid_column = 3;
    m_selectionRect = 0;
    m_view = qobject_cast<QGraphicsView*>(parent);
    m_selectionGroup = 0;

    // AutoCut
    m_cutAuto_tolerance = 3;
    m_constraintHeight = false;
    m_constraintWidth = false;

    setItemIndexMethod(QGraphicsScene::NoIndex);
}

/*
 * Getter / Setter
 */
void WorkArea::setBackground(const QString &filename)
{
    if(!filename.isEmpty())
    {
        // Load File
        m_background = QPixmap(filename);

        if(m_background.hasAlpha()) m_background_masked = m_background;

        // Scene size init
        setSceneRect(QRectF(QPointF(0,0),m_background.size()));

        // Add Image to Scene
        m_backgroundItem = addPixmap(m_background);

        // Border
        addRect(m_background.rect());
    }
}

QPixmap WorkArea::background() const
{
    if(!m_background_masked.isNull()) return m_background_masked;
    else return m_background;
}

void WorkArea::setTool(WorkArea::Tool tool)
{
    m_tool = tool;
}

void WorkArea::setAnimation(Animation *animation)
{
    // Remove previous animation items
    if(m_animation)
    {
        for(int i=0;i<m_animation->cuts()->size();i++)
        {
            removeItem(m_animation->cuts()->at(i));
        }
    }

    m_animation = animation;
    if(animation)
    {
        m_animation->setMarks(&m_marks);
        m_animation->setBackground(background());

        // Add existing cuts of animation
        for(int i=0;i<m_animation->cuts()->size();i++)
        {
            addItem(m_animation->cuts()->at(i));
        }
    }
}
Animation* WorkArea::animation()
{
    return m_animation;
}

/*
 * Mouse Event
 */
void WorkArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Prevent empty animation
    if(!m_animation)
    {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    QPointF position = event->scenePos();

    isSelecting = false;

    // Prevent draw selection when cursor is on a cut
    if(itemAt(position))
    {
        QGraphicsObject *obj = itemAt(position)->toGraphicsObject();
        if(obj && obj->inherits("Cut"))
        {
            // Select one cut
            m_selectedCuts.clear();
            m_selectedCuts << obj;
            emit itemSelected(qobject_cast<Cut*>(obj));
            QGraphicsScene::mousePressEvent(event);
            return;
        }
    }
    // Prevent cursor out of scene
    if(!sceneRect().contains(position))
    {
        QGraphicsScene::mousePressEvent(event);
        return;
    }
    // Clear old selection group
    if(m_selectionGroup)
    {
        destroyItemGroup(m_selectionGroup);
        m_selectionGroup = 0;
    }

    switch(m_tool)
    {
    case ToolCutRect:
        m_animation->magnetize(&position);
        m_originDrawing = position;
        m_currentDrawing = addCut(QRectF(m_originDrawing,QSize()));
        m_currentDrawing->setReady(false);
        break;
    case ToolCutGrid:
        m_animation->magnetize(&position);
        m_originDrawing = position;
        m_currentDrawing = addCut(QRectF(m_originDrawing,QSize()),WorkArea::TypeCutGrid);
        break;
    case ToolMarkH:
        m_currentMark = addMark(position.y(),Mark::MarkHorizontal);
        break;
    case ToolMarkV:
        m_currentMark = addMark(position.x(),Mark::MarkVertical);
        break;
    case ToolPicker:
        setMaskColor(m_background.toImage().pixel(position.toPoint()));
        break;
    case ToolNormal:
    case ToolCutAuto:
        m_selectionOrigin = m_view->mapFromScene(position.toPoint());
        if(!m_selectionRect)
            m_selectionRect = new QRubberBand(QRubberBand::Rectangle,m_view);
        m_selectionRect->setGeometry(QRect(m_selectionOrigin,QSize()));
        m_selectionRect->show();
        isSelecting = true;
        break;
    default:
        break;
    }
    QGraphicsScene::mousePressEvent(event);
}

void WorkArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Prevent empty animation
    if(!m_animation)
    {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }

    QRect selectionGeometryMapped;
    switch(m_tool)
    {
    case ToolCutRect:
    case ToolCutGrid:
        if(m_currentDrawing)
        {
            m_currentDrawing->setReady(true);
            m_currentDrawing->update();
            m_currentDrawing = 0;
            m_animation->showNextFrame();
        }
        break;
    case ToolMarkH:
    case ToolMarkV:
        m_currentMark = 0;
        break;
    case ToolPicker:
        setMaskColor(m_background.toImage().pixel(event->scenePos().toPoint()));
        break;
    case ToolCutAuto:
        if(!isSelecting)
        {
            isSelecting = false;
            break;
        }
        autoCut(m_view->mapToScene(m_selectionRect->geometry()).boundingRect().toRect());
        m_selectionRect->hide();
        m_animation->showNextFrame();
        break;
    case ToolNormal:
        if(!isSelecting)
        {
            isSelecting = false;
            break;
        }

        Cut *cut;
        selectionGeometryMapped = m_view->mapToScene(m_selectionRect->geometry()).boundingRect().toRect();
        m_selectedCuts.clear();
        for(int i=0;i<m_animation->cuts()->size();i++)
        {
            cut = m_animation->cuts()->at(i);
            if(selectionGeometryMapped.intersects(cut->geometry().toRect()))
            {
                m_selectedCuts << cut;
                cut->setSelected(true);
            }
            else
                cut->setSelected(false);
        }

        if(!m_selectedCuts.isEmpty())
        {
            m_selectionGroup = createItemGroup(m_selectedCuts);
            m_selectionGroup->setFlag(QGraphicsItem::ItemIsMovable);
            m_selectionGroup->setFlag(QGraphicsItem::ItemIsSelectable);
            m_selectionGroup->setSelected(true);

            if(m_selectedCuts.size() > 1)
                emit multipleSelection();
            else
                emit itemSelected(qobject_cast<Cut*>(m_selectedCuts.first()->toGraphicsObject()));
        }

        m_selectionRect->hide();
        break;
    default:
        break;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void WorkArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Prevent empty animation
    if(!m_animation)
    {
         QGraphicsScene::mouseMoveEvent(event);
         return;
    }


    QPointF position = event->scenePos().toPoint();
    bool inScene = sceneRect().contains(position);

    switch(m_tool)
    {
    case ToolCutRect:
    case ToolCutGrid:
        if(inScene && m_currentDrawing)
        {
            m_animation->magnetize(&position);
            m_currentDrawing->setGeometry(QRectF(m_originDrawing,position).normalized());
        }
        break;
    case ToolMarkH:
        if(inScene && m_currentMark)
        {
            m_currentMark->setPosition(position.y());
        }
        break;
    case ToolMarkV:
        if(inScene && m_currentMark)
        {
            m_currentMark->setPosition(position.x());
        }
        break;
    case ToolNormal:
    case ToolCutAuto:
        if(isSelecting)
        {
            m_selectionRect->setGeometry(QRect(m_selectionOrigin, m_view->mapFromScene(event->scenePos().toPoint())).normalized());
        }
        break;
    default:
    case ToolPicker:
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void WorkArea::keyReleaseEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace))
    {
        deleteSelection();
    }
}

/*
 * Private Methods
 */
void WorkArea::refreshAll()
{
    // Clear scene
    removeItem(m_backgroundItem);

    // Add Background
    if(!m_background_masked.isNull())
        m_backgroundItem = addPixmap(m_background_masked);
    else if(!m_background.isNull())
        m_backgroundItem = addPixmap(m_background);
}

void WorkArea::autoCut(const QRect &area)
{
    QImage image = m_background_masked.copy(area).toImage();
    QList<QPoint> shapePixel;
    QRgb transparent = qRgba(0,0,0,0);
    int index = 0, sx =0, sy = 0,
            x_min = 0, x_max = 0, y_min = 0, y_max = 0;

    // For Constraints
    QList<Cut*> genCuts;
    bool constraints = m_constraintHeight || m_constraintWidth;
    int maxHeight = 0, maxWidth = 0;

    for(int y=0;y<image.height()-1;y++)
    {
        for(int x=0;x<image.width()-1;x++)
        {
            if(qAlpha(image.pixel(x,y)) > 0)
            {
                x_min = x; x_max = x;
                y_min = y; y_max = y;

                shapePixel.append(QPoint(x,y));
                while(index < shapePixel.size())
                {
                    sx = shapePixel.at(index).x();
                    sy = shapePixel.at(index).y();

                    for(int py=-m_cutAuto_tolerance;py <= m_cutAuto_tolerance;++py)
                    {
                        for(int px=-m_cutAuto_tolerance;px <= m_cutAuto_tolerance; ++px)
                        {
                            if(image.rect().contains(sx+px,sy+py) && qAlpha(image.pixel(sx+px,sy+py)) > 0)
                            {
                                shapePixel.append(QPoint(sx+px,sy+py));
                                image.setPixel(sx+px,sy+py,transparent);
                            }
                        }
                    }

                    if(sx < x_min) x_min = sx;
                    if(sx > x_max) x_max = sx;
                    if(sy < y_min) y_min = sy;
                    if(sy > y_max) y_max = sy;

                    image.setPixel(sx,sy,transparent);
                    index++;
                }

                // Block all other sub-rect in main Rect
                for(int iy=y_min;iy<y_max;iy++)
                {
                    for(int ix=x_min;ix<x_max;ix++)
                    {
                        if(qAlpha(image.pixel(ix,iy)) > 0)
                            image.setPixel(ix,iy,transparent);
                    }
                }

                Cut* cut = addCut(QRectF(x_min+area.x(),y_min+area.y(),x_max-x_min+1,y_max-y_min+1));
                if(constraints)
                {
                    genCuts.append(cut);
                    if(m_constraintHeight) maxHeight = qMax(maxHeight,y_max-y_min+1);
                    if(m_constraintWidth) maxWidth = qMax(maxWidth,x_max-x_min+1);
                }
                index = 0;
                shapePixel.clear();
            }
        }
    }

    if(constraints)
    {
        for(int i=0;i<genCuts.size();i++)
        {
            Cut* cut = genCuts.at(i);
            if(m_constraintHeight) cut->setHeight(maxHeight);
            if(m_constraintWidth) cut->setWidth(maxWidth);
        }
    }
}

/*
 * Public Methods
 */
void WorkArea::setMaskColor(QRgb color)
{
    // Copy Original background
    m_background_masked = QPixmap(m_background);
    // Create and apply new mask
    m_background_masked.setMask(m_background.createMaskFromColor(color));

    // Refresh Scene
    refreshAll();

    // Update animation
    if(m_animation)
        m_animation->setBackground(m_background_masked);

    // Send new color to Interface
    emit pickerColor(color);
}

Cut* WorkArea::addCut(const QRectF &geometry, WorkArea::CutType cutType)
{
    Cut* cut = new Cut(geometry);
    connect(cut,SIGNAL(geometryChanged()),m_interface,SLOT(cut_GeometryChange()));

    if(cutType == TypeCutGrid)
    {
        cut->setRow(m_cutGrid_row);
        cut->setColumn(m_cutGrid_column);
    }

    m_animation->addFrameCount(cut->frameCount());
    m_animation->addCut(cut);

    // Add cut to scene
    addItem(cut);

    return cut;
}

Mark* WorkArea::addMark(int position, Mark::MarkOrientation orientation)
{
    Mark* mark = new Mark(orientation,position,width()-1,height()-1);
    connect(mark,SIGNAL(positionChange()),m_interface,SLOT(mark_PositionChange()));

    m_marks.append(mark);

    // Add To Scene
    addItem(mark);

    return mark;
}

void WorkArea::deleteCut(Cut *cut)
{
    m_animation->deleteCut(cut);
}

void WorkArea::removeAnimation()
{
    // Remove previous animation items
    if(m_animation)
    {
        for(int i=0;i<m_animation->cuts()->size();i++)
        {
            removeItem(m_animation->cuts()->at(i));
        }
    }
    m_animation = 0;
}

void WorkArea::setItemWidth(int width)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        if(sceneRect().contains(cut->x()+width,0)) cut->setWidth(width);
    }
}
void WorkArea::setItemHeight(int height)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        if(sceneRect().contains(0,cut->y()+height)) cut->setHeight(height);
    }
}
void WorkArea::setItemX(int x)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        if(sceneRect().contains(x,0)) cut->setX(x);
    }
}
void WorkArea::setItemY(int y)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        if(sceneRect().contains(0,y)) cut->setY(y);
    }
}
void WorkArea::setItemRow(int row)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        m_animation->addFrameCount( (row-cut->row()) * cut->column());
        cut->setRow(row);
        update(cut->geometry());
    }
}
void WorkArea::setItemColumn(int column)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        m_animation->addFrameCount( (column-cut->column()) * cut->row());
        cut->setColumn(column);
        update(cut->geometry());
    }
}
void WorkArea::setItemCellWidth(int cellWidth)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        if(sceneRect().contains(cut->column()*cellWidth,0)) cut->setCellWidth(cellWidth);
    }
}
void WorkArea::setItemCellHeight(int cellHeight)
{
    if(m_selectedCuts.size() != 1) return;

    Cut *cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        if(sceneRect().contains(0,cut->row()*cellHeight)) cut->setCellHeight(cellHeight);
    }
}

void WorkArea::toggledMagnet(bool checked) { m_animation->setMagnetism(checked); }

void WorkArea::alignLeft()
{
    if(m_selectedCuts.size() <= 1 || !m_selectionGroup) return;

    Cut *cut = 0;
    qreal left = width();

    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        left = qMin(left,cut->geometry().left());
    }
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setX(left);
    }

    destroyItemGroup(m_selectionGroup);
    m_selectionGroup = 0;
}
void WorkArea::alignRight()
{
    if(m_selectedCuts.size() <= 1 || !m_selectionGroup) return;

    qreal right = 0;
    Cut *cut = 0;

    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        right = qMax(right,cut->geometry().right());
    }
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setX(cut->x() + right - cut->geometry().right());
    }
    destroyItemGroup(m_selectionGroup);
    m_selectionGroup = 0;
}
void WorkArea::alignCenterH()
{
    if(m_selectedCuts.size() <= 1 || !m_selectionGroup) return;

    qreal centerMax = -1, centerMin = width(), center = 0;
    Cut *cut = 0;

    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        center = cut->geometry().x() + cut->geometry().width()/2;
        centerMin = qMin(centerMin,center);
        centerMax = qMax(centerMax,center);
    }
    center =  centerMin + (centerMax-centerMin)/2;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setX(center - cut->geometry().width()/2);
    }
    destroyItemGroup(m_selectionGroup);
    m_selectionGroup = 0;
}
void WorkArea::alignTop()
{
    if(m_selectedCuts.size() <= 1 || !m_selectionGroup) return;

    Cut *cut = 0;
    qreal top = height();

    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        top = qMin(top,cut->geometry().top());
    }
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setY(top);
    }

    destroyItemGroup(m_selectionGroup);
    m_selectionGroup = 0;
}
void WorkArea::alignBottom()
{
    if(m_selectedCuts.size() <= 1 || !m_selectionGroup) return;

    Cut *cut = 0;
    qreal bottom = 0;

    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        bottom = qMax(bottom,cut->geometry().bottom());
    }
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setY(cut->y() + bottom - cut->geometry().bottom());
    }

    destroyItemGroup(m_selectionGroup);
    m_selectionGroup = 0;
}
void WorkArea::alignCenterV()
{
    if(m_selectedCuts.size() <= 1 || !m_selectionGroup) return;

    qreal centerMax = -1, centerMin = height(), center = 0;
    Cut *cut = 0;

    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        center = cut->geometry().y() + cut->geometry().height()/2;
        centerMin = qMin(centerMin,center);
        centerMax = qMax(centerMax,center);
    }
    center = centerMin + (centerMax-centerMin)/2;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setY(center - cut->geometry().height()/2);
    }
    destroyItemGroup(m_selectionGroup);
    m_selectionGroup = 0;
}

void WorkArea::alignContentLeft()
{
    if(m_selectedCuts.size() <= 0) return;

    Cut* cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setX(cut->x() + contentMinX(cut));
    }
}
void WorkArea::alignContentRight()
{
    if(m_selectedCuts.size() <= 0) return;

    Cut* cut=0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setX((int)cut->x() -  ((int)cut->width()-1-contentMaxX(cut)));
    }
}
void WorkArea::alignContentCenterH()
{
    if(m_selectedCuts.size() <= 0) return;

    Cut* cut=0;
    int xMin=0,xMax=0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        xMin = contentMinX(cut);
        xMax = contentMaxX(cut);
        cut->setX( cut->x() + xMin + (xMax-xMin)/2 - cut->width()/2 );
    }
}
void WorkArea::alignContentTop()
{
    if(m_selectedCuts.size() <= 0) return;

    Cut* cut = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setY(cut->y() + contentMinY(cut));
    }
}
void WorkArea::alignContentBottom()
{
    if(m_selectedCuts.size() <= 0) return;

    Cut* cut=0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        cut->setY((int)cut->y() -  ((int)cut->height()-1-contentMaxY(cut)));
    }
}
void WorkArea::alignContentCenterV()
{
    if(m_selectedCuts.size() <= 0) return;

    Cut* cut=0;
    int yMin = 0, yMax = 0;
    for(int i=0;i<m_selectedCuts.size();i++)
    {
        cut = qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject());
        yMin = contentMinY(cut);
        yMax = contentMaxY(cut);
        cut->setY( cut->y() + yMin + (yMax-yMin)/2 - cut->height()/2 );
    }
}
int WorkArea::contentMaxX(Cut *cut)
{
    QImage image = m_background_masked.copy(cut->geometry().toRect()).toImage();

    for(int x=image.width()-1;x>0;x--)
        for(int y=0;y<image.height()-1;y++)
            if(qAlpha(image.pixel(x,y))) return x;

    return cut->geometry().right();
}
int WorkArea::contentMinX(Cut *cut)
{
    QImage image = m_background_masked.copy(cut->geometry().toRect()).toImage();

    for(int x=0;x<image.width()-1;x++)
        for(int y=0;y<image.height()-1;y++)
            if(qAlpha(image.pixel(x,y))) return x;

    return cut->geometry().left();
}
int WorkArea::contentMaxY(Cut *cut)
{
    QImage image = m_background_masked.copy(cut->geometry().toRect()).toImage();

    for(int y=image.height()-1;y>0;y--)
        for(int x=0;x<image.width()-1;x++)
            if(qAlpha(image.pixel(x,y))) return y;

    return cut->geometry().bottom();
}
int WorkArea::contentMinY(Cut *cut)
{
    QImage image = m_background_masked.copy(cut->geometry().toRect()).toImage();

    for(int y=0;y<image.height()-1;y++)
        for(int x=0;x<image.width()-1;x++)
            if(qAlpha(image.pixel(x,y))) return y;

    return cut->geometry().top();
}

void WorkArea::changeTolerance(int tolerance) { m_cutAuto_tolerance = tolerance; }
void WorkArea::changeConstraintHeight(bool enabled) { m_constraintHeight = enabled; }
void WorkArea::changeConstraintWidth(bool enabled) { m_constraintWidth = enabled; }

// Animation
void WorkArea::setAnimationTitle(const QString &title)
{
    if(m_animation)
    {
        m_animation->setTitle(title);
    }
}

void WorkArea::deleteSelection()
{
    if(!m_selectedCuts.isEmpty())
    {
        if(m_selectionGroup)
        {
            destroyItemGroup(m_selectionGroup);
            m_selectionGroup = 0;
        }

        for(int i=0;i<m_selectedCuts.size();i++)
        {
            removeItem(m_selectedCuts.at(i));
            deleteCut(qobject_cast<Cut*>(m_selectedCuts.at(i)->toGraphicsObject()));
        }
        m_selectedCuts.clear();
        m_animation->showNextFrame();
    }
}
