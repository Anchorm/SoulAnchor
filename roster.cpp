#include "roster.h"

Roster::Roster(QWidget *parent) :    QWidget(parent, Qt::Window),
    ui(new Ui::Roster)
{
    ui->setupUi(this);
    populateCb();
    ui->te_preview->document()->setDocumentMargin(20);
    this->setObjectName("RosterWindow");
}

Roster::~Roster()
{
    delete ui;
}

void Roster::on_pb_close_clicked()
{
    hide();
}

void Roster::hideEvent(QHideEvent *){
    ui->te_preview->clear();
}

void Roster::on_pb_make_clicked()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString tl = "t_" + settings.value("translation").toString();
    int iTempo = ui->cb_amount->currentText().toInt();
    QString sTempo = ui->cb_amount->currentText();
    QString what = ui->cb_what->currentData(0x0100).toString();
    QString genreName = ui->cb_what->currentText();
    QString rosterName = genreName + " - " + sTempo + " C/S";
    QVector<int> books;
    int chaptersTotal = 0;

    ui->te_preview->clear();

    QString sqlCheck = QString("SELECT name FROM sqlite_master "
                              "WHERE type='table' AND name='%1';").arg(rosterName);
    QSqlQuery queryCheck(sqlCheck, dbH.rosterDb);
    if (queryCheck.next()) {
        ui->te_preview->insertPlainText("Roster " + rosterName + " already exists, remove it first.");
        return;
    }

    ui->te_preview->insertPlainText("Creating roster: ");
    ui->te_preview->insertPlainText(rosterName);
    ui->te_preview->repaint();

    if(what == "all books" or what == "alle boeken"){
        for (int b = 1; b <= 66 ; b++ ) {
            books.append(b);
        }
    } else if(what == "O.T."){
        for (int b = 1; b <= 39 ; b++ ) {
            books.append(b);
        }
    } else if(what == "N.T."){
        for (int b = 40; b <= 66 ; b++ ) {
            books.append(b);
        }
    } else {
        QString where = "WHERE genre_nr = " + what ;
        QString sqlB = QString("SELECT book_nr FROM number_name %2").arg(where);
        QSqlQuery queryBk(sqlB, dbH.bibleDb);

        queryBk.exec();
        while (queryBk.next()) {
            books.append(queryBk.value(0).toInt() );
        }
    }

    //determine the amount of chapters and add QVector pairs of bookNr and chapNr to a QVector
    QVector< QVector<int> > bkCh;

    for (int bkNr : books) {
        QString sqlC = QString("SELECT c FROM %1 WHERE b = %2 "
                               "ORDER BY c DESC LIMIT 1").arg(tl).arg(bkNr);

        QSqlQuery queryC(sqlC, dbH.bibleDb);
        queryC.exec();
        int finalChap = 0;

        while (queryC.next()) {
            finalChap = queryC.value(0).toInt();
            chaptersTotal += finalChap;
        }

        for (int i = 1; i <= finalChap; i++ ) {
            bkCh.append( {bkNr, i} );
        }
    }

    //calculate sessions
    double sessions = chaptersTotal / iTempo;
    double remainder = chaptersTotal % iTempo;
    if (remainder > 0) {
        sessions += 1;
    }

    // add a table to roster.db

    QString sqlDrop = QString("DROP TABLE IF EXISTS '%1' ").arg(rosterName);
    QSqlQuery queryDrop(sqlDrop, dbH.rosterDb);
    queryDrop.exec();

    QString sqlCreate = QString("CREATE TABLE '%1' ("
                                "SESSION INTEGER NOT NULL,"
                                "BOOK INTEGER NOT NULL,"
                                "CHAPTER INTEGER NOT NULL,"
                                "CURRENT TEXT"
                                ");").arg(rosterName);

    QSqlQuery queryAdd(sqlCreate, dbH.rosterDb);
    queryAdd.exec();

    // insert session information, a row for every chapter in the session

    if(dbH.rosterDb.transaction()) {
        QSqlQuery insertQ(dbH.rosterDb);

        for (int s = 1; s <= sessions ; s++ ) {

            for (int c = 1; c <= iTempo; c++) {

                if (!bkCh.isEmpty()){
                    QVector<int> bC = bkCh.takeFirst();
                    QString sqlInsert = QString("INSERT INTO '%1' (SESSION, BOOK, CHAPTER) "
                                                "VALUES (%2, %3, %4);"
                                                ).arg(rosterName).arg(s).arg(bC[0]).arg(bC[1]);
                    insertQ.prepare(sqlInsert);
                    insertQ.exec();
                }
            }
        }

        QString sqlSet = QString("UPDATE '%1' SET CURRENT = 'true' "
                                 "WHERE SESSION = 1;").arg(rosterName);
        QSqlQuery setActive(sqlSet, dbH.rosterDb);
        setActive.exec();

       if(!dbH.rosterDb.commit()) {
            dbH.rosterDb.rollback();
            ui->te_preview->insertPlainText("\n\nFailed to commit changes "
                                            "to the roster.db database.");
       } else {
           emit rosterCreated();
           ui->te_preview->insertPlainText("\n\nReady. The reading plan can "
                                           "now be selected in the menu.");
       }

    } else {
        ui->te_preview->insertPlainText("\n\nFailed to start a transaction for "
                                        "the roster.db database.");
    }
}

