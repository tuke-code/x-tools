﻿/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "barchartsettings.h"
#include "ui_barchartsettings.h"

#include <QAbstractSeries>
#include <QChartView>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPointF>
#include <QTimer>

#include "barchartview.h"
#include "common/xtools.h"
#include "page/charts/chartsview.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
using namespace QtCharts;
#endif

BarChartSettings::BarChartSettings(QWidget *parent)
    : ChartSettings(parent)
    , ui(new Ui::BarChartSettings)
{
    ui->setupUi(this);
    ui->checkBoxLegend->setChecked(true);
    QComboBox *cb = ui->comboBoxDataType;
    cb->addItem(tr("Binary") + "-Y", static_cast<int>(ChartsView::DataFormat::BinaryY));
    cb->addItem(tr("Text") + "-Y", static_cast<int>(ChartsView::DataFormat::TextY));
#if 0
    cb->addItem(tr("Binary") + "-XY", static_cast<int>(Charts::DataFormat::BinaryXY));
    cb->addItem(tr("Text") + "-XY", static_cast<int>(Charts::DataFormat::TextXY));
#endif
    connect(ui->comboBoxDataType, xComboBoxActivated, this, [=]() {
        emit this->dataFormatChanged(ui->comboBoxDataType->currentData().toInt());
    });

    connect(ui->checkBoxLegend,
            &QCheckBox::clicked,
            this,
            &BarChartSettings::invokeSetLegendVisible);
    connect(ui->pushButtonClear,
            &QPushButton::clicked,
            this,
            &BarChartSettings::invokeClearChannels);
    connect(ui->pushButtonImport,
            &QPushButton::clicked,
            this,
            &BarChartSettings::invokeImportChannels);
    connect(ui->pushButtonExport,
            &QPushButton::clicked,
            this,
            &BarChartSettings::invokeExportChannels);

    QGridLayout *parametersGridLayout = new QGridLayout(ui->widgetControl);
    parametersGridLayout->addWidget(new QLabel(tr("Channel"), this), 0, 0, Qt::AlignCenter);
    parametersGridLayout->addWidget(new QLabel(tr("Visible"), this), 0, 1, Qt::AlignCenter);
    auto *typeLabel = new QLabel(tr("Type"), this);
    parametersGridLayout->addWidget(typeLabel, 0, 2, Qt::AlignCenter);
    typeLabel->hide();
    parametersGridLayout->addWidget(new QLabel(tr("Color"), this), 0, 3, Qt::AlignCenter);
    parametersGridLayout->addWidget(new QLabel(tr("Name"), this), 0, 4, Qt::AlignCenter);

    const int channelNumber = channelCount();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    while (m_channelContexts.size() < channelNumber) {
        m_channelContexts.append({nullptr});
    }
#else
    m_channelContexts.resize(channelNumber);
#endif
    for (int i = 0; i < channelNumber; ++i) {
        int row = i + 1;
        QString str = QString::number(row);
        auto *label = new QLabel(str, this);
        parametersGridLayout->addWidget(label, row, 0, Qt::AlignCenter);

        auto *visibleCheckedBox = new QCheckBox(this);
        parametersGridLayout->addWidget(visibleCheckedBox, row, 1, Qt::AlignCenter);
        setupVisibleCheckBox(visibleCheckedBox, i);

        auto *typeComboBox = new QComboBox(this);
        parametersGridLayout->addWidget(typeComboBox, row, 2, Qt::AlignCenter);
        setupTypeComboBox(typeComboBox, i);
        typeComboBox->hide();

        auto *colorButton = new QPushButton(this);
        parametersGridLayout->addWidget(colorButton, row, 3, Qt::AlignCenter);
        setupColorButton(colorButton, i);

        auto *nameLineEdit = new QLineEdit(this);
        parametersGridLayout->addWidget(nameLineEdit, row, 4, Qt::AlignCenter);
        setupNameLineEdit(nameLineEdit, i);
    }
}

BarChartSettings::~BarChartSettings()
{
    delete ui;
}

QVariantMap BarChartSettings::save() const
{
    return QVariantMap();
}

