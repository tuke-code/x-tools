﻿/****************************************************************************************
 * Copyright 2018-2021 Qter(qsaker@qq.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ***************************************************************************************/
#include <QFile>
#include <QTime>
#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSqlError>
#include <QJsonArray>
#include <QListWidget>
#include <QPushButton>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QStandardPaths>

#include "SAKDebugger.hh"
#include "SAKDebuggerInputDataPreset.hh"
#include "SAKDebuggerInputDataPresetItem.hh"

#include "ui_SAKDebuggerInputDataPreset.h"

SAKDebuggerInputDataPreset::SAKDebuggerInputDataPreset(
        QSqlDatabase *sqlDatabase,
        QSettings *settings,
        QString settingsGroup,
        QMenu *itemsMenu,
        QWidget *parent)
    :QWidget(parent)
    ,mSqlDatabase(sqlDatabase)
    ,mSettings(settings)
    ,mSettingsGroup(settingsGroup)
    ,mItemsMenu(itemsMenu)
    ,mUi(new Ui::SAKDebuggerInputDataPreset)
{
    mUi->setupUi(this);
    mClearMessageInfoTimer.setInterval(SAK_CLEAR_MESSAGE_INTERVAL);
    connect(&mClearMessageInfoTimer, &QTimer::timeout, this, [&](){
        mClearMessageInfoTimer.stop();
        mUi->infoLabel->clear();
    });


    connect(mUi->clearPushButton, &QPushButton::clicked,
            this, [=](){
        QString title = tr("Clear Data");
        QString text = tr("All Data Items Will Be Deleted!")
                + " "
                + tr("Are you sure you want to do this?");
        auto ret = QMessageBox::warning(this,
                                        title,
                                        text,
                                        QMessageBox::Ok|QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            while (mUi->itemListWidget->count()) {
                QListWidgetItem *item = mUi->itemListWidget->item(0);
                deleteItem(item);
            }
        }
    });
    connect(mUi->exportPushButton, &QPushButton::clicked,
            this, &SAKDebuggerInputDataPreset::exportItems);
    connect(mUi->importPushButton, &QPushButton::clicked,
            this, &SAKDebuggerInputDataPreset::importItems);
    connect(mUi->deletePushButton, &QPushButton::clicked,
            this, [=](){
        QListWidgetItem *item = mUi->itemListWidget->currentItem();
        deleteItem(item);
    });
    connect(mUi->addPushButton, &QPushButton::clicked,
            this, [=](){
        QListWidgetItem *item = new QListWidgetItem();
        SAKDebuggerInputDataPresetItem *itemWidget
                = new SAKDebuggerInputDataPresetItem();
        setItemWidget(item, itemWidget);
        insertRecord(mTableContext.tableName, itemWidget);
    });


    mTableContext.tableName = settingsGroup + QString("PresetData");
    QString queryString = QString("CREATE TABLE '%1' (")
            .arg(mTableContext.tableName);
    queryString.append(QString("%1 INTEGER PRIMARY KEY NOT NULL, ")
                       .arg(mTableContext.columns.id));
    queryString.append(QString("%1 INTEGER NOT NULL, ")
                       .arg(mTableContext.columns.format));
    queryString.append(QString("%1 TEXT NOT NULL, ")
                       .arg(mTableContext.columns.description));
    queryString.append(QString("%1 TEXT NOT NULL)")
                       .arg(mTableContext.columns.text));
    // Try to create table
    mSqlQuery = QSqlQuery(*sqlDatabase);
    if (!sqlDatabase->tables().contains(mTableContext.tableName)) {
        if (!mSqlQuery.exec(queryString)) {
            QString errorString = mSqlQuery.lastError().text();
            outputMessage(errorString, true);
            qWarning() << "Create table("
                       << mTableContext.tableName
                       << ")failed:"
                       << errorString;
            qInfo() << queryString;
        }
    }
    readinRecord();
}

SAKDebuggerInputDataPreset::~SAKDebuggerInputDataPreset()
{
    delete mUi;
}

