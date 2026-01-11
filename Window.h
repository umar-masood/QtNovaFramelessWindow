#pragma once
#include <QWidget>
#include "components/Button.h"
#include <dwmapi.h>
#include <windowsx.h>
#include <windows.h>
#include <QWindow>
#include <QHBoxLayout>
#include <QObject>
#include <QPoint>
#include <QSet>

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window( QWidget *parent = nullptr);
    virtual ~Window() = default;

    void setDarkMode(bool value);
    void setInteractiveTitleBarWidget(QWidget *widget);

    QWidget* titleBar() const;
    QWidget* contentArea() const;

    protected:
    void paintEvent(QPaintEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    private:

    // Current Window Handle (ID)
    HWND hwnd;

    // Set Window Controls Icons
    void applyThemedIcons();

    // Set Window Widgets theme
    void applyStyleSheet();

    // Apply DWM Effects such as rounded corners, shadow etc.
    void applyDWMEffects();

    // Setup whole window
    void setupWindow();

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


