#include "Window.h"

Window::Window(QWidget *parent) : QWidget(nullptr), isDarkMode(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    setupTitleBar();
}

void Window::setDarkMode(bool value) {
    isDarkMode = value; 
    for (Button *b : {closeBtn, minimizeBtn, maximizeBtn})  b->setDarkMode(isDarkMode);
    applyThemedIcons();
    update(); 
}

void Window::applyThemedIcons() {
    closeBtn->setUnicodeIcon("\uE8BB", 10);
    minimizeBtn->setUnicodeIcon("\uE921", 10);
    bool nativeMaximized = ::IsZoomed(hwnd); 
    if (nativeMaximized)
        maximizeBtn->setUnicodeIcon("\uE923", 10); 
    else
        maximizeBtn->setUnicodeIcon("\uE922", 10);

    update();
}


void Window::applyStyleSheet() {
    titleBar->setStyleSheet("background-color: transparent;");
    contentArea->setStyleSheet("background-color: transparent;");
    customTitleBar->setAttribute(Qt::WA_TranslucentBackground, true);
}

void Window::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QColor BG = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), BG);
}

Button * Window::windowButton()
{
    Button *b = new Button;
    b->setSecondary(true);
    b->setIconSize(QSize(16,16));
    b->setDisplayMode(Button::ToolButton);
    b->setSize(QSize(30, 30));
    return b;
}

void Window::setupTitleBar() {
    /* Title Bar */
    titleBar = new QWidget(this);
    titleBar->setFixedHeight(36);
    titleBar->setContentsMargins(0, 0, 0, 0);
    titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    /* Title Bar Main Layout*/
    titleBarLayout = new QHBoxLayout(titleBar);
    titleBarLayout->setContentsMargins(0, 0, 0, 0);
    titleBarLayout->setSpacing(0);

    /* Custom Title Bar*/
    customTitleBar = new QWidget(this);
    customTitleBar->setContentsMargins(0, 0, 0, 0);
    customTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    titleBarLayout->addWidget(customTitleBar);

    /* Custom Title Bar Layout*/
    customTitleBarLayout = new QHBoxLayout(customTitleBar);
    customTitleBarLayout->setSpacing(0);
    customTitleBarLayout->setContentsMargins(5, 0, 0, 0);
    customTitleBarLayout->setAlignment(Qt::AlignLeft);

    /* Window Controls*/
    closeBtn = windowButton();
    connect(closeBtn, &Button::clicked, this, &Window::onCloseClicked);
    minimizeBtn = windowButton();
    connect(minimizeBtn, &Button::clicked, this, &Window::onMinimizeClicked);
    maximizeBtn = windowButton();
    connect(maximizeBtn, &Button::clicked, this, &Window::onMaximizeClicked);

    titleBarLayout->addSpacing(5);
    titleBarLayout->addWidget(minimizeBtn, 0, Qt::AlignRight);
    titleBarLayout->addSpacing(5);
    titleBarLayout->addWidget(maximizeBtn, 0, Qt::AlignRight);
    titleBarLayout->addSpacing(5);
    titleBarLayout->addWidget(closeBtn, 0, Qt::AlignRight);
    titleBarLayout->addSpacing(5);

    /* Window Style Applying for Resizing*/
    hwnd = reinterpret_cast<HWND>(winId());

    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style |= WS_THICKFRAME | WS_CAPTION;
    SetWindowLong(hwnd, GWL_STYLE, style);

    const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
    enum DWM_WINDOW_CORNER_PREFERENCE {
        DWMWCP_DEFAULT      = 0,
        DWMWCP_DONOTROUND   = 1,
        DWMWCP_ROUND        = 2,
        DWMWCP_ROUNDSMALL   = 3
    };

    DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND; 
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    
    // Force Windows to recalculate the frame based on new styles
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

   /* Content Area */
   contentArea = new QWidget(this);
   contentArea->setContentsMargins(0, 0, 0, 0);
   contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   /* Entire Layout */
   entireLayout = new QVBoxLayout;
   entireLayout->setContentsMargins(0, 0, 0, 0);
   entireLayout->setSpacing(0);
   entireLayout->addWidget(titleBar, 0, Qt::AlignTop);
   entireLayout->addWidget(contentArea, 0);
   setLayout(entireLayout);

   /* Apply Styles */
   applyStyleSheet();
   applyThemedIcons();
}

