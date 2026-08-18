#include "Window.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFile>

using ResizeRegion = WindowPrivate::ResizeRegion;

Window::Window(QWidget *parent) : QWidget(parent), m_d(std::make_unique<WindowPrivate>()) {
    /* Window Properties */
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    /* Title Bar */
    m_d->mainTitleBar = new QWidget(this);
    m_d->mainTitleBar->setFixedHeight(36);
    m_d->mainTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    m_d->mainTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);    
    
    /* Sub Title Bar */
    m_d->subTitleBar = new QWidget;
    m_d->subTitleBar->setFixedHeight(36);
    m_d->subTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    m_d->subTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 

    /* Title Bar Main Layout*/
    m_d->mainTitleBarLayout = new QHBoxLayout(m_d->mainTitleBar);
    m_d->mainTitleBarLayout->setSpacing(0);
    m_d->mainTitleBarLayout->setContentsMargins(0, 0, 0, 0);
    m_d->mainTitleBarLayout->addWidget(m_d->subTitleBar);

    /* Window Buttons */
    m_d->closeBtn = createWindowButton();
    setInteractiveTitleBarWidget(m_d->closeBtn);
    connect(m_d->closeBtn, &WinButton::clicked, this, &QWidget::close);
    
    m_d->minimizeBtn = createWindowButton();
    setInteractiveTitleBarWidget(m_d->minimizeBtn);
    connect(m_d->minimizeBtn, &WinButton::clicked, this, &QWidget::showMinimized);
    
    m_d->maximizeBtn = createWindowButton();
    setInteractiveTitleBarWidget(m_d->maximizeBtn);
    connect(m_d->maximizeBtn, &WinButton::clicked, this, &Window::maximizedClicked);

    m_d->mainTitleBarLayout->addWidget(m_d->minimizeBtn, 0, Qt::AlignRight);
    m_d->mainTitleBarLayout->addSpacing(4);
    m_d->mainTitleBarLayout->addWidget(m_d->maximizeBtn, 0, Qt::AlignRight);
    m_d->mainTitleBarLayout->addSpacing(4);
    m_d->mainTitleBarLayout->addWidget(m_d->closeBtn, 0, Qt::AlignRight);
    m_d->mainTitleBarLayout->addSpacing(6);

    /* Content Area */
    m_d->contentArea = new QWidget(this);
    m_d->contentArea->setContentsMargins(0, 0, 0, 0);
    m_d->contentArea->setAttribute(Qt::WA_TranslucentBackground);
    m_d->contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* Entire Layout */
    m_d->entireLayout = new QVBoxLayout;
    m_d->entireLayout->setContentsMargins(4, 4, 4, 4);
    m_d->entireLayout->setSpacing(0);
    m_d->entireLayout->addWidget(m_d->mainTitleBar, 0, Qt::AlignTop);
    m_d->entireLayout->addWidget(m_d->contentArea, 0);
    setLayout(m_d->entireLayout);

    /* Apply Styles */
    setDarkMode(m_d->darkMode);

    /* Installing Event Filter */
    qApp->installEventFilter(this);
}

WinButton* Window::createWindowButton() {
    auto *b = new WinButton;
    b->setIconSize(QSize(18, 18));
    b->setCursor(Qt::PointingHandCursor);
    b->setFixedSize(QSize(26, 26));
    return b;
}

void Window::setDarkMode(bool dark) {
    m_d->darkMode = dark; 

    for (WinButton *b : {m_d->closeBtn, m_d->minimizeBtn, m_d->maximizeBtn}) 
        b->setDarkMode(dark);

    QString style = QString("background-color: %1;").arg(dark ? "#1F1F1F" : "#FFFFFF");
    m_d->mainTitleBar->setStyleSheet(style);
    m_d->contentArea->setStyleSheet(style);

    setWindowButtonsIcons();
    update(); 
}

bool Window::darkMode() const {
    return m_d->darkMode;
}

void Window::setInteractiveTitleBarWidget(QWidget *widget) {
    if (widget) 
        m_d->interactiveWidgets.insert(widget);
}

bool Window::isPointInsideInteractiveTitleBarWidgets(int x, int y) {
    const QPoint globalPos(x, y);

    for (QWidget *widget : m_d->interactiveWidgets) {
        if (!widget)
            continue;

        const QPoint localPos = widget->mapFromGlobal(globalPos);

        if (widget->rect().contains(localPos)) 
            return true;
    }

    return false;
}

void Window::setInteractionBlocked(bool enable) {
    m_d->interactionBlocked = enable;

    if (m_d->interactionBlocked)
        setCursor(Qt::ArrowCursor);
}

void Window::setBorderColor(const QColor &color) {
    m_d->borderColor = color;
    update();
}

