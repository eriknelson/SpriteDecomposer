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
#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QtXml>
#include <QCloseEvent>

#include "aboutdialog.h"
#include "animation.h"
#include "workarea.h"
#include "cut.h"
#include "mark.h"

#include <QDebug>

namespace Ui {
class Interface;
}

class Interface : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor / Destructor
    explicit Interface(QWidget *parent = 0);
    ~Interface();

protected:
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent* event);

public slots:
    // Tools
    void tool_normal();
    void tool_zoomIn();
    void tool_zoomOut();
    void tool_cutRect();
    void tool_cutGrid();
    void tool_mark();
    void tool_picker();
    void tool_cutAuto();

    // Menu
    void menu_open();
    void menu_save();
    void menu_saveAs();
    void menu_quit();
    void menu_about();

    // configurePanel
    void configurePanelChanged(int index);

    // configureCut
    void addCutRect();
    void addCutGrid();
    void cut_GeometryChange();

    // configureMark
    void addMark();
    void mark_OrientationChange(int index);
    void mark_PositionChange();

    // configurePicker
    void picker_OpenColorPicker();
    void picker_SetMaskColor();

    // Viewer
    void viewer_zoomIn();
    void viewer_zoomOut();

    // Animation
    void animation_add();
    void animation_remove();
    void animation_changeTitle(QString title);

    // Workarea
    void pickerColor(QRgb color);
    void itemSelected(Cut* selection);
    void multipleSelection();

    // Animation
    void frameCountUpdated(int frameCount);

private:
    Ui::Interface *ui;              // Interface
    QList<Animation*> m_animations; // Animations List
    WorkArea* m_workarea;           // WorkArea
    QAction *m_currentToolSelected; // Tool selected
    qreal m_zoomRatio;              // Zoom Scene ratio
    qreal m_zoomViewRatio;          // Zoom View ratio
    QString m_imageFilename;        // Background image (sprite sheet) filename
    QString m_xmlFilename;         // Current XML file
    bool m_workModified;
    bool m_selectAnimation;
    AboutDialog *m_aboutDialog;

private:
    int saveModification();
    void saveAll(const QString &filename);
    void openXMl(const QString &filename);
    void openImage(const QString &filename, bool createDefaultAnimation = true);
    Animation* addAnimation(const QString &title);
    void removeAnimation(int index);
    void showCutRectInfo(Cut* cut);
    void showCutGridInfo(Cut* cut);
    void closeAll();

    // Translation
    void fillLanguages();
    void retranslate();
    QMenu* m_languages;

private slots:
    void selectAnimation(int index);
    void setLanguage(QAction* action);
};

#endif // INTERFACE_H