void BarChartSettings::load(const QVariantMap &parameters)
{
    if (parameters.isEmpty()) {
        return;
    }

    BarChartSettings1 keys;

    setDataType(parameters.value(keys.dataType).toInt());

    bool legendVisible = parameters.value(keys.legendVisible).toBool();
    ui->checkBoxLegend->setChecked(legendVisible);

    int cachePoints = parameters.value(keys.cachePoints).toInt();
    ui->spinBoxCachePoints->setValue(cachePoints);

    QJsonArray channels = parameters.value(keys.channels).toJsonArray();
    int channelCount = channels.count();
    int contextCount = m_channelContexts.size();
    if (channels.count() != m_channelContexts.size()) {
        qWarning("The number(%d) of channels is not equal to the number of series(%d).",
                 channelCount,
                 contextCount);
        return;
    }

    for (int i = 0; i < channels.size(); ++i) {
        QJsonObject obj = channels[i].toObject();
        QString name = obj.value(keys.channelName).toString();
        bool visible = obj.value(keys.channelVisible).toBool();
        QString color = obj.value(keys.channelColor).toString();
        int type = obj.value(keys.channelType).toInt();

        m_channelContexts[i].nameLineEdit->setText(name);
        m_channelContexts[i].visibleCheckBox->setChecked(visible);
        m_channelContexts[i].colorButton->setStyleSheet("background-color: " + color + ";");
        int index = m_channelContexts[i].typeComboBox->findData(type);
        m_channelContexts[i].typeComboBox->setCurrentIndex(index);
    }
}

int BarChartSettings::channelCount()
{
    return 16;
}

int BarChartSettings::dataType()
{
    return ui->comboBoxDataType->currentData().toInt();
}

void BarChartSettings::setDataType(int type)
{
    int index = ui->comboBoxDataType->findData(type);
    if (index != -1) {
        ui->comboBoxDataType->setCurrentIndex(index);
    }
}

bool BarChartSettings::legendVisible()
{
    return ui->checkBoxLegend->isChecked();
}

void BarChartSettings::setLegendVisible(bool visible)
{
    ui->checkBoxLegend->setChecked(true);
}

int BarChartSettings::cachePoints()
{
    return ui->spinBoxCachePoints->value();
}

void BarChartSettings::setCachePoints(int points)
{
    ui->spinBoxCachePoints->setValue(points);
}

void BarChartSettings::updateUiState(bool ioIsOpened)
{
    ui->comboBoxDataType->setEnabled(!ioIsOpened);
}

void BarChartSettings::setupVisibleCheckBox(QCheckBox *checkBox, int channelIndex)
{
    checkBox->setCheckable(true);
    checkBox->setChecked(true);
    m_channelContexts[channelIndex].visibleCheckBox = checkBox;
    connect(checkBox, &QCheckBox::clicked, this, [=](bool checked) {
        emit channelVisibleChanged(channelIndex, checked);
    });
}

void BarChartSettings::setupTypeComboBox(QComboBox *comboBox, int channelIndex)
{
    m_channelContexts[channelIndex].typeComboBox = comboBox;
    comboBox->clear();
    comboBox->addItem(QIcon(":/res/icons/line_series.svg"),
                      seriesTypeToString(QAbstractSeries::SeriesType::SeriesTypeLine),
                      QAbstractSeries::SeriesType::SeriesTypeLine);
    comboBox->addItem(QIcon(":/res/icons/spline_series.svg"),
                      seriesTypeToString(QAbstractSeries::SeriesType::SeriesTypeSpline),
                      QAbstractSeries::SeriesType::SeriesTypeSpline);
    comboBox->addItem(QIcon(":/res/icons/scatter_series.svg"),
                      seriesTypeToString(QAbstractSeries::SeriesType::SeriesTypeScatter),
                      QAbstractSeries::SeriesType::SeriesTypeScatter);
    connect(comboBox, QOverload<int>::of(xComboBoxActivated), this, [=](int index) {
        emit channelTypeChanged(channelIndex, comboBox->itemData(index).toInt());
    });
}

void BarChartSettings::setupColorButton(QPushButton *button, int channelIndex)
{
    m_channelContexts[channelIndex].colorButton = button;
    button->setStyleSheet("background-color: rgb(0, 0, 255);");

    connect(button, &QPushButton::clicked, this, [=]() {
        QColor color = QColorDialog::getColor(Qt::blue, this);
        if (color.isValid()) {
            button->setStyleSheet("background-color: " + color.name() + ";");
            emit channelColorChanged(channelIndex, color);
        }
    });
}

void BarChartSettings::setupNameLineEdit(QLineEdit *lineEdit, int channelIndex)
{
    m_channelContexts[channelIndex].nameLineEdit = lineEdit;
    lineEdit->setText(tr("Channel") + " " + QString::number(channelIndex + 1));

    connect(lineEdit, &QLineEdit::textEdited, this, [=]() {
        emit channelNameChanged(channelIndex, lineEdit->text());
    });
}

QString BarChartSettings::seriesTypeToString(int type) const
{
    switch (type) {
    case QAbstractSeries::SeriesType::SeriesTypeLine:
        return tr("Line");
    case QAbstractSeries::SeriesType::SeriesTypeSpline:
        return tr("Spline");
    case QAbstractSeries::SeriesType::SeriesTypeScatter:
        return tr("Scatter");
    default:
        return QString();
    }
}
