/******************************************************************************
 * Copyright 2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#ifndef SAKANALYZERTOOLUI_HH
#define SAKANALYZERTOOLUI_HH

#include <QWidget>

namespace Ui {
class SAKAnalyzerToolUi;
}
class SAKAnalyzerToolUi : public QWidget
{
    Q_OBJECT
public:
    explicit SAKAnalyzerToolUi(QWidget *parent = nullptr);
    void setToolName(const QString &name);

private:
    Ui::SAKAnalyzerToolUi *ui;
};

#endif // SAKANALYZERTOOLUI_HH
