#include "interface.h"
#include "ui_interface.h"
#include "application.h"

Interface::Interface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Interface)
{
    ui->setupUi(this);

    /*
     * Initialisation
     */

    // Center Window on Screen
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());

    // menuTools
    m_currentToolSelected = ui->toolNormal;

    // Main Scene
    m_workarea = new WorkArea(this,ui->workArea);
    ui->workArea->setScene(m_workarea);

    // configMark
    ui->mark_L_Vertical->setEnabled(false);
    ui->mark_I_X->setEnabled(false);
    ui->mark_L_X->setEnabled(false);

    // Variables
    m_zoomRatio = 1.0;
    m_zoomViewRatio = 1.0;
    m_workModified = false;
    m_selectAnimation = false;
    m_aboutDialog = new AboutDialog(this->centralWidget());
    m_aboutDialog->hide();

    /*
     * Signal / Slot
     */

    // Tools
    connect(ui->toolNormal,SIGNAL(triggered()),this,SLOT(tool_normal()));
    connect(ui->toolZoomIn,SIGNAL(triggered()),this,SLOT(tool_zoomIn()));
    connect(ui->toolZoomOut,SIGNAL(triggered()),this,SLOT(tool_zoomOut()));
    connect(ui->toolCutRect,SIGNAL(triggered()),this,SLOT(tool_cutRect()));
    connect(ui->toolCutGrid,SIGNAL(triggered()),this,SLOT(tool_cutGrid()));
    connect(ui->toolMark,SIGNAL(triggered()),this,SLOT(tool_mark()));
    connect(ui->toolPicker,SIGNAL(triggered()),this,SLOT(tool_picker()));
    connect(ui->toolCutAuto,SIGNAL(triggered()),this,SLOT(tool_cutAuto()));

    // Menu
    connect(ui->menuOpen,SIGNAL(triggered()),this,SLOT(menu_open()));
    connect(ui->menuSave,SIGNAL(triggered()),this,SLOT(menu_save()));
    connect(ui->menuSaveAs,SIGNAL(triggered()),this,SLOT(menu_saveAs()));
    connect(ui->menuQuit,SIGNAL(triggered()),this,SLOT(menu_quit()));
    connect(ui->menuAbout,SIGNAL(triggered()),this,SLOT(menu_about()));

    // configCutRect
    connect(ui->cutRect_I_Width,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemWidth(int)));
    connect(ui->cutRect_I_Height,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemHeight(int)));
    connect(ui->cutRect_I_X,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemX(int)));
    connect(ui->cutRect_I_Y,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemY(int)));
    connect(ui->cutRect_B_Add,SIGNAL(clicked()),this,SLOT(addCutRect()));
    connect(ui->cutRect_I_Magnetism,SIGNAL(toggled(bool)),m_workarea,SLOT(toggledMagnet(bool)));
    connect(ui->cutRect_B_Remove,SIGNAL(clicked()),m_workarea,SLOT(deleteSelection()));

    // configCutGrid
    connect(ui->cutGrid_I_Width,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemWidth(int)));
    connect(ui->cutGrid_I_Height,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemHeight(int)));
    connect(ui->cutGrid_I_X,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemX(int)));
    connect(ui->cutGrid_I_Y,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemY(int)));
    connect(ui->cutGrid_I_Row,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemRow(int)));
    connect(ui->cutGrid_I_Column,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemColumn(int)));
    connect(ui->cutGrid_B_Add,SIGNAL(clicked()),this,SLOT(addCutGrid()));
    connect(ui->cutGrid_I_Magnetism,SIGNAL(toggled(bool)),m_workarea,SLOT(toggledMagnet(bool)));
    connect(ui->cutGrid_B_Remove,SIGNAL(clicked()),m_workarea,SLOT(deleteSelection()));
    connect(ui->cutGrid_I_CellHeight,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemCellHeight(int)));
    connect(ui->cutGrid_I_CellWidth,SIGNAL(valueChanged(int)),m_workarea,SLOT(setItemCellWidth(int)));

    // Link configCutGrid & configCutRect
    connect(ui->cutGrid_I_Magnetism,SIGNAL(toggled(bool)),ui->cutRect_I_Magnetism,SLOT(setChecked(bool)));
    connect(ui->cutRect_I_Magnetism,SIGNAL(toggled(bool)),ui->cutGrid_I_Magnetism,SLOT(setChecked(bool)));

    // configCutAuto
    connect(ui->cutAuto_I_Tolerance,SIGNAL(valueChanged(int)),m_workarea,SLOT(changeTolerance(int)));
    connect(ui->cutAuto_I_ConstraintHeight,SIGNAL(toggled(bool)),m_workarea,SLOT(changeConstraintHeight(bool)));
    connect(ui->cutAuto_I_ConstraintWidth,SIGNAL(toggled(bool)),m_workarea,SLOT(changeConstraintWidth(bool)));

    // configMark
    connect(ui->mark_I_Orientation,SIGNAL(currentIndexChanged(int)),this,SLOT(mark_OrientationChange(int)));
    connect(ui->mark_B_Add,SIGNAL(clicked()),this,SLOT(addMark()));

    // configPicker
    connect(ui->picker_B_ColorPicker,SIGNAL(clicked()),this,SLOT(picker_OpenColorPicker()));
    connect(ui->picker_B_Valid,SIGNAL(clicked()),this,SLOT(picker_SetMaskColor()));

    // Viewer
    connect(ui->viewer_I_ZoomIn,SIGNAL(clicked()),this,SLOT(viewer_zoomIn()));
    connect(ui->viewer_I_ZoomOut,SIGNAL(clicked()),this,SLOT(viewer_zoomOut()));

    // Animation Panel
    connect(ui->animation_I_Add,SIGNAL(clicked()),this,SLOT(animation_add()));
    connect(ui->animation_I_Remove,SIGNAL(clicked()),this,SLOT(animation_remove()));
    connect(ui->animation_I_List,SIGNAL(currentIndexChanged(int)),this,SLOT(selectAnimation(int)));
    connect(ui->animation_I_Title,SIGNAL(textChanged(QString)),this,SLOT(animation_changeTitle(QString)));

    // Alignements
    connect(ui->align_sceneLeft,SIGNAL(clicked()),m_workarea,SLOT(alignLeft()));
    connect(ui->align_sceneRight,SIGNAL(clicked()),m_workarea,SLOT(alignRight()));
    connect(ui->align_sceneCenterH,SIGNAL(clicked()),m_workarea,SLOT(alignCenterH()));
    connect(ui->align_sceneTop,SIGNAL(clicked()),m_workarea,SLOT(alignTop()));
    connect(ui->align_sceneBottom,SIGNAL(clicked()),m_workarea,SLOT(alignBottom()));
    connect(ui->align_sceneCenterV,SIGNAL(clicked()),m_workarea,SLOT(alignCenterV()));

    connect(ui->align_contentLeft,SIGNAL(clicked()),m_workarea,SLOT(alignContentLeft()));
    connect(ui->align_contentRight,SIGNAL(clicked()),m_workarea,SLOT(alignContentRight()));
    connect(ui->align_contentCenterH,SIGNAL(clicked()),m_workarea,SLOT(alignContentCenterH()));
    connect(ui->align_contentTop,SIGNAL(clicked()),m_workarea,SLOT(alignContentTop()));
    connect(ui->align_contentBottom,SIGNAL(clicked()),m_workarea,SLOT(alignContentBottom()));
    connect(ui->align_contentCenterV,SIGNAL(clicked()),m_workarea,SLOT(alignContentCenterV()));

    // WorkArea
    connect(m_workarea,SIGNAL(pickerColor(QRgb)),this,SLOT(pickerColor(QRgb)));
    connect(m_workarea,SIGNAL(itemSelected(Cut*)),this,SLOT(itemSelected(Cut*)));
    connect(m_workarea,SIGNAL(multipleSelection()),this,SLOT(multipleSelection()));

    /*
     * Interface
     */

    // Default Interface Config
    ui->menuview_Sequence->setChecked(false);
    ui->dockSequence->close();
    ui->menuview_Sequence->setEnabled(false);
    this->adjustSize();

    ui->workArea->setBackgroundBrush(QBrush(QPixmap(":/resources/checkerboard.png")));

    /*
     * Translation
     */
    fillLanguages();
    retranslate();
    m_languages->actions().first()->toggle();
}

