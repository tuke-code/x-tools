﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "wsservertransferview.h"

#include "wsservertransfermodel.h"

WsServerTransferView::WsServerTransferView(QWidget *parent)
    : SocketTransferView{parent}
{
    auto model = new WsServerTransferModel(this);
    setTableModel(model);
    connect(model, &WsServerTransferModel::outputBytes, this, &WsServerTransferView::outputBytes);

    auto tv = tableView();
    tv->hideColumn(1);
    tv->hideColumn(2);
    for (int i = 6; i < 9; ++i) {
        tv->hideColumn(i);
    }

    auto hHeader = tv->horizontalHeader();
    hHeader->setStretchLastSection(true);
}

WsServerTransferView::~WsServerTransferView() {}
