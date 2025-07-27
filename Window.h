#pragma once
#include <QtWidgets>
#include "../components/Button.h"
#include <dwmapi.h>
#include <windowsx.h>
#include <windows.h>
#include <QWindow>

class Window : public QWidget {
    Q_OBJECT

public:
    explicit Window( QWidget *parent = nullptr);
    virtual ~Window() = default;

    void setDarkMode(bool value);
    void applyThemedIcons();
    void applyStyleSheet();
    void setupTitleBar();

    QHBoxLayout* _titleBarLayout() const;
    QWidget* _CustomTitleBarArea() const;
    QWidget* _titleBarArea() const;
    QWidget* _contentArea() const;

    HWND hwnd;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    bool event(QEvent *evt) override;

 private:
    bool determineNonClickableWidgetUnderMouse(QLayout *layout, int x, int y);
    void propagateActiveStateInCustomTitlebar(QLayout *layout, bool active_state);

    Button* windowButton();

    bool isDarkMode;
    bool showBorder = true;

    Button *closeBtn = nullptr;
    Button *minimizeBtn = nullptr;
    Button *maximizeBtn = nullptr;
    
    QWidget *titleBar = nullptr;
    QHBoxLayout *titleBarLayout = nullptr;

    QWidget *customTitleBar = nullptr;
    QHBoxLayout *customTitleBarLayout = nullptr;

    QWidget *contentArea = nullptr;
    QVBoxLayout *entireLayout = nullptr;
    
    const QString closeIconLight = ":/Icons/close_light.svg";
    const QString closeIconDark = ":/Icons/close_dark.svg";

    const QString minimizeIconLight = ":/Icons/minimize_light.svg";
    const QString minimizeIconDark = ":/Icons/minimize_dark.svg";

    const QString maximizeIconLight = ":/Icons/maximize_light.svg";
    const QString maximizeIconDark = ":/Icons/maximize_dark.svg";

    const QString restoreIconLight = ":/Icons/restore_light.svg";
    const QString restoreIconDark = ":/Icons/restore_dark.svg";

private slots:
    void onCloseClicked();
    void onMaximizeClicked();
    void onMinimizeClicked();
};