Interface::~Interface()
{
    delete ui;
}

void Interface::closeEvent(QCloseEvent *event)
{
    if (m_workModified)
    {
        saveModification();
        event->accept();
    }
    QMainWindow::closeEvent(event);
}

void Interface::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        retranslate();
    }
    QMainWindow::changeEvent(event);
}

/***** SLOTS *****/

// Tools
void Interface::tool_normal()
{
    if(m_currentToolSelected != ui->toolNormal)
    {
        m_currentToolSelected->setChecked(false);
        m_currentToolSelected = ui->toolNormal;

        this->setCursor(Qt::ArrowCursor);

        // WorkArea
        m_workarea->setTool(WorkArea::ToolNormal);

        // StatsBar
        ui->statusBar->clearMessage();
    }
    else
    {
        m_currentToolSelected->setChecked(true);
    }
}
void Interface::tool_zoomIn()
{
    // Maximum zoom 3200%
    if(m_zoomRatio < 32)
    {
        m_zoomRatio *= 2;
        ui->workArea->scale(2.0,2.0);
        setWindowTitle("Sprite Decomposer - "+QString::number(100*m_zoomRatio)+"%");
    }
}
void Interface::tool_zoomOut()
{
    // Minimum zoom 12,5%
    if(m_zoomRatio > 0.125)
    {
        m_zoomRatio /= 2;
        ui->workArea->scale(0.5,0.5);
        setWindowTitle("Sprite Decomposer - "+QString::number(100*m_zoomRatio)+"%");
    }
}
void Interface::tool_cutRect()
{
    // Switch tool
    if(m_currentToolSelected != ui->toolCutRect)
    {
        // Interface
        m_currentToolSelected->setChecked(false);
        m_currentToolSelected = ui->toolCutRect;
        ui->configurePanel->setCurrentIndex(0);
        this->setCursor(Qt::CrossCursor);

        // WorkArea
        m_workarea->setTool(WorkArea::ToolCutRect);

        // StatsBar
        ui->statusBar->showMessage(trUtf8("Ajouter un découpage rectangulaire"));
    }
    // Already selected
    else
    {
        m_currentToolSelected->setChecked(true);
    }
}
void Interface::tool_cutGrid()
{
    // Switch tool
    if(m_currentToolSelected != ui->toolCutGrid)
    {
        // Interface
        m_currentToolSelected->setChecked(false);
        m_currentToolSelected = ui->toolCutGrid;
        ui->configurePanel->setCurrentIndex(1);
        this->setCursor(Qt::CrossCursor);

        // WorkArea
        m_workarea->setTool(WorkArea::ToolCutGrid);

        // StatsBar
        ui->statusBar->showMessage(trUtf8("Ajouter un découpage en grille"));
    }
    // Already selected
    else
    {
        m_currentToolSelected->setChecked(true);
    }
}
void Interface::tool_mark()
{
    // Switch tool
    if(m_currentToolSelected != ui->toolMark)
    {
        // Interface
        m_currentToolSelected->setChecked(false);
        m_currentToolSelected = ui->toolMark;
        ui->configurePanel->setCurrentIndex(3);
        ui->mark_I_Orientation->setCurrentIndex(0);
        this->setCursor(Qt::ArrowCursor);

        // WorkArea
        m_workarea->setTool(WorkArea::ToolMarkH);

        // StatsBar
        ui->statusBar->showMessage(trUtf8("Ajouter un repère"));
    }
    // Already selected
    else
    {
        m_currentToolSelected->setChecked(true);
    }
}
void Interface::tool_picker()
{
    // Switch tool
    if(m_currentToolSelected != ui->toolPicker)
    {
        // Interface
        m_currentToolSelected->setChecked(false);
        m_currentToolSelected = ui->toolPicker;
        ui->configurePanel->setCurrentIndex(4);
        this->setCursor(Qt::CrossCursor);

        // WorkArea
        m_workarea->setTool(WorkArea::ToolPicker);

        // StatsBar
        ui->statusBar->showMessage(trUtf8("Choisir la couleur de transparence"));
    }
    // Already selected
    else
    {
        m_currentToolSelected->setChecked(true);
    }
}
void Interface::tool_cutAuto()
{
    // Switch tool
    if(m_currentToolSelected != ui->toolCutAuto)
    {
        // Interface
        m_currentToolSelected->setChecked(false);
        m_currentToolSelected = ui->toolCutAuto;
        ui->configurePanel->setCurrentIndex(2);
        this->setCursor(Qt::CrossCursor);

        // WorkArea
        m_workarea->setTool(WorkArea::ToolCutAuto);

        // StatsBar
        ui->statusBar->showMessage(trUtf8("Définir une zone à découper automatiquement"));
    }
    // Already selected
    else
    {
        m_currentToolSelected->setChecked(true);
    }
}

