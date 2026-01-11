#include "Window.h"

Window::Window(QWidget *parent) : QWidget(nullptr), isDarkMode(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    setupWindow();
}

void Window::setDarkMode(bool value) {
    isDarkMode = value; 
    for (Button *b : {closeBtn, minimizeBtn, maximizeBtn}) 
        b->setDarkMode(isDarkMode);

    applyThemedIcons();
    update(); 
}

void Window::setInteractiveTitleBarWidget(QWidget *widget) {
    if (!widget) 
        return;
 
    if (interactiveWidgets.contains(widget)) 
        return;

    interactiveWidgets.insert(widget);
}

void Window::applyThemedIcons() {
    closeBtn->setUnicodeIcon("\uE8BB", 10);
    minimizeBtn->setUnicodeIcon("\uE921", 10);

    if (::IsZoomed(hwnd))
        maximizeBtn->setUnicodeIcon("\uE923", 10); 
    else
        maximizeBtn->setUnicodeIcon("\uE922", 10);

    update();
}

void Window::applyStyleSheet() {
    _mainTitleBar->setStyleSheet("background-color: transparent;");
    _contentArea->setStyleSheet("background-color: transparent;");
}

void Window::applyDWMEffects() {
    /* Window Style Applying for Resizing*/
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
}

void Window::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QColor BG = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), BG);
}

Button * Window::createWindowButton() {
    Button *b = new Button;
    b->setSecondary(true);
    b->setIconSize(QSize(16,16));
    b->setDisplayMode(Button::IconOnly);
    b->setFixedSize(QSize(30, 30));
    return b;
}

void Window::setupWindow() {
    /* Title Bar */
    _mainTitleBar = new QWidget(this);
    _mainTitleBar->setFixedHeight(36);
    _mainTitleBar->setContentsMargins(0, 0, 0, 0);
    _mainTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    _mainTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    /* Sub Title Bar */
    _subTitleBar = new QWidget;
    _subTitleBar->setFixedHeight(36);
    _subTitleBar->setContentsMargins(0, 0, 0, 0);
    _subTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    _subTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 
    
    /* Title Bar Main Layout*/
    _mainTitleBarLayout = new QHBoxLayout(_mainTitleBar);
    _mainTitleBarLayout->setContentsMargins(0, 0, 0, 0);
    _mainTitleBarLayout->setSpacing(0);
    _mainTitleBarLayout->addWidget(_subTitleBar);

    /* Window Controls*/
    closeBtn = createWindowButton();
    setInteractiveTitleBarWidget(closeBtn);
    connect(closeBtn, &Button::clicked, this, &Window::onCloseClicked);
    
    minimizeBtn = createWindowButton();
    setInteractiveTitleBarWidget(minimizeBtn);
    connect(minimizeBtn, &Button::clicked, this, &Window::onMinimizeClicked);
    
    maximizeBtn = createWindowButton();
    setInteractiveTitleBarWidget(maximizeBtn);
    connect(maximizeBtn, &Button::clicked, this, &Window::onMaximizeClicked);

    _mainTitleBarLayout->addWidget(minimizeBtn, 0, Qt::AlignRight);
    _mainTitleBarLayout->addSpacing(4);
    _mainTitleBarLayout->addWidget(maximizeBtn, 0, Qt::AlignRight);
    _mainTitleBarLayout->addSpacing(4);
    _mainTitleBarLayout->addWidget(closeBtn, 0, Qt::AlignRight);
    _mainTitleBarLayout->addSpacing(4);

    /* Window Handle ID */
    hwnd = reinterpret_cast<HWND>(winId());
    applyDWMEffects();

   /* Content Area */
   _contentArea = new QWidget(this);
   _contentArea->setContentsMargins(0, 0, 0, 0);
   _contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   /* Entire Layout */
   entireLayout = new QVBoxLayout;
   entireLayout->setContentsMargins(0, 0, 0, 0);
   entireLayout->setSpacing(0);
   entireLayout->addWidget(_mainTitleBar, 0, Qt::AlignTop);
   entireLayout->addWidget(_contentArea, 0);
   setLayout(entireLayout);

   /* Apply Styles */
   applyStyleSheet();
   applyThemedIcons();
}

void Window::onCloseClicked() { ::SendMessage(hwnd, WM_CLOSE, 0, 0); }
void Window::onMinimizeClicked() { ::ShowWindow(hwnd, SW_MINIMIZE); }
void Window::onMaximizeClicked() {
    if (::IsZoomed(hwnd)) 
        ::ShowWindow(hwnd, SW_RESTORE);
    else 
        ::ShowWindow(hwnd, SW_MAXIMIZE);
    
    applyThemedIcons();
}

bool Window::isPointInsideInteractiveTitleBarWidgets(int x, int y) {
    QPoint globalPos(x, y);

    for (QWidget *widget : interactiveWidgets) {
        if (!widget)
            continue;

        QPoint localPos = widget->mapFromGlobal(globalPos);
        if (widget->rect().contains(localPos)) 
            return true;
    }
    return false;
}

bool Window::nativeEvent(const QByteArray &, void *message, qintptr *result) {
    MSG *msg = static_cast<MSG*>(message);

    switch (msg->message) {
    case WM_NCCALCSIZE:
        return true;

    case WM_NCHITTEST: {
        RECT r;
        GetWindowRect(msg->hwnd, &r);

        int x = GET_X_LPARAM(msg->lParam);
        int y = GET_Y_LPARAM(msg->lParam);
        const int border = 8;

        if (x < r.left + border && y < r.top + border) {
            *result = HTTOPLEFT; return true;
        }
        if (x > r.right - border && y < r.top + border) {
            *result = HTTOPRIGHT; return true;
        }
        if (x < r.left + border && y > r.bottom - border) {
            *result = HTBOTTOMLEFT; return true;
        }
        if (x > r.right - border && y > r.bottom - border) {
            *result = HTBOTTOMRIGHT; return true;
        }
        if (x < r.left + border) {
            *result = HTLEFT; return true;
        }
        if (x > r.right - border) {
            *result = HTRIGHT; return true;
        }
        if (y < r.top + border) {
            *result = HTTOP; return true;
        }
        if (y > r.bottom - border) {
            *result = HTBOTTOM; return true;
        }
        if (!isPointInsideInteractiveTitleBarWidgets(x, y)) {
            *result = HTCAPTION; return true;
        }

        return false;
    }

    case WM_SIZE:
        applyThemedIcons();
        break;
    }

    return false;
}

QWidget* Window::titleBar() const { return _subTitleBar; }
QWidget* Window::contentArea() const { return _contentArea; }
