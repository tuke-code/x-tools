﻿/*
 * Copyright 2018-2020 Qter(qsaker@qq.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 */
#include "SAKDebugPage.hh"
#include "SAKBaseTransmissionItemWidget.hh"

SAKBaseTransmissionItemWidget::SAKBaseTransmissionItemWidget(SAKDebugPage *debugPage, QWidget *parent)
    :QWidget (parent)
    ,debugPage (debugPage)
{
    connect(debugPage, &SAKDebugPage::bytesRead, this, &SAKBaseTransmissionItemWidget::write);
    connect(this, &SAKBaseTransmissionItemWidget::bytesRead, debugPage, &SAKDebugPage::write);
}

void SAKBaseTransmissionItemWidget::write(QByteArray data)
{
    Q_UNUSED(data);
}
