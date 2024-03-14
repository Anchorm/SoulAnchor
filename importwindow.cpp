/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#include "importwindow.h"

ImportWindow::ImportWindow(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    setWindowTitle("Import Bible translation - SoulAnchor");
    setObjectName("ImportWindow");
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(200, 200);

    connect(selectBtn, &QPushButton::clicked, this, &ImportWindow::selectTranslation);
    connect(importBtn, &QPushButton::clicked, this, &ImportWindow::importTranslation);
    connect(removeBtn, &QPushButton::clicked, this, &ImportWindow::removeTranslation);

    infoTe->setText(defTxt);
    infoTe->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoTe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoTe->setReadOnly(true);

    delimLe->setMaxLength(1);
    delimLe->setMaximumWidth(30);
    delimLbl->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    selectBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    importBtn->setDisabled(true);

    layHbox->addWidget(selectBtn);
    layHbox->addWidget(delimLbl);
    layHbox->addWidget(delimLe);
    layHbox->addStretch();
    layHbox->addWidget(importBtn);

    layHbox2->addWidget(removeBtn);
    layHbox2->addStretch();

    layVbox->setContentsMargins(5,5,5,5);
    layVbox->setSpacing(10);
    layVbox->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    layVbox->addLayout(layHbox);
    layVbox->addWidget(infoTe);
    layVbox->addLayout(layHbox2);

    setLayout(layVbox);
}

void ImportWindow::selectTranslation()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile); // select only one file
    dialog.setNameFilter(tr("Delimited text file *.csv *.txt (*.csv *.txt)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setDirectory(QDir::homePath());

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.empty()) {
        infoTe->setText(defTxt);
        return;
    }

    tlFile->setFileName(fileNames.first());

    if (!tlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        infoTe->setText(tr("could not open file: ") + tlFile->fileName());
        return;
    }

    QFileInfo tlFileInfo(tlFile->fileName());
    QStringList abbrLang = tlFileInfo.baseName().split("-");
    if (abbrLang.count() != 2) {
        infoTe->setText(tr("wrong filename: ") + tlFile->fileName());
        return;
    }

    abbreviation = abbrLang.at(0).toLower();
    language = abbrLang.at(1).toLower();
    infoTe->setText(tr("Selected file: ") + tlFile->fileName());

    QTextStream in(tlFile);
    name = in.readLine(); // header with name

    if (!abbreviation.isEmpty() && !language.isEmpty() && !name.isEmpty()) {
        infoTe->append("\n Abbreviation: " + abbreviation + "\n Language: " + language + "\n Name: " + name);
    } else {
        infoTe->append("\n Need abbreviation, language, name \n eg KJV, en, King James Version");
        if (tlFile != nullptr && tlFile->isOpen())
            tlFile->close();
        return;
    }

    QStringList line;
    QString b, c, v, t;
    QString delimiter = delimLe->text();
    int count = 0;
    infoTe->append("\nExample lines:\n");

    while (!in.atEnd()) {
        line = in.readLine().split(delimiter);
        if (line.count() == 4) {
            b = line[0];
            c = line[1];
            v = line[2];
            t = line[3];
            infoTe->append(b + " " + c + " " + v + " " + t);
        } else {
            infoTe->append("error: unexpected number of fields");
            importBtn->setDisabled(true);
            break;
        }
        count += 1;
        if (count == 3) {
            infoTe->append("\nYou should see the first 3 lines without the delimiter character."
                           "\nIn the format number number number text."
                           "\nIf everything looks alright press the import button.");
            importBtn->setEnabled(true);
            break;
        }
    }

    if (tlFile != nullptr && tlFile->isOpen())
        tlFile->close();
}