void SAKDebuggerInputDataPreset::readinRecord()
{
    const QString queryString = QString("SELECT * FROM %1")
            .arg(mTableContext.tableName);
    if (mSqlQuery.exec(queryString)) {
        while (mSqlQuery.next()) {
            SAKDebuggerInputDataPresetItem::ITEM_CTX itemContext;
            QVariant tempVariant = mSqlQuery.value(mTableContext.columns.id);
            itemContext.id = tempVariant.toULongLong();

            tempVariant = mSqlQuery.value(mTableContext.columns.format);
            itemContext.format = tempVariant.toUInt();

            tempVariant = mSqlQuery.value(mTableContext.columns.description);
            itemContext.description = tempVariant.toString();

            tempVariant = mSqlQuery.value(mTableContext.columns.text);
            itemContext.text = tempVariant.toString();

            QListWidgetItem *item = new QListWidgetItem();
            auto *itemWidget = new SAKDebuggerInputDataPresetItem(itemContext);
            setItemWidget(item, itemWidget);
        }
    } else {
        qWarning() << "Can not exec query command:"
                   << qPrintable(queryString);
    }
}

void SAKDebuggerInputDataPreset::outputMessage(QString msg, bool isError)
{
    QString color = "black";
    if (isError){
        color = "red";
        QApplication::beep();
    }
    mUi->infoLabel->setStyleSheet(QString("QLabel{color:%1}").arg(color));

    mUi->infoLabel->setText(QTime::currentTime().toString("hh:mm:ss ") + msg);
    mClearMessageInfoTimer.start();
}

void SAKDebuggerInputDataPreset::updateFormat(quint64 id, int format)
{
    SAKDebugger::commonSqlApiUpdateRecord(&mSqlQuery,
                                          mTableContext.tableName,
                                          mTableContext.columns.format,
                                          QVariant::fromValue(format),
                                          id,
                                          true);
}

void SAKDebuggerInputDataPreset::updateDescription(quint64 id,
                                           const QString &description)
{
    SAKDebugger::commonSqlApiUpdateRecord(&mSqlQuery,
                                          mTableContext.tableName,
                                          mTableContext.columns.description,
                                          QVariant::fromValue(description),
                                          id,
                                          true);
}

void SAKDebuggerInputDataPreset::updateText(quint64 id, const QString &text)
{
    SAKDebugger::commonSqlApiUpdateRecord(&mSqlQuery,
                                          mTableContext.tableName,
                                          mTableContext.columns.text,
                                          QVariant::fromValue(text),
                                          id,
                                          true);
}

void SAKDebuggerInputDataPreset::insertRecord(
        const QString &tableName,
        SAKDebuggerInputDataPresetItem *itemWidget)
{
    SAKStructDataPresetItemTableContext tableCtx;
    QString queryString = QString("INSERT INTO %1(%2,%3,%4,%5)"
                                  " VALUES(%6,%7,'%8','%9')")
            .arg(tableName,
                 tableCtx.columns.id,
                 tableCtx.columns.format,
                 tableCtx.columns.description,
                 tableCtx.columns.text,
                 QString::number(itemWidget->itemID()),
                 QString::number(itemWidget->itemTextFromat()),
                 itemWidget->itemDescription(),
                 itemWidget->itemText());
    if (!mSqlQuery.exec(queryString)) {
#if 0
        qWarning() << "Insert record to("
                   << tableName
                   << ") table failed: "
                   << mSqlQuery.lastError().text();
        qInfo() << queryString;
#endif
    }
}

void SAKDebuggerInputDataPreset::setItemWidget(
        QListWidgetItem *item,
        SAKDebuggerInputDataPresetItem *itemWidget)
{
    bool contain = false;
    for (int i = 0; i < mUi->itemListWidget->count(); i++) {
        QListWidgetItem *item = mUi->itemListWidget->item(i);
        QWidget *w = mUi->itemListWidget->itemWidget(item);
        auto *iw = qobject_cast<SAKDebuggerInputDataPresetItem*>(w);
        if (iw->itemID() == itemWidget->itemID()) {
            contain = true;
            break;
        }
    }

    if (!contain) {
        item->setSizeHint(itemWidget->sizeHint());
        mUi->itemListWidget->addItem(item);
        mUi->itemListWidget->setItemWidget(item, itemWidget);

        connect(itemWidget,
                &SAKDebuggerInputDataPresetItem::formatChanged,
                this,
                &SAKDebuggerInputDataPreset::updateFormat);
        connect(itemWidget,
                &SAKDebuggerInputDataPresetItem::textChanged,
                this,
                &SAKDebuggerInputDataPreset::updateText);
        connect(itemWidget,
                &SAKDebuggerInputDataPresetItem::descriptionChanged,
                this,
                &SAKDebuggerInputDataPreset::updateDescription);

        QAction *action = mItemsMenu->addAction(itemWidget->itemDescription(),
                                                this,
                                                [=](){
            QString rawData = itemWidget->itemText();
            int format = itemWidget->itemTextFromat();
            emit this->invokeWriteBytes(rawData, format);
        });
        // The action will be deleted after item widget is destroyed.
        connect(itemWidget, &SAKDebuggerInputDataPresetItem::destroyed,
                action, &QAction::deleteLater);
        connect(itemWidget,
                &SAKDebuggerInputDataPresetItem::descriptionChanged,
                this,
                [=](quint64 id, const QString text){
            Q_UNUSED(id);
            action->setText(text);
        });
    }
}

