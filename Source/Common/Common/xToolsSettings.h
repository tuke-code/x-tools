/***************************************************************************************************
 * Copyright 2023-2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QLoggingCategory>
#include <QSettings>
#include <Qt>

class xToolsSettings : public QSettings
{
    Q_OBJECT
    // clang-format off
    Q_PROPERTY(int hdpiPolicy READ hdpiPolicy WRITE setHdpiPolicy NOTIFY hdpiPolicyChanged)
    Q_PROPERTY(int uiType READ uiType WRITE setUiType NOTIFY uiTypeChanged)
    Q_PROPERTY(bool clearSettings READ clearSettings WRITE setClearSettings NOTIFY clearSettingsChanged)
    Q_PROPERTY(int pageIndex READ pageIndex WRITE setPageIndex NOTIFY pageIndexChanged)
    Q_PROPERTY(bool isTextBesideIcon READ isTextBesideIcon WRITE setIsTextBesideIcon NOTIFY isTextBesideIconChanged)
    Q_PROPERTY(int palette READ palette WRITE setPalette NOTIFY paletteChanged)
    Q_PROPERTY(QString customPalette READ customPalette WRITE setCustomPalette NOTIFY customPaletteChanged)
    // clang-format on

private:
    explicit xToolsSettings(QObject *parent = nullptr);

public:
    static xToolsSettings *instance();
    Q_INVOKABLE void openSettingsFileDir();

    QString settingsPath();

    int hdpiPolicy();
    void setHdpiPolicy(int policy);

    int uiType();
    void setUiType(int type);

    QString appStyle();
    void setAppStyle(const QString &style);

    QString language();
    void setLanguage(const QString &lan);

    bool clearSettings();
    void setClearSettings(bool clear);

    int pageIndex();
    void setPageIndex(int index);

    bool isTextBesideIcon();
    void setIsTextBesideIcon(bool is);

    int palette();
    void setPalette(int p);

    QString customPalette();
    void setCustomPalette(const QString &p);

signals:
    void hdpiPolicyChanged();
    void uiTypeChanged();
    void clearSettingsChanged();
    void pageIndexChanged();
    void isTextBesideIconChanged();
    void paletteChanged();
    void customPaletteChanged();

public:
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);

private:
    struct
    {
        const QString hdpiPolicy{"hdpiPolicy"};
        const QString uiType{"uiType"};
        const QString appStyle{"appStyle"};
        const QString language{"language"};
        const QString clearSettings{"clearSettings"};
        const QString pageIndex{"pageIndex"};
        const QString isTextBesideIcon{"isTextBesideIcon"};
        const QString palette{"palette"};
        const QString customPalette{"customPalette"};
    } mSettingsKey;

    QLoggingCategory mLoggingCategory{"sak.settings"};
};
