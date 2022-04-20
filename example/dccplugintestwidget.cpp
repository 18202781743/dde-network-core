/*
 * Copyright (C) 2020 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "dccplugintestwidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <DGuiApplicationHelper>
#include <DPalette>
#include <QPainter>
#include <DApplicationHelper>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QScrollArea>
#include <DTitlebar>
#include <DPaletteHelper>

#include <QLineEdit>
#include <QScroller>
#include <QObject>
#include <QTimer>

#include "dccnetworkmodule.h"
#include "moduledatamodel.h"
//#include <widgets/multiselectlistview.h>
#include <DBackgroundGroup>
#include <DListView>
#include <DStyledItemDelegate>
#include <DFrame>
#include <QScreen>
#include <QScrollBar>
#include <QLabel>
typedef  DListView ListView;
//typedef  QListView TabView;
typedef  DStyledItemDelegate ListItemDelegate;
typedef  DStyledItemDelegate TabItemDelegate;
class TabView : public QListView
{
public:
    explicit TabView(QWidget *parent = nullptr)
        : QListView(parent)
    {
        setViewMode(QListView::ListMode);
        setFlow(QListView::LeftToRight);
        setResizeMode(QListView::Adjust);
        setMinimumSize(700,30);
        setMaximumHeight(50);
    }
};

const QSize MainWindowMininumSize(QSize(800, 600));
const QMargins ZeroMargins(0, 0, 0, 0);
const int NavViewMaximumWidth = QWIDGETSIZE_MAX;
const int NavViewMinimumWidth = 188;
const QSize ListViweItemIconSize_IconMode(84, 84);
const QSize ListViweItemSize_IconMode(280, 84);
const QSize ListViweItemIconSize_ListMode(32, 32);
const QSize ListViweItemSize_ListMode(168, 48);

static int WidgetMinimumWidth = 820;
static int WidgetMinimumHeight = 634;

const int second_widget_min_width = 230;
const int third_widget_min_width = 340;
const int widget_total_min_width = 820;
const int four_widget_min_widget = widget_total_min_width + third_widget_min_width + 40;
const int first_widget_min_width = 188;
const qint32 ActionIconSize = 30;//大图标角标大小

const QSize ListViweItemIconSize(84,84);
const QSize ListViweItemSize(170,168);

Q_DECLARE_METATYPE(QMargins)
const QMargins navItemMargin(5, 3, 5, 3);
const QVariant NavItemMargin = QVariant::fromValue(navItemMargin);

DccPluginTestWidget::DccPluginTestWidget(QWidget *parent)
    : DMainWindow(parent)
    , m_contentWidget(new QWidget(this))
    , m_backwardBtn(new DIconButton(QStyle::SP_ArrowBack, this))
//    , m_dconfig(new DConfig(ControlCenterConfig, QString(), this))
//    , m_searchWidget(new SearchWidget(this))
    , m_rootModule(nullptr)
    , m_currentModule(nullptr)
//    , m_pluginManager(nullptr)
    , m_mainView(nullptr)
{
    initUI();
    initConfig();
    loadModules();
}

DccPluginTestWidget::~DccPluginTestWidget()
{

}


int DccPluginTestWidget::getScrollPos(const int index)
{
    int pos = 0;
    for (int i = 0; i < qMin<int>(m_pages.count(), index); i++) {
        pos += m_pages[i]->height();
    }
    return pos;
}

void DccPluginTestWidget::showPage(const QString &url, const UrlType &uType)
{
    qInfo() << "show page url:" << url;
    if (url.isEmpty() || url == "/") {
        toHome();
    }
    if (!m_rootModule) {
        QTimer::singleShot(10, this, [ = ] {
            showPage(url, uType);
        });
        return;
    }
    showPage(m_rootModule, url, uType);
}

void DccPluginTestWidget::showPage(ModuleObject *const module, const QString &url, const UrlType &uType)
{
    QStringList names = url.split('/');
    const QString &name = names.takeFirst();
    int index = -1;
    QString childName;
    for (auto child : module->childrens()) {
        if (uType == UrlType::Name)
            childName = child->name();
        if (uType == UrlType::DisplayName)
            childName = child->moduleData()->DisplayName;
        if (childName == name || child->moduleData()->ContentText.contains(name)) {
            index = module->childrens().indexOf(child);
            Q_EMIT module->activeChild(index);
            return showPage(child, names.join('/'), uType);
        }
    }
}

void DccPluginTestWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange) {
        updateMainView();
    }
    return DMainWindow::changeEvent(event);
}

void DccPluginTestWidget::initUI()
{
    setMinimumSize(MainWindowMininumSize);
    m_contentWidget->setAccessibleName("contentwindow");
    m_contentWidget->setObjectName("contentwindow");
    setCentralWidget(m_contentWidget);

    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->setContentsMargins(ZeroMargins);


    auto menu = titlebar()->menu();
    if (!menu) {
        qDebug() << "menu is nullptr, create menu!";
        menu = new QMenu;
    }
    menu->setAccessibleName("titlebarmenu");
    titlebar()->setMenu(menu);

    auto action = new QAction(tr("Help"), menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this] {
        openManual();
    });

    m_backwardBtn->setAccessibleName("backwardbtn");

    titlebar()->addWidget(m_backwardBtn, Qt::AlignLeft | Qt::AlignVCenter);
    titlebar()->setIcon(QIcon::fromTheme("preferences-system"));

    connect(m_backwardBtn, &DIconButton::clicked, this, &DccPluginTestWidget::toHome);

//    m_searchWidget->setMinimumSize(300, 36);
//    m_searchWidget->setAccessibleName("SearchModule");
//    m_searchWidget->lineEdit()->setAccessibleName("SearchModuleLineEdit");
//    titlebar()->addWidget(m_searchWidget, Qt::AlignCenter);
}

void DccPluginTestWidget::initConfig()
{
//    if (!m_dconfig->isValid()) {
//        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_dconfig->name(), m_dconfig->subpath());
//        return;
//    }

//    auto w = m_dconfig->value(WidthConfig).toInt();
//    auto h = m_dconfig->value(HeightConfig).toInt();
//    resize(w, h);
//    Dtk::Widget::moveToCenter(this);
}

void DccPluginTestWidget::loadModules()
{
    QFutureWatcher<void> *watcher= new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this] {
        showModule(m_rootModule, m_contentWidget);
//            m_searchWidget->setModuleObject(m_rootModule);
    });

    QFuture<void> future = QtConcurrent::run([this] {
        m_rootModule = new ModuleObject();
        DccNetworkPlugin *net = new DccNetworkPlugin();
        m_rootModule->appendChild(net->module());
        m_rootModule->moveToThread(qApp->thread());
    });
    watcher->setFuture(future);
}

void DccPluginTestWidget::toHome()
{
    m_rootModule->setChildType(ModuleObject::ChildType::MainIcon);
    setCurrentModule(nullptr);
    showModule(m_rootModule, m_contentWidget);
}

void DccPluginTestWidget::updateMainView()
{
    if (!m_mainView)
        return;
    // set background
    DPalette pa = DPaletteHelper::instance()->palette(m_mainView);
    QColor baseColor = palette().base().color();
    DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(baseColor);
    if (ct == DGuiApplicationHelper::LightType) {
        pa.setBrush(DPalette::ItemBackground, palette().base());
    }
    else {
        baseColor = DGuiApplicationHelper::adjustColor(baseColor, 0, 0, +5, 0, 0, 0, 0);
        pa.setColor(DPalette::ItemBackground, baseColor);
    }
    DPaletteHelper::instance()->setPalette(m_mainView, pa);
}

void DccPluginTestWidget::clearPage(QWidget *const widget)
{
    QLayout *layout = widget->layout();
    QScrollArea *area = qobject_cast<QScrollArea *>(widget);
    if (area)
        area->widget()->deleteLater();
    if (layout) {
        while (QLayoutItem *child = layout->takeAt(0))
        {
            layout->removeWidget(child->widget());
            child->widget()->deleteLater();
            layout->removeItem(child);
            delete child;
        }
        delete layout;
    }
}

void DccPluginTestWidget::configLayout(QBoxLayout *const layout)
{
    layout->setMargin(0);
    layout->setSpacing(0);
}

void DccPluginTestWidget::showModule(ModuleObject *const module, QWidget *const parent, const int index)
{
    if (!module || !parent)
        return;
    module->active();
    if (module->childrens().isEmpty())
        return;
    if (module->findChild(currentModule()) >= 0)
        return;

    qDebug() << QString("module name:%1, index:%2, children size:%3").arg(module->name()).arg(index).arg(module->childrens().size());
    clearPage(parent);

    switch (module->childType())
    {
    case ModuleObject::ChildType::MainIcon:
        showModuleMainIcon(module, parent, index);
        break;
    case ModuleObject::ChildType::MainList:
        setCurrentModule(module);
        showModuleMainList(module, parent, index);
        break;
    case ModuleObject::ChildType::HList:
        setCurrentModule(module);
        showModuleHList(module, parent, index);
        break;
    case ModuleObject::ChildType::VList:
        setCurrentModule(module);
        showModuleVList(module, parent, index);
        break;
    case ModuleObject::ChildType::Page:
        setCurrentModule(module);
        showModulePage(module, parent, index);
    default:
        break;
    }
}

void DccPluginTestWidget::showModuleMainIcon(ModuleObject *const module, QWidget *const parent, const int index)
{
    QVBoxLayout *vlayout = new QVBoxLayout(parent);
    configLayout(vlayout);
    parent->setLayout(vlayout);

    ModuleDataModel *model = new ModuleDataModel(parent);
    model->setData(module);

    ListView *view = new ListView(parent);
    ListItemDelegate *delegate = new ListItemDelegate(view);
    view->setItemDelegate(delegate);
    vlayout->addWidget(view);
    view->setModel(model);
    view->setFrameShape(QFrame::Shape::NoFrame);
    view->setAutoScroll(true);
    view->setDragEnabled(false);
    view->setMaximumWidth(NavViewMaximumWidth);
    view->setMinimumWidth(NavViewMinimumWidth);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setIconSize(ListViweItemIconSize_IconMode);
    view->setGridSize(ListViweItemSize_IconMode);
    view->setSpacing(20);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setViewMode(ListView::IconMode);
    view->setAcceptDrops(false);
//    view->setAlignment(Qt::AlignCenter);
    m_mainView = view;
    updateMainView();

    m_backwardBtn->setEnabled(false);

    auto onClicked = [this, module, parent] (const QModelIndex &index) {
        module->setChildType(ModuleObject::ChildType::MainList);
        m_backwardBtn->setEnabled(true);
        // 展开主菜单时，原来的主菜单被析构，需清空其指针
        m_mainView->deleteLater();
        m_mainView = nullptr;
        showModule(module, parent, index.row());
    };

    connect(view, &ListView::activated, view, &ListView::clicked);
    connect(view, &ListView::clicked, view, onClicked);
    connect(module, &ModuleObject::activeChild, this , [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);
    if (index < 0)
        return;
    onClicked(model->index(index, 0));
}

void DccPluginTestWidget::showModuleMainList(ModuleObject *const module, QWidget *const parent, const int index)
{
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    configLayout(hlayout);
    parent->setLayout(hlayout);

    ModuleDataModel *model = new ModuleDataModel(parent);
    model->setData(module);

    ListView *view = new ListView(parent);
    ListItemDelegate *delegate = new ListItemDelegate(view);
    view->setItemDelegate(delegate);
    QWidget *childWdiget = new QWidget(parent);
    hlayout->addWidget(view, 1);
    hlayout->addWidget(childWdiget, 5);

    view->setModel(model);
    view->setFrameShape(QFrame::Shape::NoFrame);
    view->setAutoScroll(true);
    view->setDragEnabled(false);
    view->setMaximumWidth(NavViewMaximumWidth);
    view->setMinimumWidth(NavViewMinimumWidth);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    view->setIconSize(ListViweItemIconSize_ListMode);
    view->setGridSize(ListViweItemSize_ListMode);
    view->setSpacing(0);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto onClicked = [ = ] (const QModelIndex &index) {
        const int row = index.row();
        if (module->childrens().size() <= row) {
            qWarning() << "activated not exist item!";
            return;
        }
        view->setCurrentIndex(index);
        showModule(module->childrens()[row], childWdiget, 0);
    };

    connect(view, &ListView::activated, view, &ListView::clicked);
    connect(view, &ListView::clicked, view, onClicked);
    connect(module, &ModuleObject::activeChild, view, [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);
    connect(module, &ModuleObject::removedChild, childWdiget, [this](ModuleObject *const childModule) {
        if (childModule->findChild(currentModule()) >= 0) {
            toHome();
        }
    });
    if (index < 0)
        return;
    Q_EMIT view->clicked(model->index(index, 0));
}

void DccPluginTestWidget::showModuleHList(ModuleObject *const module, QWidget *const parent, const int index)
{
    QVBoxLayout *vlayout = new QVBoxLayout(parent);
    parent->setLayout(vlayout);

    DFrame *dframeTab = new DFrame(parent);
    QHBoxLayout *hlayout = new QHBoxLayout(dframeTab);
    hlayout->setMargin(3);
    hlayout->setSpacing(0);
    TabView *view = new TabView(parent);
    hlayout->addWidget(view);
    ModuleDataModel *model = new ModuleDataModel(view);
    TabItemDelegate *delegate = new TabItemDelegate(view);
    model->setData(module);
    view->setModel(model);
    view->setItemDelegate(delegate);
    vlayout->addWidget(dframeTab, 1, Qt::AlignCenter);

    QWidget *childWdiget = new QWidget(parent);
    DFrame *childFrame = new DFrame(parent);

    connect(module, &ModuleObject::removedChild, this, [this] (ModuleObject *const module) {
        if (module->findChild(currentModule()) >= 0) {
            toHome();
        }
    });

    auto onClicked = [ = ] (const QModelIndex &index) {
        const int row = index.row();
        if (module->childrens().size() <= row) {
            qWarning() << "activated not exist item!";
            return;
        }
        view->setCurrentIndex(index);

        // 判断子项是否为垂直菜单，如果是则需要加上Frame
        if (vlayout->count() >= 2)
            vlayout->takeAt(vlayout->count() - 1);
        if (module->childrens()[row]->childType() == ModuleObject::ChildType::VList) {
            vlayout->addWidget(childFrame, 6);
            childFrame->show();
            childWdiget->hide();
            showModule(module->childrens()[row], childFrame, 0);
        } else {
            childFrame->hide();
            childWdiget->show();
            vlayout->addWidget(childWdiget, 6);
            showModule(module->childrens()[row], childWdiget, 0);
        }
    };

    connect(view, &TabView::activated, view, &TabView::clicked);
    connect(view, &TabView::clicked, this, onClicked);
    connect(module, &ModuleObject::activeChild, view, [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);
    if (index < 0)
        return;
    Q_EMIT view->clicked(model->index(index, 0));
}

void DccPluginTestWidget::showModuleVList(ModuleObject *const module, QWidget *const parent, const int index)
{
    QHBoxLayout *hlayout = new QHBoxLayout(parent);
    configLayout(hlayout);
    parent->setLayout(hlayout);

    ModuleDataModel *model = new ModuleDataModel(parent);
    model->setData(module);
    DListView *view = new DListView(parent);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *vlayout = new QVBoxLayout;
    widget->setLayout(vlayout);
    vlayout->addWidget(view);
    QWidget *extraButton = module->extraButton();
    if (extraButton)
        vlayout->addWidget(getExtraPage(extraButton));
    hlayout->addWidget(widget, 1);
    hlayout->addWidget(new DVerticalLine);

    QWidget *childWidget = new QWidget(parent);
    hlayout->addWidget(childWidget, 5);

    view->setModel(model);
    view->setFrameShape(QFrame::NoFrame);
    view->setAutoScroll(true);
    view->setDragEnabled(false);
    view->setMaximumWidth(NavViewMaximumWidth);
    view->setMinimumWidth(NavViewMinimumWidth);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setIconSize(ListViweItemIconSize_ListMode);
    view->setItemSize(ListViweItemSize_ListMode);
    view->setSpacing(0);
    view->setItemSpacing(2);
    view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto onClicked = [this, module, childWidget, view] (const QModelIndex &index) {
        const int row = index.row();
        if (module->childrens().size() <= row) {
            qWarning() << "activated not exist item!";
            return;
        }
        view->setCurrentIndex(index);
        showModule(module->childrens()[row], childWidget, 0);
    };

    connect(view, &ListView::activated, view, &ListView::clicked);
    connect(view, &ListView::clicked, view, onClicked);
    connect(module, &ModuleObject::activeChild, view, [onClicked, model] (const int index) {
        onClicked(model->index(index, 0));
    });
    connect(module, &ModuleObject::extraButtonClicked, childWidget, [this, childWidget, module] {
        clearPage(childWidget);
        setCurrentModule(nullptr);
        QVBoxLayout *tempLayout = new QVBoxLayout;
        configLayout(tempLayout);
        childWidget->setLayout(tempLayout);
        tempLayout->addWidget(module->page());
    });
    connect(view, &ListView::destroyed, module, &ModuleObject::deactive);

    if (index < 0)
        return;
    Q_EMIT view->clicked(model->index(index, 0));
}

void DccPluginTestWidget::showModulePage(ModuleObject *const module, QWidget *const parent, const int index)
{
    QScrollArea *area = new QScrollArea(parent);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    configLayout(mainLayout);
    parent->setLayout(mainLayout);
    mainLayout->addWidget(area);

    area->setFrameShape(QFrame::NoFrame);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidgetResizable(true);

    QWidget *areaWidget = new QWidget(area);
    area->setWidget(areaWidget);
    QVBoxLayout *vlayout = new QVBoxLayout(areaWidget);
    configLayout(vlayout);
    areaWidget->setLayout(vlayout);

    m_pages.clear();
    for (auto child : module->childrens()) {
        auto page = getPage(child->page(), child->moduleData()->DisplayName);
        if (page) {
            m_pages << page;
            vlayout->addWidget(page);
        }
        child->active();

        connect(child, &ModuleObject::moduleDataChanged, area, [ = ] {
            vlayout->removeWidget(page);
            const int index = m_pages.indexOf(page);
            m_pages.removeOne(page);
            page->deleteLater();
            auto newPage = getPage(child->page(), child->moduleData()->DisplayName);
            if (newPage) {
                m_pages.insert(index, newPage);
                vlayout->insertWidget(index, newPage);
            }
        });
    }
    if (m_pages.count() > 1)
        vlayout->addStretch(1);

    QWidget *extraButton = module->extraButton();
    if (extraButton)
        vlayout->addWidget(getExtraPage(extraButton), 0, Qt::AlignBottom);
    area->verticalScrollBar()->setSliderPosition(getScrollPos(index));

    connect(module, &ModuleObject::activeChild, area, [this, area] (const int index) {
        area->verticalScrollBar()->setSliderPosition(getScrollPos(index));
    });

    connect(module, &ModuleObject::removedChild, area, [this, module, vlayout](ModuleObject *const childModule) {
        int index = module->childrens().indexOf(childModule);
        QWidget *w = m_pages.at(index);
        vlayout->removeWidget(w);
        w->deleteLater();
        m_pages.removeAt(index);
    });
    auto addChild = [this, module, vlayout](ModuleObject *const childModule) {
        int index = module->childrens().indexOf(childModule);
        auto newPage = getPage(childModule->page(), childModule->moduleData()->DisplayName);
        if (newPage) {
            m_pages.insert(index, newPage);
            vlayout->insertWidget(index, newPage);
        }
    };
    connect(module, &ModuleObject::insertedChild, area, addChild);
    connect(module, &ModuleObject::appendedChild, area, addChild);

    connect(areaWidget, &QWidget::destroyed, module, [module] {
        for (auto child : module->childrens()) {
            child->deactive();
        }
        module->deactive();
    });
}

QWidget* DccPluginTestWidget::getPage(QWidget *const widget, const QString &title)
{
    if (!widget)
        return nullptr;
    QLabel *titleLbl = new QLabel(title, this);
    QWidget *page = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(page);
    page->setLayout(vLayout);
    vLayout->addWidget(titleLbl, 0, Qt::AlignTop);
    vLayout->addWidget(widget, 1, Qt::AlignTop);
    if (title.isEmpty()) {
        titleLbl->setVisible(false);
    }
    return page;
}

QWidget* DccPluginTestWidget::getExtraPage(QWidget *const widget)
{
    QWidget *tmpWidget = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(tmpWidget);
    tmpWidget->setLayout(vLayout);
    vLayout->addWidget(widget);
    vLayout->setSpacing(0);
    return tmpWidget;
}

void DccPluginTestWidget::openManual()
{
    QString helpTitle;
    if (currentModule())
        helpTitle = currentModule()->name();
    if (helpTitle.isEmpty())
        helpTitle = "controlcenter";

//    const QString &dmanInterface = "com.deepin.Manual.Open";
//    QDBusInterface interface(dmanInterface,
//                             "/com/deepin/Manual/Open",
//                             dmanInterface,
//                             QDBusConnection::sessionBus());

//    QDBusMessage reply = interface.call("OpenTitle", "dde", helpTitle);
//    if (reply.type() == QDBusMessage::ErrorMessage) {
//        qWarning() << "Open manual failed, error message:" << reply.errorMessage();
//    }
}
