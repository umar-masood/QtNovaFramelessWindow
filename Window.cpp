#include "Window.h"

Window::Window(QWidget *parent) : QWidget(nullptr), isDarkMode(false) {
    /* Window Properties */
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    /* Title Bar */
    _mainTitleBar = new QWidget(this);
    _mainTitleBar->setFixedHeight(36);
    _mainTitleBar->setContentsMargins(0, 0, 0, 0);
    _mainTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    _mainTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);    
    _mainTitleBar->installEventFilter(this);
    _mainTitleBar->setMouseTracking(true);
    
    /* Sub Title Bar */
    _subTitleBar = new QWidget;
    _subTitleBar->setFixedHeight(36);
    _subTitleBar->setContentsMargins(0, 0, 0, 0);
    _subTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    _subTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 
    _subTitleBar->setMouseTracking(true);

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
    _mainTitleBarLayout->addSpacing(6);

    /* Content Area */
    _contentArea = new QWidget(this);
    _contentArea->setContentsMargins(0, 0, 0, 0);
    _contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _contentArea->installEventFilter(this);
    _contentArea->setMouseTracking(true);
    setInteractiveTitleBarWidget(_contentArea);

    /* Entire Layout */
    entireLayout = new QVBoxLayout;
    entireLayout->setContentsMargins(4, 4, 4, 4);
    entireLayout->setSpacing(0);
    entireLayout->addWidget(_mainTitleBar, 0, Qt::AlignTop);
    entireLayout->addWidget(_contentArea, 0);
    setLayout(entireLayout);


    /* Apply Styles */
    setDarkMode(isDarkMode);
    setWindowControlsIcons();

    /* Installing Event Filter */
    installEventFilter(this);
}

Button * Window::createWindowButton() {
    Button *b = new Button;
    b->setSecondary(true);
    b->setIconSize(QSize(18, 18));
    b->setDisplayMode(Button::IconOnly);
    b->setCursor(Qt::PointingHandCursor);
    b->setBorderTransparent(true);
    b->setNormalBackgroundTransparent(true);
    b->setDisplayMode(Button::IconOnly);
    b->setFixedSize(QSize(26, 26));
    return b;
}

void Window::setDarkMode(bool value) {
    isDarkMode = value; 

    for (Button *b : {closeBtn, minimizeBtn, maximizeBtn}) 
        b->setDarkMode(isDarkMode);

    QString style = QString("background-color: %1;").arg(isDarkMode ? "#1F1F1F" : "#FFFFFF");
    _mainTitleBar->setStyleSheet(style);
    _contentArea->setStyleSheet(style);

    setWindowControlsIcons();
    update(); 
}

void Window::setInteractiveTitleBarWidget(QWidget *widget) {
    if (!widget) 
        return;
 
    if (interactiveWidgets.contains(widget)) 
        return;

    interactiveWidgets.insert(widget);
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

void Window::setWindowControlsIcons() {
    closeBtn->setIconPaths(closeIconLight, closeIconDark);
    minimizeBtn->setIconPaths(minimizeIconLight, minimizeIconDark);

    update();
    updateMaximizeIcon();
}

void Window::updateMaximizeIcon() {
    QRect screenRect = screen()->availableGeometry();

    if (geometry() == screenRect) 
        maximizeBtn->setIconPaths(restoreIconLight, restoreIconDark);
    else 
        maximizeBtn->setIconPaths(maximizeIconLight, maximizeIconDark);
}

void Window::onCloseClicked() { close(); }
void Window::onMinimizeClicked() { showMinimized(); }
void Window::onMaximizeClicked() {
    QRect screenRect = screen()->availableGeometry();

    if (geometry() == screenRect) {
        if (normalGeometry.isValid()) {
            setGeometry(normalGeometry);
            showBorder = true;
        } 
    } else {
        normalGeometry = geometry(); 
        setGeometry(screenRect);   
        showBorder = false;
    }

    update();
    updateMaximizeIcon();
}

void Window::updateCursorForRegion(ResizeRegion region) {
    switch (region) {
        case ResizeRegion::Top:
        case ResizeRegion::Bottom: setCursor(Qt::SizeVerCursor); break;
        case ResizeRegion::Left:
        case ResizeRegion::Right: setCursor(Qt::SizeHorCursor); break;
        case ResizeRegion::TopLeft:
        case ResizeRegion::BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        case ResizeRegion::TopRight:
        case ResizeRegion::BottomLeft: setCursor(Qt::SizeBDiagCursor); break;
        default: setCursor(Qt::ArrowCursor); break;
    }
}

bool Window::eventFilter(QObject *obj, QEvent *event) {
    if (obj == _mainTitleBar) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                onMaximizeClicked(); 
                return true;          
            }
        }
    }


    return QWidget::eventFilter(obj, event);
}