// Menu
void Interface::menu_open()
{
    // Document opened must be saved
    if(m_workModified && !m_imageFilename.isEmpty())
    {
        if(saveModification() < 0) return;
    }

    // Open file
    QString filename = QFileDialog::getOpenFileName(this,trUtf8("Ouvrir un fichier"),"",trUtf8("Tous les fichiers(*.*);;Fichier image (*.png *.jpg *.bmp *.gif);;Fichier XML(*.xml)")); //*/"/Users/Nicolas/Pictures/Resources/Sprite/NessSBBB.png";

    if(!filename.isEmpty())
    {
        // Clear all stuff
        closeAll();

        QString extension = filename.right(3).toLower();
        if(extension == "xml")
            openXMl(filename);
        else if(extension == "png" || extension == "jpg" || extension == "bmp" || extension == "gif")
            openImage(filename);
        else
            QMessageBox::warning(this,trUtf8("Impossible d'ouvrir le fichier"),trUtf8("Type de fichier non supporté"));

        m_workModified = false;
        setWindowModified(false);
    }
}
void Interface::menu_save()
{
    if(!m_xmlFilename.isEmpty())
        saveAll(m_xmlFilename);
    else
        menu_saveAs();
}
void Interface::menu_saveAs()
{
    QString saveFile = QFileDialog::getSaveFileName(this,trUtf8("Enregistrer sous..."),QString(),trUtf8("Fichier XML(*.xml)"));
    saveAll(saveFile);
}

