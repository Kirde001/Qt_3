#ifndef EDITBOOKDIALOG_H
#define EDITBOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>

class EditBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditBookDialog(QWidget *parent = nullptr);
    ~EditBookDialog();

    QString getAuthor() const;
    QString getTitle() const;
    int getYear() const;
    QString getGenre() const;
    QString getPublisher() const;
    QString getISBN() const;
    int getPageCount() const;

    void setBookData(const QString &author, const QString &title, int year, const QString &genre, const QString &publisher, const QString &isbn, int pageCount);
private slots:
    void on_buttonBox_accepted();

private:
    QLineEdit *authorLineEdit;
    QLineEdit *titleLineEdit;
    QSpinBox *yearSpinBox;
    QLineEdit *genreLineEdit;
    QDialogButtonBox *buttonBox;
    QLineEdit *publisherLineEdit;
    QLineEdit *isbnLineEdit;
    QSpinBox *pageCountSpinBox;

    void setupUi();
    bool validateInput();
};

#endif // EDITBOOKDIALOG_H