void SAKDebuggerInputDataPreset::deleteItem(QListWidgetItem *item)
{
    if (item) {
        QWidget *itemWidget = mUi->itemListWidget->itemWidget(item);
        auto *iw =
                reinterpret_cast<SAKDebuggerInputDataPresetItem*>(itemWidget);
        quint64 id = iw->itemID();
        QString tableName = mTableContext.tableName;

        // Delete record from database.
        mUi->itemListWidget->removeItemWidget(item);
        delete item;
        SAKDebugger::commonSqlApiDeleteRecord(&mSqlQuery, tableName, id);
    } else {
        outputMessage(tr("Plese select an item first."));
    }
}

void SAKDebuggerInputDataPreset::exportItems()
{
    QJsonArray jsonArray;
    for (int i = 0; i < mUi->itemListWidget->count(); i++) {
        auto *itemWidget = mUi->itemListWidget->itemWidget(mUi->itemListWidget->item(i));
        auto *iw = qobject_cast<SAKDebuggerInputDataPresetItem*>(itemWidget);
        if (iw) {
            QJsonObject obj;
            QString key;
            QJsonValue value;


            key = mTableContext.columns.id;
            value = QVariant::fromValue(iw->itemID()).toJsonValue();
            obj.insert(key, value);

            key = mTableContext.columns.format;
            value = QVariant::fromValue(iw->itemTextFromat()).toJsonValue();
            obj.insert(key, value);

            key = mTableContext.columns.description;
            value = QVariant::fromValue(iw->itemDescription()).toJsonValue();
            obj.insert(key, value);

            key = mTableContext.columns.text;
            value = QVariant::fromValue(iw->itemText()).toJsonValue();
            obj.insert(key, value);


            jsonArray.append(QJsonValue(obj));
        }
    }
    QJsonDocument jsonDoc;
    jsonDoc.setArray(jsonArray);


    // Open file
    auto desktopLocation = QStandardPaths::DesktopLocation;
    QString defaultName = QStandardPaths::writableLocation(desktopLocation);
    defaultName.append(QString("/"));
    defaultName.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    defaultName.append(".json");
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Outport data"),
                                                    defaultName,
                                                    QString("json (*.json)"));
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QFile::ReadWrite)){
            file.write(jsonDoc.toJson());
            file.close();
        }else{
            outputMessage(file.errorString(), false);
        }
    }
}

void SAKDebuggerInputDataPreset::importItems()
{
    auto writableLocation = QStandardPaths::DesktopLocation;
    QString defaultPath = QStandardPaths::writableLocation(writableLocation);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import data"),
                                                    defaultPath,
                                                    QString("json (*.json)"));
    QFile file(fileName);
    QByteArray jsonArray;
    if (file.open(QFile::ReadWrite)) {
        jsonArray = file.readAll();
        file.close();
    } else {
        outputMessage(file.errorString(), false);
        return;
    }

    QJsonDocument jsc = QJsonDocument::fromJson(jsonArray);
    QJsonArray jsa = jsc.array();
    for (int i = 0; i < jsa.count(); i++) {
        if (!jsa.at(i).isObject()) {
            continue;
        }

        QJsonObject jso = jsa.at(i).toObject();
        SAKDebuggerInputDataPresetItem::SAKStructDataPresetItemContext itemCtx;
        QVariant idVariant = jso.value(mTableContext.columns.id);
        itemCtx.id = idVariant.toULongLong();

        QVariant formatVariant = jso.value(mTableContext.columns.format);
        itemCtx.format = formatVariant.toUInt();

        QVariant desVariant = jso.value(mTableContext.columns.description);
        itemCtx.description = desVariant.toString();

        QVariant textVariant = jso.value(mTableContext.columns.text);
        itemCtx.text = textVariant.toString();

        QListWidgetItem *item = new QListWidgetItem();
        auto itemWidget = new SAKDebuggerInputDataPresetItem(itemCtx);
        setItemWidget(item, itemWidget);
        insertRecord(mTableContext.tableName,
                     qobject_cast<SAKDebuggerInputDataPresetItem*>(itemWidget));
    }
}