void Window::setBackgroundColor(const QColor &light, const QColor &dark) {
    m_d->backgroundColorLight = light;
    m_d->backgroundColorDark = dark;
    update();
}

void Window::setWindowButtonsIcons() {
    m_d->closeBtn->setIconPaths(":/icons/win-close-light.svg", ":/icons/win-close-dark.svg");
    m_d->minimizeBtn->setIconPaths(":/icons/win-minimize-light.svg", ":/icons/win-minimize-dark.svg");

    update();
    updateMaximizeIcon();
}

void Window::updateMaximizeIcon() {
    const QRect screenRect = screen()->availableGeometry();

    if (geometry() == screenRect) 
        m_d->maximizeBtn->setIconPaths(":/icons/win-restore-light.svg", ":/icons/win-restore-dark.svg");
    else 
        m_d->maximizeBtn->setIconPaths(":/icons/win-maximize-light.svg", ":/icons/win-maximize-dark.svg");
}

void Window::maximizedClicked() {
    QRect screenRect = screen()->availableGeometry();

    if (geometry() == screenRect) {
        if (m_d->normalGeometry.isValid()) {
            setGeometry(m_d->normalGeometry);
            m_d->normalWindow = true;
        } 
        
    } else {
        m_d->normalGeometry = geometry(); 
        setGeometry(screenRect);
        m_d->normalWindow = false;
    }

    update();
    updateMaximizeIcon();
}

void Window::updateCursorForRegion(ResizeRegion region) {
    switch (region) {
        case ResizeRegion::Top:
        case ResizeRegion::Bottom:        setCursor(Qt::SizeVerCursor);   break;
        case ResizeRegion::Left:
        case ResizeRegion::Right:         setCursor(Qt::SizeHorCursor);   break;
        case ResizeRegion::TopLeft:
        case ResizeRegion::BottomRight:   setCursor(Qt::SizeFDiagCursor); break;
        case ResizeRegion::TopRight:
        case ResizeRegion::BottomLeft:    setCursor(Qt::SizeBDiagCursor); break;
        default:                                         unsetCursor();                  break;
    }
}

bool Window::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint windowPos = mapFromGlobal(mouseEvent->globalPosition().toPoint());

        if (!isActiveWindow()) 
            return QWidget::eventFilter(obj, event);

        if (m_d->normalWindow && !m_d->interactionBlocked && rect().contains(windowPos)) {
            ResizeRegion region = detectResizeRegion(windowPos);
            updateCursorForRegion(region);
            m_d->currentResizeRegion = region;

        } else if (m_d->normalWindow) {
            unsetCursor();
            m_d->currentResizeRegion = ResizeRegion::None;
        }
    }

    if (event->type() == QEvent::MouseButtonDblClick && obj == m_d->mainTitleBar) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            maximizedClicked();
            return true;
        }
    }

    if (m_d->interactionBlocked) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            case QEvent::MouseButtonDblClick:
            case QEvent::Wheel:
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
                return true; // Fully block

            default:
                break;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void Window::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setBrush(darkMode() ? m_d->backgroundColorDark : m_d->backgroundColorLight);

    if (m_d->normalWindow) {
        painter.setPen(QPen(m_d->borderColor, 0.5));
        painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 6, 6);
    } else {
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect());
    }
}

ResizeRegion Window::detectResizeRegion(const QPoint &pos) {
    const QRect r = rect();

    bool onLeft     = pos.x() <= m_d->resizeMargin;
    bool onRight    = pos.x() >= r.width() - m_d->resizeMargin;
    bool onTop      = pos.y() <= m_d->resizeMargin;
    bool onBottom   = pos.y() >= r.height() - m_d->resizeMargin;

    if (onTop && onLeft)        return ResizeRegion::TopLeft;
    if (onTop && onRight)       return ResizeRegion::TopRight;
    if (onBottom && onLeft)     return ResizeRegion::BottomLeft;
    if (onBottom && onRight)    return ResizeRegion::BottomRight;
    if (onTop)                  return ResizeRegion::Top;
    if (onBottom)               return ResizeRegion::Bottom;
    if (onLeft)                 return ResizeRegion::Left;
    if (onRight)                return ResizeRegion::Right;

    return ResizeRegion::None;
}

void Window::mouseMoveEvent(QMouseEvent *event) {
    if (m_d->dragging) {
        QPoint delta = event->globalPosition().toPoint() - m_d->dragStartGlobalPos;
        move(m_d->dragStartWindowPos + delta);
        return; 
    }

    QWidget::mouseMoveEvent(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_d->dragging)
        m_d->dragging = false;
    
    QWidget::mouseReleaseEvent(event);
}

