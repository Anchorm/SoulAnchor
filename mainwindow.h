/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "databasehandler.h"
#include "parwindow.h"
#include "aboutwindow.h"
#include "settingswindow.h"
#include "roster.h"
#include "globals.h"

#include <QMainWindow>
#include <QSettings>
#include <QQueue>
#include <QList>
#include <QLabel>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTranslator>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QShortcut>
#include <QGraphicsDropShadowEffect>
#include <QCursor>
#include <QIcon>
#include <QScreen>
#include <QRegularExpression>
#include <QUrl>
#include <QFlags>
#include <QTextDocument>
#include <QTextCursor>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QDesktopServices>
#include <QActionGroup>
#include <random>
#include <QResizeEvent>


//QtDocs: The following lines declare the  class(es) in the Ui namespace, which is the standard namespace for the UI classes generated from .ui files by the uic tool:
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT // macro for moc, meta object compiler, generate code for signals and stuff...

    QString tab; // active tab
    QString startup; // start with scriptures?

    int docMargin; // margin around scriptures
    int frameWidth; // max width of bible frame

    QString guiLanguage;
    QString bknLanguage; // booknames

    bool rosterRead;
    // roster has been read; checkbox should be checked; conf rosterRead should be false

    QString scripDisplay; // table format or book format
    QString strongTl; // translation used for Strongs

    QString saStyle; // css

    // for showing a devotion or hymn text
    const QString devStyle = "text-align:left;margin:10px;font-size:large;";

    // for 'reset' to make sure no weird format exists after anchor click
    const QTextCharFormat emptyFormat;
    // for highlighting matches after a find in page action
    QTextCharFormat matchFormat;

    QTextBrowser *textBrowser; // for find in page, switch between bible / info / strongs
    QStringList strongsHistory; // for context menu history
    QStringList dictwordHistory; // same

    const QIcon bookIcon = QIcon(":/data/img/book.png");
    const QIcon bookAddIcon = QIcon(":/data/img/book_add.png");
    const QIcon bookRemoveIcon = QIcon(":/data/img/book_remove.png");
    const QIcon bookmarkIcon = QIcon(":/data/img/bookmark.png");
    const QIcon bookOpenIcon = QIcon(":/data/img/book_open.png");
    const QIcon nextIcon = QIcon(":/data/img/bullet_arrow_right.png");
    const QIcon prevIcon = QIcon(":/data/img/bullet_arrow_left.png");
    const QIcon selectIcon = QIcon(":/data/img/edit-select-all");
    const QIcon copyIcon = QIcon(":/data/img/edit-copy");
    const QIcon findIcon = QIcon(":/data/img/edit-find");
    const QIcon closeIcon = QIcon(":/data/img/window-close.png");
    const QIcon musIcon = QIcon(":/data/img/music.png");
    const QIcon docIcon = QIcon(":/data/img/document_notes.png");
    const QIcon playIcon = QIcon(":/data/img/control_play_blue.png");
    const QIcon stopIcon = QIcon(":/data/img/control_stop_blue.png");
    const QIcon strongIcon = QIcon(":/data/img/biceps.png");
    const QIcon scrollIcon = QIcon(":/data/img/script_yellow.png");

    QQueue< QHash<QString, int> > printQ; // print queue, hash keys: bk c1 c2 v1 v2
    QQueue< QHash<QString, int> > printHistory; // keep a history of print jobs

    QMenu *otMenu = new QMenu("OT", this);
    QMenu *ntMenu = new QMenu("NT", this);

      /*****************
     * W I N D O W S  *
    *****************/
    ParWindow *parW = new ParWindow(this);
    AboutWindow *aboutW = new AboutWindow(this);
    SettingsWindow *settingsW = new SettingsWindow(this);
    Roster *rosterW = new Roster(this);

    void popupMsg(const QString message);
    void printMsg(const QString message);

    QList< QList<int> > encScrip = {
    {19,4,6},
    {19,119,10},{19,119,12},{19,119,27},{19,119,29},{19,119,30},{19,119,32},{19,119,33},{19,119,34},
    {19,119,35},{19,119,104},{19,119,105},{19,119,106},{19,119,133},{19,119,135},{19,119,143},{19,119,144},
    {19,119,160},{19,119,163},{19,119,165},{19,119,166},{19,119,167},
    {20,18,10},
    {21,12,13},
    {46,3,13},{46,3,16},{46,3,23},
    {47,6,17},{47,7,1},
    {48,5,24},{48,6,2},{48,6,7},{48,6,8},{48,6,9},
    {51,1,13},{51,3,1},{51,3,2},
    {58,6,19},
    {62,2,4},{62,2,5},{62,2,15},{62,2,16},{62,2,17},{62,2,23},{62,2,24},{62,2,25}
    };

    QLabel *encTxtLbl = new QLabel(this);
    QLabel *encPicLbl = new QLabel(this);
    QStringList encPics;

    QString dictIndex;

    QFlags<QTextDocument::FindFlag> findInPageflags;

    QMediaPlayer *mplayer = new QMediaPlayer(this);
    QAudioOutput *audioOutput = new QAudioOutput(this);
    QList< QList<QString> > musicList;

    QFont *scripFont = new QFont();
    QActionGroup *rosterGroup = new QActionGroup(this);

    bool iFilter; // immersion filter
    bool jFilter; // judeans filter
    // {"de doper", "de dompelaar"}, {"baptist", "immerser"}
    const QHash<QString, QString> immersionDict =
    {
        {"dopen", "onderdompelen"},
        {"gedoopt", "ondergedompeld"},
        {"doopt", "dompelt"},
        {"doopte", "dompelde"},
        {"de doop", "de onderdompeling"},
        {"de dopen", "het onderdompelen"},
        {"der dopen", "het onderdompelen"},
        {"ik doop", "ik dompel"},
        {"een doop", "een onderdompeling"},
        {"zijn doop", "zijn onderdompeling"},

        {"baptism", "immersion"},
        {"baptisms", "immersions"},
        {"baptised", "immersed"},
        {"baptized", "immersed"},
        {"baptize", "immerse"},
        {"baptise", "immerse"},
        {"baptizer", "immerser"}
    };

    const QHash<QString, QString> judeansDict =
    {
        {"joden", "Judeeërs"},
        {"jood", "Judeeër"},
        {"joods", "Judees"},
        {"joodse", "Judese"},
        {"jodin", "Judese"},
        {"het jodendom", "het Judaïsme"},

        {"jews", "Judeans"},
        {"jew", "Judean"},
        {"jewish", "Judean"},
        {"jewess", "Judean"}
    };

    // for cross references
    const QHash<QString, QString> crossrefDict =
    {
        {"1", "Gen"},{"2", "Exod"},{"3", "Lev"},{"4", "Num"},{"5", "Deut"},
        {"6", "Josh"},{"7", "Judg"},{"8", "Ruth"},{"9", "1Sam"},{"10", "2Sam"},
        {"11", "1Kgs"},{"12", "2Kgs"},{"13", "1Chr"},{"14", "2Chr"},{"15", "Ezra"},
        {"16", "Neh"},{"17", "Esth"},{"18", "Job"},{"19", "Ps"},{"20", "Prov"},
        {"21", "Eccl"},{"22", "Song"},{"23", "Isa"},{"24", "Jer"},{"25", "Lam"},
        {"26", "Ezek"},{"27", "Dan"},{"28", "Hos"},{"29", "Joel"},{"30", "Amos"},
        {"31", "Obad"},{"32", "Jonah"},{"33", "Mic"},{"34", "Nah"},{"35", "Hab"},
        {"36", "Zeph"},{"37", "Hag"},{"38", "Zech"},{"39", "Mal"},{"40", "Matt"},
        {"41", "Mark"},{"42", "Luke"},{"43", "John"},{"44", "Acts"},{"45", "Rom"},
        {"46", "1Cor"},{"47", "2Cor"},{"48", "Gal"},{"49", "Eph"},{"50", "Phil"},
        {"51", "Col"},{"52", "1Thess"},{"53", "2Thess"},{"54", "1Tim"},{"55", "2Tim"},
        {"56", "Titus"},{"57", "Phlm"},{"58", "Heb"},{"59", "Jas"},{"60", "1Pet"},
        {"61", "2Pet"},{"62", "1John"},{"63", "2John"},{"64", "3John"},{"65", "Jude"},
        {"66", "Rev"},
    };

    // topical Bible links
    QString topicalIndex;

    // active color theme
    QString activeScheme;
    QHash<QString, QString> scheme = {
        {"nrClr", "none"},
        {"txtClr", "none"},
        {"titleClr", "none"},
        {"bgClr", "none"},
        {"bg2Clr", "none"},
        {"clashClr", "none"}
        };

    // regular expressions
    const QString *strongPattern = new QString(
            "(?<strongStartTag><S>)"
            "\\s*(?<strongNr>\\d+)"
            "\\s*(?<strongEndTag></S>)");
    const QRegularExpression *strongRegex = new QRegularExpression(*strongPattern);

    const QString *strongPattern2 = new QString(
            "\\s*"
            "[gGhH]"
            "\\d{1,4}"
            "\\s*");
    const QRegularExpression *getStrongRegex = new QRegularExpression(*strongPattern2);

    QRegularExpression *audioBibleRegex = new QRegularExpression();

    const QString *twotPattern = new QString(
            "\\s*"
            "\\d{1,4}"
            "\\s*");
    const QRegularExpression *twotRegex = new QRegularExpression(*twotPattern);

    const QString *psalmPattern = new QString(
            "\\s*(?<ps>[pP][sS][aA][lL][mM])"
            "\\s*(?<ch>\\d{1,3})");

    const QString *hymnPattern = new QString("(?<Hymn>hymns)");

    const QString *scripPattern = new QString(
            "\\s*(?<prt>[1-3]?)"
            "\\s*(?<bk>[a-zA-Zëüï]{2,30})"
            "\\s*(?<ch1>\\d{0,3})"
            "-?(?<ch2>\\d{0,3})"
            ":?(?<vs1>\\d{0,3})"
            "-?(?<vs2>\\d{0,3})");

    const QRegularExpression *psalmRegex = new QRegularExpression(*psalmPattern);
    const QRegularExpression *hymnRegex = new QRegularExpression(*hymnPattern);
    const QRegularExpression *scripRegex = new QRegularExpression(*scripPattern);

    const QString *bkPattern = new QString("(^\\d{0,1}"
                        "[A-Z]{1}[a-z]+)"
                        ".{1}"
                        "(\\d+)"
                        ".{1}"
                        "(\\d+)");
    const QRegularExpression *bkRegex = new QRegularExpression(*bkPattern);

    const QString *chPattern = new QString("-(\\d{0,1}"
                        "[A-Z]{1}[a-z]+)"
                        ".{1}"
                        "(\\d+)"
                        ".{1}"
                        "(\\d+)$");
    const QRegularExpression *chRegex = new QRegularExpression(*chPattern);

    const QString *nrPattern = new QString("\\s*\\d+\\s*");
    const QRegularExpression *nrRegex = new QRegularExpression(*nrPattern);

    // spurgeon scripture links
    const QString spurgeonUrlPattern = "href='B:"
                               "(?<scrip>\\d{0,3}\\s*\\d{0,3}:\\d{0,3})"
                               "'";
    const QRegularExpression *SpurgeonUrlRegex = new QRegularExpression(spurgeonUrlPattern);

    // footnotes
    const QString *notePattern = new QString("<f>\\[(\\d+)\\]</f>");
    const QRegularExpression *noteRegex = new QRegularExpression(*notePattern);
    const QString *breakPattern = new QString("^<pb/>");
    const QRegularExpression *breakRegex = new QRegularExpression(*breakPattern);

    const QString *tagPattern = new QString("</?\\w+>|<pb/>");
    const QRegularExpression *tagRegex = new QRegularExpression(*tagPattern);

    QString activeTl; //active translation
    bool hasNotes = false; // translation has footnotes?
    QString activeSubh; //active subheadings

    //QtDocs: The following line declares a member variable which is a pointer to the MainWindow UI class. A member variable is associated with a specific class, and accessible for all its methods.
    Ui::MainWindow *ui;

