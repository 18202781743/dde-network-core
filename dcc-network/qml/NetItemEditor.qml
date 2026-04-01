// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dtk 1.0 as D

import org.deepin.dcc 1.0

D.ItemDelegate {
    Layout.fillWidth: true
    backgroundVisible: false
    checkable: false
    background: DccItemBackground {
        separatorVisible: true
    }
}
