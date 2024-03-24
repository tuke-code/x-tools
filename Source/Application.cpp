﻿/***************************************************************************************************
 * Copyright 2018-2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "Application.h"

#include <QFile>
#include <QPushButton>
#include <QRect>
#include <QScreen>
#include <QSplashScreen>
#include <QTextCursor>
#include <QTranslator>

#include "xToolsDataStructure.h"
#include "MainWindow.h"
#include "xToolsSettings.h"
#include "xToolsTranslator.h"

#ifdef Q_OS_WIN
#include "SystemTrayIcon.h"
#endif

Application::Application(int argc, char** argv)
    : xToolsApplication(argc, argv)
{
    // It can avoid app crash in this way to show a splashScreen. If you create a QSplashScreen and
    // show it in the main function, app will crash(test on Ubuntu 16.04). Of course, it is because
    // that I use a wrong way, also, it could be a bug of Qt.
    QPixmap pixmap(":/Resources/Images/StartUi.jpg");
    m_splashScreen = new QSplashScreen(pixmap);
    showSplashScreenMessage(tr("Initializing..."));
    m_splashScreen->show();
    processEvents();

    // Palette
    int ret = xToolsSettings::instance()->palette();
    if ((ret == xToolsDataStructure::PaletteDark) || (ret == xToolsDataStructure::PaletteLight)) {
        QString fileName = (ret == xToolsDataStructure::PaletteLight
                                ? ":/Resources/Palettes/DarkPalette"
                                : ":/Resources/Palettes/DarkPalette");
        setupPalette(fileName);
    } else {
        QString customPalette = xToolsSettings::instance()->customPalette();
        if (customPalette.isEmpty()) {
            qInfo() << "Current palette is: system";
        } else {
            setupPalette(customPalette);
        }
    }

    // Setup ui language.
    QString language = xToolsSettings::instance()->language();
    xToolsTranslator::instance()->setupLanguage(language);
    showSplashScreenMessage(tr("Initializing main window..."));
    
    auto mainWindow = new MainWindow();
    m_splashScreen->finish(mainWindow);
    QObject::connect(this,
                     &Application::activeMainWindow,
                     mainWindow,
                     &MainWindow::activateWindow);
    mainWindow->show();

#ifdef Q_OS_WIN
    // Setup system tray icon.
    auto systemTrayIcon = new SystemTrayIcon(this);
    QObject::connect(systemTrayIcon, &SystemTrayIcon::invokeExit, this, [=]() {
        mainWindow->close();
    });
    QObject::connect(systemTrayIcon, &SystemTrayIcon::invokeShowMainWindow, this, [=]() {
        mainWindow->show();
    });
    systemTrayIcon->show();
#endif

    // Move the window to the screen central.
#ifndef Q_OS_ANDROID
    mainWindow->resize(int(double(mainWindow->height()) * 1.732), mainWindow->height());
#endif
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();
    bool tooWidth = (mainWindow->width() > screenRect.width());
    bool tooHeight = (mainWindow->height() > screenRect.height());
    if (tooWidth || tooHeight) {
        mainWindow->showMaximized();
        qInfo() << "The screen is too small.";
    } else {
        mainWindow->move((screenRect.width() - mainWindow->width()) / 2,
                         (screenRect.height() - mainWindow->height()) / 2);
    }
    showSplashScreenMessage(tr("Finished..."));

    QString msg = QString("The size of main window is: %1x%2")
                      .arg(mainWindow->width())
                      .arg(mainWindow->height());
    qInfo() << qPrintable(msg);
}

void Application::showSplashScreenMessage(const QString &msg)
{
    m_splashScreen->showMessage(msg, Qt::AlignBottom, QColor(255, 255, 255));
}

void Application::setupPalette(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QDataStream out(&file);
        QPalette p;
        out >> p;
        file.close();
        setPalette(p);
        qInfo() << "Current palette is:" << fileName;
    } else {
        qWarning() << "Open palette file error:" << file.errorString();
    }
}