void Window::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    // Resize
    if (m_d->normalWindow && m_d->currentResizeRegion != ResizeRegion::None) {
        if (windowHandle()) {
            Qt::Edges edges;

            switch (m_d->currentResizeRegion) {
                case ResizeRegion::Left:        edges = Qt::LeftEdge; break;
                case ResizeRegion::Right:       edges = Qt::RightEdge; break;
                case ResizeRegion::Top:         edges = Qt::TopEdge; break;
                case ResizeRegion::Bottom:      edges = Qt::BottomEdge; break;
                case ResizeRegion::TopLeft:     edges = Qt::TopEdge | Qt::LeftEdge; break;
                case ResizeRegion::TopRight:    edges = Qt::TopEdge | Qt::RightEdge; break;
                case ResizeRegion::BottomLeft:  edges = Qt::BottomEdge | Qt::LeftEdge; break;
                case ResizeRegion::BottomRight: edges = Qt::BottomEdge | Qt::RightEdge; break;
                default: break;
            }

            windowHandle()->startSystemResize(edges);
        }

        QWidget::mousePressEvent(event);
        return;
    }

    // Dragging
    if (m_d->mainTitleBar->geometry().contains(event->pos()) &&
        !isPointInsideInteractiveTitleBarWidgets(event->position().x(), event->position().y()))
    {
        m_d->dragging = true;
        m_d->dragStartGlobalPos = event->globalPosition().toPoint();
        m_d->dragStartWindowPos = frameGeometry().topLeft();
    }

    QWidget::mousePressEvent(event);
}

void Window::leaveEvent(QEvent *event) {
    m_d->currentResizeRegion = ResizeRegion::None;
    unsetCursor();

    QWidget::leaveEvent(event);
}

void Window::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    m_d->normalWindow = true;

    update();
    updateMaximizeIcon();
}

void Window::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange)
        updateMaximizeIcon();

    QWidget::changeEvent(event);
}

void Window::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    QPoint localPos = mapFromGlobal(QCursor::pos());

    if (rect().contains(localPos) && m_d->normalWindow) {
        ResizeRegion region = detectResizeRegion(localPos);
        updateCursorForRegion(region);
        m_d->currentResizeRegion = region;
    } else {
        unsetCursor();
        m_d->currentResizeRegion = ResizeRegion::None;
    }
}

Window::~Window() {
    qApp->removeEventFilter(this);
}

QWidget* Window::titleBar() const { 
    return m_d->subTitleBar; 
}

QWidget* Window::contentArea() const { 
    return m_d->contentArea; 
}

// -------------- Window Button --------------
WinButton::WinButton(QWidget *parent) : QPushButton(parent) {
    m_d = std::make_unique<WinButtonPrivate>();
}

void WinButton::setDarkMode(bool dark) {
    if (m_d->darkMode == dark)
        return;
    
    m_d->darkMode = dark;
    update();
}

bool WinButton::darkMode() const {
    return m_d->darkMode;
}

void WinButton::setIconPaths(const QString &lightPath, const QString &darkPath) {
    if (lightPath.isEmpty() || darkPath.isEmpty())
        return;
    
    if (!QFile::exists(lightPath) || !QFile::exists(darkPath))
        return;

    m_d->iconLight = QPixmap(lightPath).scaled(iconSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    m_d->iconDark = QPixmap(darkPath).scaled(iconSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    update();
}

void WinButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // No Border
    painter.setPen(Qt::NoPen);

    // Background
    QColor brushColor;
    if (m_d->pressed)
        brushColor = darkMode() ? QColor("#242424") : QColor("#FFFFFF");
    else if (m_d->hovered)
        brushColor = darkMode() ? QColor("#323232") : QColor("#F2F2F2");
    else
        brushColor = Qt::transparent;

    painter.setBrush(brushColor);

    // Rounded Rect
    constexpr int radius = 8;
    painter.drawRoundedRect(rect().adjusted(-1, -1,  1, 1), radius, radius);

    // Icon
    int x = (width() - iconSize().width()) / 2;
    int y = (height() - iconSize().height()) / 2;
    painter.drawPixmap(x, y, darkMode() ? m_d->iconDark : m_d->iconLight);
}

bool WinButton::event(QEvent *event) {
    switch (event->type()) {
        case QEvent::Enter:
            m_d->hovered = true;
            update();
            break;
        case QEvent::Leave:
            m_d->hovered = false;
            update();
            break;
        case QEvent::MouseButtonPress:
            m_d->pressed = true;
            update();
            break;
        case QEvent::MouseButtonRelease:
            m_d->pressed = false;
            update();
            break;
    }

    return QPushButton::event(event);
}