void Interface::menu_quit()
{
    this->close();
}

void Interface::menu_about()
{
    m_aboutDialog->show();
}

// configurePanel
void Interface::configurePanelChanged(int index)
{
    switch(index)
    {
    // Cut Rect
    case 0:
        ui->toolCutRect->setChecked(true);
        tool_cutRect();
        break;
        // Cut Grid
    case 1:
        ui->toolCutGrid->setChecked(true);
        tool_cutGrid();
        break;
        // Mark
    case 2:
        ui->toolMark->setChecked(true);
        tool_mark();
        break;
        // Picker
    case 3:
        ui->toolPicker->setChecked(true);
        tool_picker();
        break;
    default:
        ui->toolNormal->setChecked(true);
        tool_normal();
        break;
    }
}

// configureCut
void Interface::addCutRect()
{
    if(ui->toolCutRect->isEnabled() && !m_imageFilename.isEmpty())
    {
        m_workarea->addCut(QRectF(ui->cutRect_I_X->value(),ui->cutRect_I_Y->value(),ui->cutRect_I_Width->value(),ui->cutRect_I_Height->value()),WorkArea::TypeCutRect);
        m_workModified = true; setWindowModified(true);
    }
}
void Interface::addCutGrid()
{
    if(ui->toolCutGrid->isEnabled() && !m_imageFilename.isEmpty())
    {
        m_workarea->addCut(QRectF(ui->cutGrid_I_X->value(),ui->cutGrid_I_Y->value(),ui->cutGrid_I_Width->value(),ui->cutGrid_I_Height->value()),WorkArea::TypeCutGrid);
        m_workModified = true; setWindowModified(true);
    }
}
void Interface::cut_GeometryChange()
{
    Cut *from = qobject_cast<Cut*>(sender());
    if(!from) return;

    if(from->row() > 1 || from->column() > 1)
        showCutGridInfo(from);
    else
        showCutRectInfo(from);

    m_workModified = true; setWindowModified(true);
}