public:
    //QtDocs:The following line declares a constructor that has a default argument called parent.
    //The value 0 indicates that the widget has no parent (it is a top-level widget).
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    bool checkTableExists(const QString &activeTl);

    void getDictWord(QString word);
    void getDictSug(QString word = "");
    void strongify();
    void getStrongs(QString strongs);
    void getTWOT(QString twot);
    void setStyleSheets();

    void toggleBible();
    void toggleFullscreen();
    void toggleTabW();
    void toggleMenu();
    void toggleInfo();

    void bookSelected();
    void chapterSelected();
    void updateBooksWidget(const QString &lang);
    void updateTLandSubh(const QString &translation, const QString &subheadings);
    void setHasNotes();
    void updateChapterWidget();
    void updateCbTranslations(); // populate comboboxes
    void setTranslation();
    void setBookTitle(QString title = "");
    QString getLongTitle(const int bk);

    void processPrintQueue();
    void printScriptures();
    void addToHistory(QHash<QString, int> job);
    void printFromHistory(QHash<QString, int> job);

    void filterImmersion();
    void filterJudeans();

    void showFindFrame();

    void ccMenuBibleFrame();
    void ccMenuBackground();
    void ccMenuInfo();
    void ccMenuStrongs();

    void createOtNtMenus();
    void popupChapters(int bkNr=0);
    void nextChapter();
    void prevChapter();

    void showAboutBook(); // show book information on the info frame
    void showAboutTl(); // show translation information on the info frame
    void centerApp();

    // use regex to check for a book and chapter to select/print
    void printRequest(const QString &request);
    void printRequestSingle(const QString &request); // for a single regex lookup

    void searchScriptures(); // tab search
    void versesWithStrongNumber(const QString &strongs, const QString &where);
    void populateSearchCbs();

    // tb: QTextBrowser
    void on_tb_scriptures_anchorClicked(const QUrl &url);
    void breakItUp(QString &textwall);
    void on_search_tb_anchorClicked(const QUrl &url);
    void on_info_tb_anchorClicked(const QUrl &url);
    void on_strongs_tb_anchorClicked(const QUrl &url);

    QString getVerse(const int bk, const int ch, const int vs);

    void on_btn_select_today_clicked();
    void todaysProverb();
    void todaysPsalm();
    void todaysLetter();
    // show a devotion from spurgeon and print a scripture
    void morningAndEvening(const QString &morningOrEvening);

    void modifications();

    void readingPlan();
    void escapeKey();
    void setEncTxt();
    void changeEncTxt();
    void printEncTxt(int bk, int ch, int vs, const QString &verse);
    void setEncPic();
    void changeEncPic();
    void showEncPic(const QString &fileName);
    void showShortcuts();

    void theLordsPrayer();
    void breakingBread();
    void immersion();
    void salvation();
    void worries();

    void findInPage(const QString &nextOrPrev);
    void startFind();
    void countMatches();
    void highlightMatches();
    void clearHighLights();
    void modifyFindInPageFlags();
    void setFindInPageLocation();

    void makeTextMenuItems();
    void showText(const QString &filepath, const QString &filename);

    void makeMusicMenu();
    void playMusic(QString filepath, QString filename);
    void stopPlayer();
    void playRandom();
    void spokenWord();
    static bool compareFunctionR(QAction *a, QAction *b);
    static bool compareFunctionS(QAction *a, QAction *b);

    void openParW();
    void applyFont(const QString &font, const QString &fontS,
                   const int &margin, const int &width);
    void applyScheme(const QString &aScheme);

    void addRostersToMenu();
    void rosterActionTriggered(QString rosterName);
    void loadRoster();
    void setActiveRoster();
    void on_action_delete_roster_triggered();
    void on_action_reset_roster_triggered();
    void on_btn_next_session_clicked();
    void on_btn_prev_session_clicked();

    void printSession(const QString &rosterName);
    void updateSession(const QString &rosterName, int session);

    void addBookmark();
    void renameBookmark();
    void deleteBookmark();
    void buildBookmarkMenu();
    void printBookmark(const QString &bookmarkName);

    void getBooksAbbr();
    void setFilters();
    void aboutFilters();

    void closeEvent(QCloseEvent*) override;    
    static void exitApp();
    void resizeEvent(QResizeEvent*) override;

    void on_cb_roster_read_clicked();
    void on_dict_pb_index_clicked();

    void on_dict_pb_go_clicked();

    void makeCrossRefs();
    void showTopics();
    void getTopic(const QString &topic);

signals:
    void parOpened(QString tlAbbr, QHash<QString, int> job);
    void setParwStyle(QHash<QString, QString> clrScheme);
};

#endif // MAINWINDOW_H
