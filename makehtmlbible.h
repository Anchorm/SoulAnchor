#ifndef MAKEHTMLBIBLE_H
#define MAKEHTMLBIBLE_H

#include "ui_makehtmlbible.h"
#include "databasehandler.h"
#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

namespace Ui {
class MakeHtmlBible;
}

class MakeHtmlBible : public QWidget
{
    Q_OBJECT

public:
    void addToCb(const QString &abbr, const QString &desc);
    void translate();
    explicit MakeHtmlBible(QWidget *parent = nullptr);
    ~MakeHtmlBible();

private slots:
    void on_pb_close_clicked();
    void on_pb_folder_clicked();
    void on_pb_make_clicked();
    void makeLinks();
    void makeBooks(const QString &abbr, QString desc, const QString &tlLanguage, const QString &table);
    void makeIndex();

private:
    QDir htmlDir;
    QVector<QString> link;
    QVector< QVector<QString> > links;
    Ui::MakeHtmlBible *ui;
};

#endif // MAKEHTMLBIBLE_H
