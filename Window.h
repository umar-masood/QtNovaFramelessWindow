#pragma once

#include <QWindow>
#include <QHBoxLayout>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QResizeEvent>
#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <memory>

// Window Button 
// This class is not a part of public API
struct WinButtonPrivate {
    public:
    bool darkMode = false;
    bool hovered = false;
    bool pressed = false;

    QPixmap iconLight, iconDark;
};

class WinButton : public QPushButton {
    Q_OBJECT
    
    public:
    WinButton(QWidget *parent = nullptr);

    void setIconPaths(const QString &lightPath, const QString &darkPath);

    void setDarkMode(bool dark);
    bool darkMode() const;

    protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

    private:
    std::unique_ptr<WinButtonPrivate> m_d;
};

// Window 
// This class is not a part of public API
struct WindowPrivate {
    bool darkMode = false;

    // Border
    QColor borderColor = QColor("#D0D0D0");

    // Maximize / Restore / Normal
    bool normalWindow = false;
    QRect normalGeometry; 

    // Interactive
    bool interactionBlocked = false;
    QSet<QWidget *> interactiveWidgets;

    // Dragging
    bool dragging = false;
    QPoint dragStartGlobalPos, dragStartWindowPos;

    // Resize
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

    ResizeRegion currentResizeRegion = ResizeRegion::None;
    const int resizeMargin = 6;

    // Buttons
    WinButton *closeBtn = nullptr;
    WinButton *minimizeBtn = nullptr;
    WinButton *maximizeBtn = nullptr;

    // Main Title Bar
    QWidget *mainTitleBar = nullptr;
    QHBoxLayout *mainTitleBarLayout = nullptr;

    // Sub Title Bar (Custom Title Bar)
    QWidget *subTitleBar = nullptr;
    QHBoxLayout *subTitleBarLayout = nullptr;

    // Content Area
    QWidget *contentArea = nullptr;

    // Entire Layout
    QVBoxLayout *entireLayout = nullptr;
};

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

    void setDarkMode(bool dark);
    bool darkMode() const;

    void setInteractiveTitleBarWidget(QWidget *widget);
    void setInteractionBlocked(bool enable);
    void setBorderColor(const QColor &color);

    QWidget* titleBar() const;
    QWidget* contentArea() const;

    protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event);

    private:
    std::unique_ptr<WindowPrivate> m_d = nullptr;

    void setWindowButtonsIcons();
    void updateMaximizeIcon();
    WinButton* createWindowButton();

    void updateCursorForRegion(WindowPrivate::ResizeRegion region);
    WindowPrivate::ResizeRegion detectResizeRegion(const QPoint &pos);
    
    bool isPointInsideInteractiveTitleBarWidgets(int x, int y);

    private slots:
    void maximizedClicked();
};