// configMark
void Interface::addMark()
{
    if(ui->toolMark->isEnabled())
    {
        if(ui->mark_I_Orientation->currentIndex() == Mark::MarkHorizontal)
            m_workarea->addMark(ui->mark_I_Y->text().toInt(),Mark::MarkHorizontal);

        else if(ui->mark_I_Orientation->currentIndex() == Mark::MarkVertical)
            m_workarea->addMark(ui->mark_I_X->text().toInt(),Mark::MarkVertical);
    }
}

void Interface::mark_OrientationChange(int index)
{
    switch(index)
    {
    case Mark::MarkHorizontal: // Horizontal
        ui->mark_L_Horizontal->setEnabled(true);
        ui->mark_I_Y->setEnabled(true);
        ui->mark_L_Y->setEnabled(true);
        ui->mark_L_Vertical->setEnabled(false);
        ui->mark_I_X->setEnabled(false);
        ui->mark_I_X->clear();
        ui->mark_L_X->setEnabled(false);

        m_workarea->setTool(WorkArea::ToolMarkH);

        this->setCursor(Qt::ArrowCursor);
        break;
    case Mark::MarkVertical: // Vertical
        ui->mark_L_Horizontal->setEnabled(false);
        ui->mark_I_Y->setEnabled(false);
        ui->mark_I_Y->clear();
        ui->mark_L_Y->setEnabled(false);
        ui->mark_L_Vertical->setEnabled(true);
        ui->mark_I_X->setEnabled(true);
        ui->mark_L_X->setEnabled(true);

        m_workarea->setTool(WorkArea::ToolMarkV);

        this->setCursor(Qt::ArrowCursor);
        break;
    }
}

void Interface::mark_PositionChange()
{
    Mark *from = qobject_cast<Mark*>(sender());
    if(!from) return;

    switch(from->orientation())
    {
    case Mark::MarkHorizontal:
        ui->mark_I_Y->setText(QString::number(from->position()));
        break;
    case Mark::MarkVertical:
        ui->mark_I_X->setText(QString::number(from->position()));
        break;
    default:
        break;
    }
}

// configPicker
void Interface::picker_OpenColorPicker()
{
    // Open Color Picker Dialog and Set Transparent Color
    if(ui->toolPicker->isEnabled() && !m_imageFilename.isEmpty())
        m_workarea->setMaskColor(QColorDialog::getColor(Qt::white,this).rgb());
}

void Interface::picker_SetMaskColor()
{
    if(ui->toolPicker->isEnabled() && !m_imageFilename.isEmpty())
    {
        QByteArray  red = ui->picker_I_Color->text().mid(0,2).toAscii(),
                green = ui->picker_I_Color->text().mid(2,2).toAscii(),
                blue = ui->picker_I_Color->text().mid(4,2).toAscii();
        QColor color(red.toInt(0,16),green.toInt(0,16),blue.toInt(0,16));
        if(color.isValid())
        {
            m_workarea->setMaskColor(color.rgb());
            m_workModified = true; setWindowModified(true);
        }
    }
}

// Viewer
void Interface::viewer_zoomIn()
{
    // Maximum zoom 3200%
    if(m_zoomViewRatio < 32 && ui->toolZoomIn->isEnabled())
    {
        m_zoomViewRatio *= 2;
        ui->viewer->scale(2.0,2.0);
    }
}
void Interface::viewer_zoomOut()
{
    // Minimum zoom 12,5%
    if(m_zoomViewRatio > 0.125 && ui->toolZoomOut->isEnabled())
    {
        m_zoomViewRatio /= 2;
        ui->viewer->scale(0.5,0.5);
    }
}

// Animation Panel
void Interface::animation_add()
{
    if(m_imageFilename.isEmpty()) return;

    bool ok;
    QString animationName = QInputDialog::getText(this, tr("Nouvelle animation"),tr("Nom de l'animation:"), QLineEdit::Normal, QString(), &ok);
    if (ok && !animationName.isEmpty())
    {
        addAnimation(animationName);
        m_workModified = true; setWindowModified(true);
    }
}
void Interface::animation_remove()
{
    if(m_imageFilename.isEmpty()) return;

    removeAnimation(ui->animation_I_List->currentIndex());
    m_workModified = true; setWindowModified(true);
}
void Interface::animation_changeTitle(QString title)
{
    if(m_imageFilename.isEmpty()) return;

    ui->animation_I_List->setItemText(ui->animation_I_List->currentIndex(),title);
    if(!m_selectAnimation)
    {
        m_workModified = true;
        setWindowModified(true);
    }
}