void Window::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QColor brushColor = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    QColor penColor   =  "#AFAFAF";

    painter.setBrush(brushColor);

    const int borderSize = 2;

    if (showBorder) {
        painter.setPen(QPen(penColor, 0.5));
        painter.drawRoundedRect(rect().adjusted(borderSize, borderSize, -borderSize, -borderSize), 6, 6);
    } else {
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect());
    }
}

Window::ResizeRegion Window::detectResizeRegion(const QPoint &pos) {
    QRect r = rect();

    bool onLeft = pos.x() <= resizeMargin;
    bool onRight = pos.x() >= r.width() - resizeMargin;
    bool onTop = pos.y() <= resizeMargin;
    bool onBottom = pos.y() >= r.height() - resizeMargin;

    if (onTop && onLeft) return ResizeRegion::TopLeft;
    if (onTop && onRight) return ResizeRegion::TopRight;
    if (onBottom && onLeft) return ResizeRegion::BottomLeft;
    if (onBottom && onRight) return ResizeRegion::BottomRight;
    if (onTop) return ResizeRegion::Top;
    if (onBottom) return ResizeRegion::Bottom;
    if (onLeft) return ResizeRegion::Left;
    if (onRight) return ResizeRegion::Right;

    return ResizeRegion::None;
}

void Window::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->position().toPoint();
    ResizeRegion region = detectResizeRegion(pos);

    // Always update the cursor
    updateCursorForRegion(region);

    // Store current region
    currentResizeRegion = region;

    // Call base class for normal processing
    QWidget::mouseMoveEvent(event);
}

void Window::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (currentResizeRegion != ResizeRegion::None) {
        if (windowHandle()) {
            Qt::Edges edges = Qt::Edges();
            switch (currentResizeRegion) {
                case ResizeRegion::Left: edges = Qt::LeftEdge; break;
                case ResizeRegion::Right: edges = Qt::RightEdge; break;
                case ResizeRegion::Top: edges = Qt::TopEdge; break;
                case ResizeRegion::Bottom: edges = Qt::BottomEdge; break;
                case ResizeRegion::TopLeft: edges = Qt::TopEdge | Qt::LeftEdge; break;
                case ResizeRegion::TopRight: edges = Qt::TopEdge | Qt::RightEdge; break;
                case ResizeRegion::BottomLeft: edges = Qt::BottomEdge | Qt::LeftEdge; break;
                case ResizeRegion::BottomRight: edges = Qt::BottomEdge | Qt::RightEdge; break;
                default: break;
            }

            windowHandle()->startSystemResize(edges);
        }

    } else if (_mainTitleBar->geometry().contains(event->pos()) && 
                !isPointInsideInteractiveTitleBarWidgets(event->position().x(), event->position().y()))
    {
        if (windowHandle())
            windowHandle()->startSystemMove();
    }

    QWidget::mousePressEvent(event);
}

void Window::leaveEvent(QEvent *event) {
    currentResizeRegion = ResizeRegion::None;
    setCursor(Qt::ArrowCursor);

    QWidget::leaveEvent(event);
}

void Window::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    showBorder = true;

    update();
    updateMaximizeIcon();
}

void Window::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange)
        updateMaximizeIcon();

    QWidget::changeEvent(event);
}

QWidget* Window::titleBar() const { return _subTitleBar; }
QWidget* Window::contentArea() const { return _contentArea; }
