#include "makehtmlbible.h"

MakeHtmlBible::MakeHtmlBible(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::MakeHtmlBible)
{
    ui->setupUi(this);

    htmlDir.setPath(::userDataDir.path() + "/html-bible");
    if (!htmlDir.exists()) {

        if (htmlDir.mkpath(".")) {
            ::sout << htmlDir.path() << " created" << Qt::endl;
        } else {
            ::sout << "Cannot find/create writable location for html bible data." << Qt::endl;
            ::sout << "E.g. $HOME/.local/share/soulanchor/html-bible" << Qt::endl;
        }
    }
    QString expl = QString(tr("Choose a translation and click create.\n"
                   "A new folder will be placed in %1 .\n"
                   "But only if no other folder with the same name exists.")).arg(htmlDir.path());
    ui->te->setText(expl);
}

MakeHtmlBible::~MakeHtmlBible()
{
    delete ui;
}

void MakeHtmlBible::translate()
{
    ui->retranslateUi(this);
}

void MakeHtmlBible::on_pb_close_clicked()
{
    hide();
    ui->cb_tl->setCurrentIndex(0);
    ui->te->clear();
}

void MakeHtmlBible::on_pb_folder_clicked()
{
    QDesktopServices::openUrl(htmlDir.absolutePath());
}

void MakeHtmlBible::addToCb(const QString &abbr, const QString &desc) {
    ui->cb_tl->addItem(desc, abbr);
}

void MakeHtmlBible::on_pb_make_clicked()
{
    ui->te->setText("working..."); ui->te->repaint();
    QString abbr = ui->cb_tl->currentData().toString();
    QString desc = ui->cb_tl->currentText();
    QString table = "t_" + abbr;
    QString tlLanguage;

    QString sql = QString("SELECT abbreviation, language FROM bible_version_key WHERE "
                          "abbreviation = '%1'").arg(abbr);
    QSqlQuery query(sql, dbH.bibleDb);

    while (query.next()) {
        tlLanguage = query.value(1).toString();
    }

    QDir abbrDir = htmlDir.path() + "/" + abbr;
    qDebug() << abbrDir.path();

    if(abbrDir.exists()) {
        ui->te->setText(abbrDir.path() + " exists. Canceling operation.");
        return;
    } else {
        if (abbrDir.mkpath(".") ) {
            ui->te->setText(abbrDir.path() + " created.");
        } else {
            ui->te->setText("Could not create " + abbrDir.path());
            return;
        }
    }

    links.clear();
    makeLinks();
    makeBooks(abbr, desc, tlLanguage, table);
    makeIndex();

    QFile bstyle(":skel/bstyle.css");
    bstyle.copy(QString(htmlDir.path() + "/%1/bstyle.css").arg(abbr) );
    QFile bscript(":skel/bscript.js");
    bscript.copy(QString(htmlDir.path() + "/%1/bscript.js").arg(abbr) );
    QFile anchor(":img/anchor_hope.jpg");
    anchor.copy(QString(htmlDir.path()+ "/%1/anchor_hope.jpg").arg(abbr) );
    QFile bpic(":/img/stairway.jpg");
    bpic.copy(QString(htmlDir.path() + "/%1/stairway.jpg").arg(abbr) );

    ui->te->setText(tr("done"));
}

void MakeHtmlBible::makeIndex(){
    htmlDir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QVector<QString> dirList;

    for (auto &dir : htmlDir.entryList() ) {
        dirList.append(dir);
    }

    htmlDir.setFilter(QDir::Files);
    QStringList filters;
    filters << "*.7z" << "*.zip";
    htmlDir.setNameFilters(filters);
    QVector<QString> zipList;

    for (QString &zipFile : htmlDir.entryList()) {
        zipList.append(zipFile);
    }

    QFile indexFile(htmlDir.path() + "/index.html");
    if(indexFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        QTextStream out(&indexFile);
        out.setCodec("UTF-8");
        out << "<!DOCTYPE html>\n<html><head>\n"
                "<meta charset='UTF-8'>\n"
                "<meta name='description' content='html bibles'>\n"
                "<meta name='keywords' content='HTML,BIBLES,FREE'>\n"
                "<meta name='author' content='Anchorman'>\n"
                "<meta name='generator' content='SoulAnchor - X11 bible reader'>\n"
                "<title>html bibles index</title>\n"

               "<style>\n"
               "body {margin-top:20px;}\n"
               "div {color:#404c57;}\n"
               "#center {max-width:800px;margin-left:auto;margin-right:auto;}\n"
               "a {text-decoration:none;}\n"
               "li {padding-bottom:3px; list-style-type:square;}\n"
               "#bibles {float:left; margin-right:20px;}\n"
               "#image {float:left;}\n"
               "#downloads {max-width:450px;}\n"
               "#downloads li {display:inline; padding-right:10px;}\n"
               "</style>\n"
               "</head>\n";
        out << "<body>\n<div id='center'>\n";

        out << "<div id='bibles'>\n";
        out << "<h2>HTML BIBLES:</h2>\n<ul>\n";

        for (QString &tl : dirList) {
            out << QString("<li><a href='%1/1_Genesis.html'>%1</a></li>\n").arg(tl);
        }
        out << "</ul></div>\n";

        out << "<div id='image'>\n<img src='anchor_hope.png'><br>\n";
        out << "<h3>downloads:</h3>\n<ul id='downloads'>\n";

        for (QString &zfile : zipList) {
            out << QString("<li><a href='%1'>%1</a></li>\n").arg(zfile);
        }

        out << "</ul></div>\n</div></body>\n</html>";
        indexFile.close();

        QFile anchorImg(":img/anchor_about.png");
        anchorImg.copy(htmlDir.path() + "/anchor_hope.png");
    } else {
        ui->te->append(tr("could not create %1/html-bible/index.html").arg(htmlDir.path()));
    }
}