// WorkArea
void Interface::pickerColor(QRgb color)
{
    if(!m_imageFilename.isEmpty())
    {
        ui->picker_I_Color->setText(QString::number(color,16).right(6).toUpper());
        ui->toolCutAuto->setEnabled(true);
    }
}
void Interface::itemSelected(Cut *selection)
{
    if(!selection) return;
    if(selection->row() > 1 || selection->column() > 1)
    {
        ui->configurePanel->setCurrentIndex(1);
        showCutGridInfo(selection);
    }
    else
    {
        ui->configurePanel->setCurrentIndex(0);
        showCutRectInfo(selection);
    }
}
void Interface::multipleSelection()
{
    ui->configurePanel->setCurrentIndex(5);
}

// Animation
void Interface::frameCountUpdated(int frameCount)
{
    ui->animation_I_Image->setText(QString::number(frameCount));
}

/*
 * Private
 */

int Interface::saveModification()
{
    QMessageBox msgBox;
    msgBox.setText(trUtf8("Les animations ont été modifiées"));
    msgBox.setInformativeText(trUtf8("Voulez-vous enregistrer les changements?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Save,trUtf8("Enregistrer"));
    msgBox.setButtonText(QMessageBox::Discard,trUtf8("Ignorer"));
    msgBox.setButtonText(QMessageBox::Cancel,trUtf8("Annuler"));
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Save:
        // Save was clicked
        if(m_xmlFilename.isEmpty())
            menu_save();
        else
            menu_saveAs();
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        return -1;
        break;
    case QMessageBox::Discard:
        // Don't Save was clicked
        break;
    default:
        // should never be reached
        break;
    }
    return 1;
}

void Interface::saveAll(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
    {
        QMessageBox::critical(this,trUtf8("Erreur"),trUtf8("Impossible d'enregistrer le fichier"));
        return;
    }

    // Creation du fichier XML
    QDomDocument doc("SpriteDecomposer");
    QDomElement root = doc.createElement("sprites");

    root.setAttribute("image",m_imageFilename.split("/").last());
    root.setAttribute("transparentColor",ui->picker_I_Color->text());
    doc.appendChild(root);

    // temp
    Animation *animation = 0;
    Cut *cut = 0;
    for(int i=0;i<m_animations.size();i++)
    {
        animation = m_animations.at(i);
        QDomElement animationElm = doc.createElement("animation");
        animationElm.setAttribute("title",animation->title());
        animationElm.setAttribute("delay",animation->speed());

        for(int j=0;j<animation->cuts()->size();j++)
        {
           cut = animation->cuts()->at(j);
           QDomElement cutElm = doc.createElement("cut");
           cutElm.setAttribute("x",cut->x());
           cutElm.setAttribute("y",cut->y());
           cutElm.setAttribute("w",cut->width());
           cutElm.setAttribute("h",cut->height());
           cutElm.setAttribute("row",cut->row());
           cutElm.setAttribute("col",cut->column());

           animationElm.appendChild(cutElm);
        }
        root.appendChild(animationElm);
    }

    QTextStream ts( &file );
    ts << doc.toString();
    file.close();

    m_xmlFilename = filename;
    setWindowModified(false);
    m_workModified = false;
}

void Interface::openXMl(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this,trUtf8("Erreur"),trUtf8("Impossible d'ouvir le fichier"));
        return;
    }

    QString dirPath;
    QStringList fileDir = filename.split("/");
    for(int i=0;i<fileDir.size()-1;i++)
    {
        dirPath += "/"+fileDir.at(i);
    }

    // Ouverture du fichier XML
    QDomDocument doc("SpriteDecomposer");
    if(!doc.setContent(&file))
    {
        file.close();
        QMessageBox::critical(this,trUtf8("Erreur"),trUtf8("Impossible d'ouvir le fichier"));
        return;
    }
    QDomElement root = doc.documentElement();
    if(root.tagName() != "sprites" || !root.hasAttribute("image") || !root.hasAttribute("transparentColor"))
    {
        file.close();
        QMessageBox::critical(this,trUtf8("Format non reconnu"),trUtf8("Le contenu de ce fichier est invalide"));
        return;
    }

    // Set background
    openImage(dirPath + "/" + root.attribute("image"),false);

    // Animations
    QDomNode n = root.firstChild();
    while( !n.isNull() )
    {
        QDomElement e = n.toElement();
        if( !e.isNull() && e.tagName() == "animation" && e.hasAttribute("title") && e.hasAttribute("delay"))
        {
            Animation *animation = addAnimation(e.attribute("title"));
            animation->setSpeed(e.attribute("delay").toInt());

            // All Cuts of animation
            QDomNode nn = n.firstChild();
            while(!nn.isNull())
            {
                QDomElement ee = nn.toElement();
                if(!ee.isNull()
                    && ee.tagName() == "cut"
                    && ee.hasAttribute("x")
                    && ee.hasAttribute("y")
                    && ee.hasAttribute("w")
                    && ee.hasAttribute("h")
                    && ee.hasAttribute("row")
                    && ee.hasAttribute("col")
                  )
                {
                    m_workarea->setItemRow(ee.attribute("row").toInt());
                    m_workarea->setItemColumn(ee.attribute("column").toInt());

                    if(ee.attribute("row").toInt() > 1 || ee.attribute("column").toInt() > 1)
                        m_workarea->addCut(QRectF(ee.attribute("x").toFloat(),ee.attribute("y").toFloat(),ee.attribute("w").toFloat(),ee.attribute("h").toFloat()),WorkArea::TypeCutGrid);
                    else
                        m_workarea->addCut(QRectF(ee.attribute("x").toFloat(),ee.attribute("y").toFloat(),ee.attribute("w").toFloat(),ee.attribute("h").toFloat()));
                }
                nn = nn.nextSibling();
            }
        }
        n = n.nextSibling();
    }

    // Set Transparent
    ui->picker_I_Color->setText(root.attribute("transparentColor"));
    picker_SetMaskColor();

    file.close();
    m_xmlFilename = filename;
}