void ImportWindow::importTranslation()
{
    infoTe->clear();

    if(bibleDb.transaction()) {
        infoTe->setText("working...");

        QSqlQuery addVersionInfo(bibleDb);
        addVersionInfo.prepare("INSERT INTO version_info (tablename, abbreviation, language, version) "
                               "VALUES (:tablename, :abbr, :lang , :version);");
        addVersionInfo.bindValue(":tablename", "t_" + abbreviation);
        addVersionInfo.bindValue(":abbr", abbreviation);
        addVersionInfo.bindValue(":lang", language);
        addVersionInfo.bindValue(":version", name);

        if (!addVersionInfo.exec()) {
            infoTe->append("\nFailed to insert into version_info. Error msg: " + addVersionInfo.lastError().text());
            bibleDb.rollback();
            return;
        }

        QSqlQuery queryAddTable(bibleDb);
        queryAddTable.prepare(QString("CREATE TABLE IF NOT EXISTS 't_%1' "
                            "(b INTEGER NOT NULL, "
                            "c INTEGER NOT NULL, "
                            "v INTEGER NOT NULL, "
                            "t TEXT); ").arg(abbreviation));

        if (!queryAddTable.exec()) {
            infoTe->append("\nFailed to create the new table. Error msg: " + queryAddTable.lastError().text());
            bibleDb.rollback();
            return;
        }

        QSqlQuery queryAddTl(bibleDb);
        queryAddTl.prepare("INSERT INTO t_" + abbreviation + " (b, c, v, t) "
                            "VALUES (:bookNr, :chapterNr, :verseNr, :text);");

        if (tlFile != nullptr && !tlFile->open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(tlFile);
        in.readLine(); // skip header
        QStringList line;
        QString b, c, v, t;
        QString delimiter = delimLe->text();

        while (!in.atEnd()) {
            line = in.readLine().split(delimiter);
            b = line[0];
            c = line[1];
            v = line[2];
            t = line[3];

            queryAddTl.bindValue(":bookNr", b);
            queryAddTl.bindValue(":chapterNr", c);
            queryAddTl.bindValue(":verseNr", v);
            queryAddTl.bindValue(":text", t);

            if (!queryAddTl.exec()) {
                infoTe->append("\nFailed to insert row. Error msg: " + queryAddTl.lastError().text());
                bibleDb.rollback();
                if (tlFile != nullptr && tlFile->isOpen())
                    tlFile->close();
                return;
            }
        }

        if(!bibleDb.commit()) {
            bibleDb.rollback();
            infoTe->append("\nFailed to commit changes to the bibles.db database.");
        } else {
            // let's see how many rows are inserted
            QSqlQuery queryCount(bibleDb);
            queryCount.prepare("SELECT COUNT(*) FROM t_" + abbreviation);
            if (!queryCount.exec()) {
                infoTe->append("Error executing query to count the number of rows");
                infoTe->append(queryCount.lastError().text());
            } else {
                if (queryCount.next()) {
                    QString rowCount = queryCount.value(0).toString();
                    infoTe->append(rowCount + " rows inserted");
                } else {
                    infoTe->append("count not count the number of rows inserted...");
                }
            }
            infoTe->append("\nTranslation added to the database."
                           "\nRestart SoulAnchor to use the new translation.");
        }
    // end transaction
    }  else {
        infoTe->append("\nFailed sqlite3 transaction for the bibles.db database.");
    }

    if (tlFile != nullptr && tlFile->isOpen())
        tlFile->close();

    importBtn->setDisabled(true);
}

void ImportWindow::removeTranslation()
{
    QStringList translations;
    QSqlQuery getTls("SELECT name FROM sqlite_master WHERE type = 'table' "
                     "and name LIKE 't_%'", bibleDb);

    while (getTls.next()) {
        QString tl = getTls.value(0).toString().toLower();
        tl.remove(0,2);
        // net bible is the default/fallback
        if (tl != "net")
            translations.append(tl);
    }
    bool ok = false;
    QString tlName = QInputDialog::getItem(this, tr("remove translation"),
                                                 tr("translation to remove:"), translations, 0, false, &ok);

    if (ok && !tlName.isEmpty() ) {
        if(bibleDb.transaction()) {

            QSqlQuery dropQuery(bibleDb);
            dropQuery.prepare("DROP TABLE t_" + tlName.toLower());
            if (!dropQuery.exec()) {
                infoTe->append("\nError could not drop table ");
                infoTe->append(dropQuery.lastError().text());
                bibleDb.rollback();
                return;
            }

            QSqlQuery delVersionInfo(bibleDb);
            delVersionInfo.prepare("DELETE FROM 'version_info' WHERE tablename = 't_" + tlName + "'");
            if (!delVersionInfo.exec()) {
                infoTe->append("\nError could not remove row in version_info:");
                infoTe->append(delVersionInfo.lastError().text());
                bibleDb.rollback();
                return;
            }

            if(!bibleDb.commit()) {
                bibleDb.rollback();
                infoTe->append("\nFailed to commit changes to the bibles.db database.");
                return;
            }
            infoTe->setText(tlName + " has been removed from the database. \nPlease restart SoulAnchor.");
            // end transaction
        } else {
            infoTe->append("\nFailed SQLite transaction for the bibles.db database.");
        }
    }   
}

void ImportWindow::centerWindow()
{
    if (parentWidget()) {
        QScreen* activeScreen = parentWidget()->screen();
        if (activeScreen != nullptr) {
            int width = (parentWidget()->width() / 100) * 60;
            int height = (parentWidget()->height() / 100) * 60;
            resize(width, height);

            auto winGeo = frameGeometry();
            auto parentGeoCenter = parentWidget()->geometry().center();
            winGeo.moveCenter(parentGeoCenter);
            move(winGeo.topLeft());
        }
    }
}

void ImportWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        selectBtn->setText(tr("select csv..."));
        importBtn->setText(tr("import"));
        removeBtn->setText(tr("remove translation from database..."));
        delimLbl->setText(tr("delimiter character: "));
        selectBtn->setText(tr("select csv..."));
    }
    event->ignore();
}
