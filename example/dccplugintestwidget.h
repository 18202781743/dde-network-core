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

#ifndef DCCPLUGINTESTWIDGET_H
#define DCCPLUGINTESTWIDGET_H

#include <QWidget>
#include <QStack>

//#include <interface/frameproxyinterface.h>
#include <interface/moduleobject.h>

#include <DMainWindow>

class QPushButton;
class DCCNetworkModule;
class QHBoxLayout;
class FourthColWidget;
class QMouseEvent;
class QPaintEvent;
class QStandardItemModel;
class QTranslator;
class QAbstractItemView;
class QBoxLayout;

DWIDGET_BEGIN_NAMESPACE
class DBackgroundGroup;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

class DccPluginTestWidget : public DMainWindow
{
    Q_OBJECT

public:
    enum class UrlType {
        Name,
        DisplayName
    };
    explicit DccPluginTestWidget(QWidget *parent = nullptr);
    ~DccPluginTestWidget();


    /**
     * @brief 显示路径请求的页面，用于搜索或DBus接口
     * @param url 路径地址,从左至右搜索，如路径错误，只显示已搜索出的模块
     */
    void showPage(const QString &url, const UrlType &uType);

protected:
    void changeEvent(QEvent *event) override;

private:
    void openManual();
    void initUI();
    void initConfig();
    void loadModules();
    void toHome();
    void updateMainView();
    void clearPage(QWidget *const widget);
    void configLayout(QBoxLayout *const layout);
    int getScrollPos(const int index);
    void showPage(ModuleObject *const module, const QString &url, const UrlType &uType);
    void showModule(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleMainIcon(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleMainList(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleHList(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleVList(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModulePage(ModuleObject *const module, QWidget *const parent, const int index = -1);
    QWidget *getPage(QWidget *const widget, const QString &title);
    QWidget *getExtraPage(QWidget *const widget);

    inline void setCurrentModule(ModuleObject *const module) { m_currentModule = module; }
    inline ModuleObject *currentModule() const { return m_currentModule; }

private:
    QWidget                             *m_contentWidget;
    Dtk::Widget::DIconButton            *m_backwardBtn;         //回退按钮
//    Dtk::Core::DConfig                  *m_dconfig;             //配置
//    SearchWidget                        *m_searchWidget;        //搜索框
    ModuleObject                        *m_rootModule;
    ModuleObject                        *m_currentModule;
//    PluginManager                       *m_pluginManager;
    QAbstractItemView                   *m_mainView;            //保存主菜单view, 方便改变背景
    QList<QWidget*>                     m_pages;                //保存终点的页面
};



#endif // DCCPLUGINTESTWIDGET_H