void Window::onCloseClicked() {
    ::SendMessage(hwnd, WM_CLOSE, 0, 0);
}

void Window::onMinimizeClicked() {
    ::ShowWindow(hwnd, SW_MINIMIZE);
}

void Window::onMaximizeClicked() {
    if (::IsZoomed(hwnd)) 
        ::ShowWindow(hwnd, SW_RESTORE);
    else 
        ::ShowWindow(hwnd, SW_MAXIMIZE);
    
    applyThemedIcons();
}

bool Window::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = (MSG *)message;
    switch (msg->message) {
    case WM_NCCALCSIZE:
        return true; break;

    case WM_NCHITTEST: {
        RECT winrect;
        GetWindowRect(msg->hwnd, &winrect);
        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);
        long local_x = x - winrect.left;
        long local_y = y - winrect.top;
        int resize_border_Width = 8;

        if (x >= winrect.left && x < winrect.left + resize_border_Width && y < winrect.bottom && y >= winrect.bottom - resize_border_Width) {
            *result = HTBOTTOMLEFT; return true;
        }
        if (x < winrect.right && x >= winrect.right - resize_border_Width && y < winrect.bottom && y >= winrect.bottom - resize_border_Width) {
            *result = HTBOTTOMRIGHT; return true;
        }
        if (x >= winrect.left && x < winrect.left + resize_border_Width && y >= winrect.top && y < winrect.top + resize_border_Width) {
            *result = HTTOPLEFT; return true;
        }
        if (x < winrect.right && x >= winrect.right - resize_border_Width && y >= winrect.top && y < winrect.top + resize_border_Width) {
            *result = HTTOPRIGHT; return true;
        }
        if (x >= winrect.left && x < winrect.left + resize_border_Width) {
            *result = HTLEFT; return true;
        }
        if (x < winrect.right && x >= winrect.right - resize_border_Width) {
            *result = HTRIGHT; return true;
        }
        if (y < winrect.bottom && y >= winrect.bottom - resize_border_Width) {
            *result = HTBOTTOM;
            return true;
        }
        if (y >= winrect.top && y < winrect.top + resize_border_Width) {
            *result = HTTOP; return true;
        }
        if (determineNonClickableWidgetUnderMouse(customTitleBarLayout, local_x, local_y)) {
            *result = HTCAPTION; return true;
        }

        *result = HTTRANSPARENT;
        break;
    }

    case WM_SIZE: {
    if (maximizeBtn) {
        maximizeBtn->setChecked(IsZoomed(hwnd));
        applyThemedIcons(); 
    }
    break;
    }
    
    default:
        break;
    }
    return false;
}

bool Window::determineNonClickableWidgetUnderMouse(QLayout *layout, int x, int y) {
    if (!layout->count() && layout->geometry().contains(x, y)) return true;

    for (int i = 0; i < layout->count(); ++i) {
        auto item = layout->itemAt(i)->widget();
        if (item) {
            if (item->geometry().contains(x, y))
                return !item->property("clickable widget").toBool();
        } else {
            auto child_layout = layout->itemAt(i)->layout();
            if (child_layout && child_layout->geometry().contains(x, y))
                return determineNonClickableWidgetUnderMouse(child_layout, x, y);
        }
    }
    return false;
}

bool Window::event(QEvent *evt) {
    switch (evt->type()) {
    case QEvent::WindowActivate:     propagateActiveStateInCustomTitlebar(customTitleBarLayout, true);   break;
    case QEvent::WindowDeactivate:   propagateActiveStateInCustomTitlebar(customTitleBarLayout, false);  break;
    default:  break; 
    }
    return QWidget::event(evt);
}

void Window::propagateActiveStateInCustomTitlebar(QLayout *layout, bool active_state) {
    for (int i = 0; i < layout->count(); ++i) {
        auto item = layout->itemAt(i)->widget();
        if (item) {
            item->setProperty("active", active_state);
            item->setStyleSheet(item->styleSheet());
        } else {
            auto child_layout = layout->itemAt(i)->layout();
            if (child_layout)
                propagateActiveStateInCustomTitlebar(child_layout, active_state);
        }
    }
}

QHBoxLayout* Window::_titleBarLayout() const { return customTitleBarLayout; }

QWidget* Window::_CustomTitleBarArea() const { return customTitleBar; }

QWidget* Window::_titleBarArea() const { return titleBar; }

QWidget* Window::_contentArea() const { return contentArea; }
