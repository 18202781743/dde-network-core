/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef VPNTLSSECTION_H
#define VPNTLSSECTION_H

#include "sections/abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace DCC_NAMESPACE {
class LineEditWidget;
class ComboxWidget;
class SwitchWidget;
}

namespace dcc {
namespace network {
class FileChooseWidget;
}
}

class VpnTLSSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnTLSSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnTLSSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, QString>> RemoteCertTypeStrMap;

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    DCC_NAMESPACE::LineEditWidget *m_remote;
    DCC_NAMESPACE::ComboxWidget *m_remoteCertTypeChooser;
    dcc::network::FileChooseWidget *m_caCert;
    DCC_NAMESPACE::SwitchWidget *m_customKeyDirection;
    DCC_NAMESPACE::ComboxWidget *m_keyDirectionChooser;

    QString m_currentRemoteCertType;
    QString m_currentKeyDirection;
};

#endif /* VPNTLSSECTION_H */