void MakeHtmlBible::makeLinks(){
    QString bookname;
    QString bookFilename;

    // for each book
    for (int i = 1; i < 67; i++ ) {
        bookname = ::g_bookNames[i];
        bookFilename = QString("%1_%2.html").arg(i).arg(bookname);
        bookFilename.replace(" ", "-");

        // for every book a link is added to the links QVector for the book index
        link.clear();
        link.append(bookname);
        link.append(QString("<a id='bk' href='%1'>%2</a>").arg(bookFilename, bookname) ) ;
        links.append(link);
    }
}

void MakeHtmlBible::makeBooks(const QString &abbr, QString desc, const QString &tlLanguage, const QString &table)
{
    int chapters;
    QString sqlGBN; // get bookname
    QSqlQuery queryGBN(dbH.bibleDb);
    QString sqlGS; // get scriptures
    QSqlQuery queryGS(dbH.bibleDb);

    QString bookname;
    QString bookFilename;
    QFile bookFile;

    // for each book
    for (int i = 1; i < 67; i++ )
    {
        chapters = dbH.getFinalChapter(i, abbr);
        sqlGBN = QString("SELECT book_nr, name_%1 from number_name "
                        "where book_nr = %2").arg(tlLanguage).arg(i);
        queryGBN.prepare(sqlGBN);

        sqlGS = QString("SELECT c,v,t FROM '%1' where b = '%2'").arg(table).arg(i);
        queryGS.prepare(sqlGS);
        QString c; QString v; QString t;

        queryGBN.exec();

        while (queryGBN.next())
        {
            bookname = queryGBN.value(1).toString();
            bookFilename = QString(htmlDir.path()
                                   + "/%1/%2_%3.html").arg(abbr).arg(i).arg(bookname);
            bookFilename.replace(" ", "-");

            bookFile.setFileName(bookFilename);
            bookFile.open(QIODevice::WriteOnly | QIODevice::Text);

            QTextStream out(&bookFile);
            out.setCodec("UTF-8");

            out << "<!DOCTYPE html>\n<html><head>\n"
                    "<meta charset='UTF-8'>\n"
                    "<meta name='description' content='html bible'>\n"
                    "<meta name='keywords' content='HTML,BIBLE,FREE'>\n"
                    "<meta name='author' content='Anchorman'>\n"
                    "<meta name='generator' content='SoulAnchor - "
                    "X11 bible reader'>\n"
                    "<link rel='stylesheet' type='text/css' href='bstyle.css'>\n"
                    "<script src='bscript.js'></script>\n"
                    "<title>";
            out << bookname;
            out << "</title></head>\n\n";
            out << "<body onload='start()'>\n";

            out << "<noscript><p style='text-align:center; font-size:90%;'>"
                   "(Without JavaScript the following will not work: "
                   "button/info, auto resize books list, "
                   "jump/scroll to active book)</p></noscript>\n\n";

            // left side, navigation pane
            out << "<div id='nav'>\n";

            // aboutpage info
            desc.replace("\'","\\x27"); // javascript single quote
            out << QString("<button onClick=\"genInfo('%1', '%2')\">%2</button>\n")
                   .arg(desc, abbr);

            // books index
            out << "<ul id='books_list'>\n";

            int count = 0;
            for (QVector<QString> &li : links) {
                count++;
                if(count == 40){
                    out << "<br>\n";
                }
                if(li[0] == bookname){
                    out << QString("<li id='active_book'>%1</li>\n").arg(li[1]);
                } else {
                    out << QString("<li>%1</li>\n").arg(li[1]);
                }
            }

            out << "</ul>\n";

            // chapter index
            out << "<ul id='chapters_list'>\n";
            for(int chap = 1; chap < chapters + 1; chap++){
                out << QString("<li><a href='#%1' id='chap'>%1</a></li>\n").arg(chap);
            }
            out << "</ul>\n";

            out << "</div>\n\n";

            // right side, content pane
            out << "<div id='content'>\n";

            // create a table for every chapter
            bookname = queryGBN.value(1).toString();
            queryGS.exec();
            while (queryGS.next())
            {
                c = queryGS.value(0).toString();
                v = queryGS.value(1).toString();
                t = queryGS.value(2).toString();

                if(c == "1" && v == "1"){
                    out << "<p id='booktitle'>" << bookname << "</p>";
                    out << "<table>";
                    out << "<thead><tr><th colspan='2' id='" << c << "'>" <<
                           bookname << " " << c <<"</th></tr></thead>\n";
                } else if (v == "1") {
                    out << "</table><br>\n\n";
                    out << "<table>";
                    out << "<thead><tr><th colspan='2' id='" << c << "'>" <<
                           bookname << " " << c <<"</th></tr></thead>\n";
                }

                out << "<tr>";

                out << "<td id='vnr'>";
                out << v;
                out << "</td>";

                out << "<td id='txt'>";
                out << t;
                out << "</td>";

                out << "</tr>\n";
            }

            // end content div
            out << "</table></div>\n\n";

            out << "<br></body></html>";
            bookFile.close();
        }
    }
}
