#include "aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setWindowTitle("About Window - SoulAnchor");
    this->setObjectName("AboutWindow");
    setWindowIcon(anchorIcon);
    setFixedSize(500, 500);

    lblAnchor->setObjectName("lblAnchor");
    lblAnchor->setStyleSheet("border-image: url(:/data/img/anchor_about.png)");
    lblAnchor->setFixedHeight(230);
    lblInfo->setText("<h1>SoulAnchor</h1>"
                "X11 Bible tool - version " + QGuiApplication::applicationVersion() + "<br>"
    );
    lblInfo->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lblInfo->setStyleSheet("padding:5px");

    amPb->setText("Anchorman in the year of our Lord 2021");
    amPb->setObjectName("pbAut");
    amPb->setCursor(Qt::PointingHandCursor);
    amPb->setToolTip("open e-mail client");

    connect(amPb, &QPushButton::clicked, this, [](){
        QDesktopServices::openUrl(
                    QUrl("mailto:soulanchor@protonmail.com", QUrl::TolerantMode)); });

    QString cred = "<table cellpadding='5'>"
            "<tr><td>the Holy Scriptures</td> <td>God Almighty</td></tr>"
            "<tr><td>translations</td> <td>most are Public domain, database based on: "
            "https://github.com/scrollmapper/bible_databases</td></tr>"
            "<tr><td>the NET Bible</td> <td>Scripture quoted by permission. "
            "Quotations designated (NET) are from the NET Bible® copyright ©1996, "
            "2019 by Biblical Studies Press, L.L.C. http://netbible.com All rights reserved</td>"
            "</tr>"
            "<tr><td>dictionaries, strongs</td> <td>Public domain, various unknown contributors, "
            "thank you all</td></tr>"
            "<tr><td>Qt 5.15</td> <td>The Qt Toolkit is Copyright (C) 2018 The Qt Company Ltd. "
            "and other contributors. And used under the conditions of the LGPLv3.</td></tr>"
            "<tr><td>cross references & topical references</td>"
                "<td>https://www.openbible.info/</td></tr>"
            "<tr><td>fatcow icons</td><td>https://www.fatcow.com/free-icons</td></tr>"
            "<tr><td>famfamfam icons</td><td>http://www.famfamfam.com/</td></tr>"
            "<tr><td>Images</td> <td>as found on the net and/or modified or created</td></tr>"
            "<tr></tr>"
            "</table>";

    teCredit->setStyleSheet("border:0; margin:0;");
    teCredit->setReadOnly(true);
    teCredit->setText(cred);

    lblLic->setText("\nSoulAnchor uses the GPLv3. See the COPYING file. \n\nThis program is distributed in the hope that it will be useful, but without any warranty; without even the implied warranty of merchantability or fitness for a particular purpose.");
    lblLic->setStyleSheet("padding:10px;border:0");
    lblLic->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lblLic->setWordWrap(true);

    aboutVbox->addWidget(lblInfo);
    aboutVbox->addWidget(lblAnchor);
    aboutVbox->addWidget(amPb);

    creditVbox->addWidget(teCredit);
    licenseVbox->addWidget(lblLic);

    aboutFrame->setLayout(aboutVbox);
    creditFrame->setLayout(creditVbox);
    licenseFrame->setLayout(licenseVbox);

    tabw->addTab(aboutFrame, "SoulAnchor");
    tabw->addTab(creditFrame, "Credits");
    tabw->addTab(licenseFrame, "License");

    mainVbox->setContentsMargins(0, 0, 0, 0);
    aboutVbox->setContentsMargins(0, 0, 0, 0);
    creditVbox->setContentsMargins(0, 0, 0, 0);
    licenseVbox->setContentsMargins(0, 0, 0, 0);

    mainVbox->addWidget(tabw);
    setLayout(mainVbox);
}

void AboutWindow::centerWindow(){
    if (parentWidget())
    {
        QScreen* activeScreen = parentWidget()->screen();
        if (activeScreen != nullptr)
        {
            auto winGeo = frameGeometry();
            auto parentGeoCenter = parentWidget()->geometry().center();
            winGeo.moveCenter(parentGeoCenter);
            move(winGeo.topLeft());
        }
    }
}
