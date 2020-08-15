#ifndef EXPORTBIBLE_H
#define EXPORTBIBLE_H

#include "ui_exportbible.h"
#include "databasehandler.h"

#include <QWidget>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QTextCodec>
#include <QFileInfo>
#include <QDesktopServices>

namespace Ui {
class ExportBible;
}

class ExportBible : public QWidget
{
    Q_OBJECT

public:
    void translate();
    void addToCb(const QString &abbr, const QString &desc);
    explicit ExportBible(QWidget *parent = nullptr);
    ~ExportBible();

private slots:
    void on_pb_folder_clicked();

    void on_pb_export_clicked();

    void on_pb_close_clicked();

private:
    QDir exportDir;
    QTextDocument *bibleDoc = new QTextDocument;
    QTextDocumentWriter writer;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    Ui::ExportBible *ui;

};

#endif // EXPORTBIBLE_H