void Roster::on_pb_preview_clicked()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString what = ui->cb_what->currentData(0x0100).toString();
    int tempo = ui->cb_amount->currentText().toInt();
    QVector<int> books;
    int chaptersTotal = 0;
    QString tl = "t_" + settings.value("translation").toString();

    // determine which book numbers
    if(what == "all books"){
        for (int b = 1; b <= 66 ; b++ ) {
            books.append(b);
        }
    } else if(what == "O.T."){
        for (int b = 1; b <= 39 ; b++ ) {
            books.append(b);
        }
    } else if(what == "N.T."){
        for (int b = 40; b <= 66 ; b++ ) {
            books.append(b);
        }
    } else {
        QString where = "WHERE genre_nr = " + what ;
        QString sqlB = QString("SELECT book_nr FROM number_name %2").arg(where);
        QSqlQuery queryBk(sqlB, dbH.bibleDb);
        queryBk.exec();
        while (queryBk.next()) {
            books.append(queryBk.value(0).toInt() );
        }
    }

    //determine the amount of chapters
    for (int bkNr : books) {
        QString sqlC = QString("SELECT c FROM %1 WHERE b = %2 "
                               "ORDER BY c DESC LIMIT 1").arg(tl).arg(bkNr);
        QSqlQuery queryC(sqlC, dbH.bibleDb);
        queryC.exec();
        while (queryC.next()) {
            chaptersTotal += queryC.value(0).toInt();
        }
    }

    //calculate sessions and months
    double sessions = chaptersTotal / tempo;
    double remainder = chaptersTotal % tempo;
    if(remainder > 0) {
        sessions += 1;
    }

    double months = sessions / 30;
    double monthsRounded = round (months * 10) / 10.0; // show one decimal

    ui->te_preview->clear();
    ui->te_preview->insertPlainText(QString("Number of books: %1 \n").arg(books.length()) );
    ui->te_preview->insertPlainText(QString("Number of chapters: %1 \n").arg(chaptersTotal) );
    ui->te_preview->insertPlainText(QString("Number of sessions: %1 \n\n").arg(sessions) );
    ui->te_preview->insertPlainText(
                QString("About %1 month(s) when reading one session a day.").arg(monthsRounded) );

}

void Roster::on_cb_what_currentTextChanged()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString what = ui->cb_what->currentData(0x0100).toString();
    QString lang = settings.value("bknLanguage").toString();
    if (lang.isEmpty()) lang = "english";

    if (what == "all books") {
        what = "book_nr between 1 and 66";
    } else if(what == "O.T.") {
        what = "book_nr between 1 and 39";
    } else if(what == "N.T.") {
        what = "book_nr between 40 and 66";
    } else {
        what = "genre_nr = " + what;
    }

    QSqlQuery query(dbH.bibleDb);
    QString sql = QString("SELECT name_%1 FROM number_name WHERE %2").arg(lang, what) ;

    ui->te_preview->clear();
    query.prepare(sql);
    query.exec();
    bool first = true;

    while (query.next()) {
        if(query.isValid() and not first) {
            ui->te_preview->insertPlainText(" - ");
        }
        ui->te_preview->insertPlainText(query.value(0).toString());
        first = false;
    }
}

void Roster::populateCb() {
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QSqlQuery query(dbH.bibleDb);
    query.prepare("SELECT genre_nr, name_english FROM genre_desc");

    ui->cb_what->addItem(tr("all books"), "all books");
    ui->cb_what->addItem(tr("O.T."), "O.T.");
    ui->cb_what->addItem(tr("N.T."), "N.T.");

    query.exec();
    while (query.next()) {
        ui->cb_what->addItem(query.value(1).toString(), query.value(0) );
    }
}

