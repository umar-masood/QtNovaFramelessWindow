#pragma once

#include "../../components/Button.h"

#include <QWindow>
#include <QHBoxLayout>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QResizeEvent>
#include <QWidget>

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window( QWidget *parent = nullptr);
    virtual ~Window() = default;

    enum class ResizeRegion {
        None,
        Left,
        Top,
        Right,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };


    void setDarkMode(bool value);
    void setInteractiveTitleBarWidget(QWidget *widget);

    QWidget* titleBar() const;
    QWidget* contentArea() const;

    protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event);

    private:
    // Window Border
    bool showBorder = false;
    
    // Maximize / Restore
    QRect normalGeometry; 

   // Icons
    QString closeIconLight      = ":/icons/win-close-light.svg"; 
    QString closeIconDark       = ":/icons/win-close-dark.svg"; 
    QString minimizeIconLight   = ":/icons/win-minimize-light.svg"; 
    QString minimizeIconDark    = ":/icons/win-minimize-dark.svg";
    QString maximizeIconLight   = ":/icons/win-maximize-light.svg"; 
    QString maximizeIconDark    = ":/icons/win-maximize-dark.svg";
    QString restoreIconLight    = ":/icons/win-restore-light.svg";
    QString restoreIconDark     = ":/icons/win-restore-dark.svg";

    // Set Window Controls Icons
    void setWindowControlsIcons();
    void updateMaximizeIcon();

    // Window Resizing 
    void updateCursorForRegion(ResizeRegion region);
    ResizeRegion detectResizeRegion(const QPoint &pos);
    
    ResizeRegion currentResizeRegion = ResizeRegion::None;
    const int resizeMargin = 4;

    // Check weather the current coordinates lie inside titlebar interactive widgets
    bool isPointInsideInteractiveTitleBarWidgets(int x, int y);

    // Helper function to create window control 
    Button* createWindowButton();

    // Set to store unique interactive widgets
    QSet<QWidget *> interactiveWidgets;

    // Theme Mode flag
    bool isDarkMode;

    // Buttons
    Button *closeBtn = nullptr;
    Button *minimizeBtn = nullptr;
    Button *maximizeBtn = nullptr;
    
    // Main Title Bar
    QWidget *_mainTitleBar = nullptr;
    QHBoxLayout *_mainTitleBarLayout = nullptr;

    // Sub Title Bar (Custom Title Bar)
    QWidget *_subTitleBar = nullptr;
    QHBoxLayout *_subTitleBarLayout = nullptr;

    // Main Content Area
    QWidget *_contentArea = nullptr;
    QVBoxLayout *entireLayout = nullptr;

    private slots:
    void onCloseClicked();
    void onMaximizeClicked();
    void onMinimizeClicked();
};