void Interface::openImage(const QString &filename,bool createDefaultAnimation)
{
    m_imageFilename = filename;

    // WorkArea Init
    m_workarea->setBackground(filename);
    if(m_workarea->background().hasAlpha()) ui->toolCutAuto->setEnabled(true);

    // Default animation
    if(createDefaultAnimation)
        addAnimation(trUtf8("Animation par défaut"));

    // Interface
    setWindowTitle("Sprite Decomposer - 100%");
}

Animation* Interface::addAnimation(const QString &title)
{
    Animation *animation = new Animation(this);
    animation->setTitle(title);
    animation->setBackground(m_workarea->background());
    m_animations.append(animation);

    // Signal / Slot
    connect(ui->viewer_I_Play,SIGNAL(toggled(bool)),animation,SLOT(playAnimation(bool)));
    connect(ui->viewer_I_First,SIGNAL(clicked()),animation,SLOT(showFirstFrame()));
    connect(ui->viewer_I_Previous,SIGNAL(clicked()),animation,SLOT(showPreviousFrame()));
    connect(ui->viewer_I_Next,SIGNAL(clicked()),animation,SLOT(showNextFrame()));
    connect(ui->viewer_I_Last,SIGNAL(clicked()),animation,SLOT(showLastFrame()));
    connect(animation,SIGNAL(frameCountUpdated(int)),this,SLOT(frameCountUpdated(int)));
    connect(ui->animation_I_Title,SIGNAL(textChanged(QString)),m_workarea,SLOT(setAnimationTitle(QString)));
    connect(ui->animation_I_Speed,SIGNAL(valueChanged(int)),animation,SLOT(setSpeed(int)));

    // Interface
    ui->animation_I_List->addItem(title);
    ui->animation_I_List->setCurrentIndex(ui->animation_I_List->count()-1);

    if(ui->animation_I_List->count() == 1)
    {
        ui->dockViewer->setEnabled(true);
        ui->dockSequence->setEnabled(true);
        ui->dockTools->setEnabled(true);
        ui->mainToolBar->setEnabled(true);
        ui->animation_I_Title->setEnabled(true);
        ui->animation_I_Speed->setEnabled(true);
    }

    return animation;
}
void Interface::removeAnimation(int index)
{
    if(index >= 0 && index < m_animations.size())
    {
        Animation *toRemove = m_animations.at(index);

        if(toRemove == m_workarea->animation()) m_workarea->removeAnimation();

        m_animations.removeAt(index);
        delete toRemove;
        ui->animation_I_List->removeItem(index);
        selectAnimation(qMax(0,index-1));
    }
}

