﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include "socketserverui.h"

namespace xTools {

class TcpServerUi : public SocketServerUi
{
    Q_OBJECT
public:
    explicit TcpServerUi(QWidget *parent = nullptr);
    ~TcpServerUi() override;

    void setUiEnabled(bool enabled) override;
};

} // namespace xTools
