#ifndef IMPORTWINDOW_H
#define IMPORTWINDOW_H

#include "globals.h"
#include "databasehandler.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QFileDialog>
#include <QDir>
#include <QLineEdit>
#include <QLabel>
#include <QInputDialog>

class ImportWindow : public QWidget
{
    Q_OBJECT

    DatabaseHandler& dbH = DatabaseHandler::getInstance();
    QSqlDatabase& bibleDb = dbH.getDatabase("bibles");

    QPushButton *selectBtn = new QPushButton(tr("select csv..."), this);
    QPushButton *importBtn = new QPushButton(tr("import"), this);
    QPushButton *removeBtn = new QPushButton(tr("remove translation from database..."), this);
    QTextEdit *infoTe = new QTextEdit(this);
    QLabel *delimLbl = new QLabel(tr("delimiter character: "), this);
    QLineEdit *delimLe = new QLineEdit("^", this);

    QHBoxLayout *layHbox = new QHBoxLayout();
    QHBoxLayout *layHbox2 = new QHBoxLayout();
    QVBoxLayout *layVbox = new QVBoxLayout();

    void selectTranslation();
    void importTranslation();
    void removeTranslation();

    QFile *tlFile = new QFile(nullptr);
    QString abbreviation;
    QString language;
    QString name;
    const QString defTxt = "Select a csv file to import a Bible translation. "
                           "Expected filename format: net-en.csv "
                     "It should have these fields: "
                     "<ul><li>book number</li>"
                     "<li>chapter number</li>"
                     "<li>verse number</li>"
                     "<li>verse text</li></ul>"
                    "The first line should be a header with the name of the translation. "
                    "SoulAnchor expects a Left To Right Language.";

protected:
    void changeEvent(QEvent *event) override;

public:
    void centerWindow();
    explicit ImportWindow(QWidget *parent = nullptr);

signals:
};

#endif // IMPORTWINDOW_H