void Interface::showCutRectInfo(Cut *cut)
{
    ui->cutRect_I_Width->setValue(cut->boundingRect().width());
    ui->cutRect_I_Height->setValue(cut->boundingRect().height());
    ui->cutRect_I_X->setValue(cut->x());
    ui->cutRect_I_Y->setValue(cut->y());
}
void Interface::showCutGridInfo(Cut *cut)
{
    ui->cutGrid_I_Width->setValue(cut->boundingRect().width());
    ui->cutGrid_I_Width->setSingleStep(cut->column());
    ui->cutGrid_I_Height->setValue(cut->boundingRect().height());
    ui->cutGrid_I_Height->setSingleStep(cut->row());
    ui->cutGrid_I_X->setValue(cut->x());
    ui->cutGrid_I_Y->setValue(cut->y());
    ui->cutGrid_I_Row->setValue(cut->row());
    ui->cutGrid_I_Column->setValue(cut->column());
    ui->cutGrid_I_CellWidth->setValue(cut->cellWidth());
    ui->cutGrid_I_CellHeight->setValue(cut->cellHeight());
}

void Interface::closeAll()
{
    for(int i=0;i<m_animations.size();i++)
        delete m_animations.at(i);
    m_animations.clear();

    m_xmlFilename.clear();
    m_imageFilename.clear();
    m_workarea->setAnimation(0);
    m_workarea->clear();

    ui->animation_I_List->clear();
    ui->picker_I_Color->clear();
    ui->animation_I_Title->clear();
}

void Interface::fillLanguages()
{
    m_languages = ui->menuLangue;

    // make a group of language actions
    QActionGroup* actions = new QActionGroup(this);
    connect(actions, SIGNAL(triggered(QAction*)), this, SLOT(setLanguage(QAction*)));

    foreach (QString avail, Application::availableLanguages())
    {
        // figure out nice names for locales
        QLocale locale(avail);
        QString language = QLocale::languageToString(locale.language());
        //QString country = QLocale::countryToString(locale.country());
        QString name = language;// + " (" + country + ")";

        // construct an action
        QAction* action = m_languages->addAction(name);
        action->setData(avail);
        action->setCheckable(true);
        actions->addAction(action);
    }
}

void Interface::retranslate()
{
    m_languages->setTitle(tr("Language"));
    m_aboutDialog->retranslate();
}

/*
 * Private Slots
 */
void Interface::selectAnimation(int index)
{
    if(index >= 0 && index < m_animations.size())
    {
        Animation *animation = m_animations.at(index);

        m_workarea->setAnimation(animation);

        // Interface
        m_selectAnimation = true;
        ui->animation_I_Title->setText(animation->title());
        ui->viewer->setScene(animation->scene());
        ui->animation_I_Image->setText(QString::number(animation->frameCount()));
        m_selectAnimation = false;
    }
    else
    {
        m_workarea->setAnimation(0);
        ui->animation_I_Title->clear();
        ui->viewer->setScene(0);
        ui->animation_I_Image->clear();

        ui->dockViewer->setEnabled(false);
        ui->dockSequence->setEnabled(false);
        ui->dockTools->setEnabled(false);
        ui->mainToolBar->setEnabled(false);
        ui->animation_I_Title->setEnabled(false);
        ui->animation_I_Speed->setEnabled(false);
    }
}

void Interface::setLanguage(QAction *action)
{
    Application::setLanguage(action->data().toString());
}

