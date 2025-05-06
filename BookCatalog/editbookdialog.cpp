#include "editbookdialog.h"
#include <QFormLayout>
#include <QMessageBox>

EditBookDialog::EditBookDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    connect(buttonBox, &QDialogButtonBox::accepted, this, &EditBookDialog::on_buttonBox_accepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

EditBookDialog::~EditBookDialog()
{
}

void EditBookDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    authorLineEdit = new QLineEdit();
    titleLineEdit = new QLineEdit();
    yearSpinBox = new QSpinBox();
    genreLineEdit = new QLineEdit();
    publisherLineEdit = new QLineEdit();
    isbnLineEdit = new QLineEdit();
    pageCountSpinBox = new QSpinBox();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    yearSpinBox->setRange(1500, 2030);
    pageCountSpinBox->setRange(1, 10000);

    formLayout->addRow("Автор:", authorLineEdit);
    formLayout->addRow("Название:", titleLineEdit);
    formLayout->addRow("Год издания:", yearSpinBox);
    formLayout->addRow("Жанр:", genreLineEdit);
    formLayout->addRow("Издательство:", publisherLineEdit);
    formLayout->addRow("ISBN:", isbnLineEdit);
    formLayout->addRow("Количество страниц:", pageCountSpinBox);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    setWindowTitle("Редактировать книгу");
}

bool EditBookDialog::validateInput()
{
    if (authorLineEdit->text().isEmpty() ||
        titleLineEdit->text().isEmpty() ||
        genreLineEdit->text().isEmpty() ||
        publisherLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка валидации", "Все поля должны быть заполнены.");
        return false;
    }
    return true;
}

void EditBookDialog::on_buttonBox_accepted()
{
    if (validateInput()) {
        accept();
    }
}

QString EditBookDialog::getAuthor() const
{
    return authorLineEdit->text();
}

QString EditBookDialog::getTitle() const
{
    return titleLineEdit->text();
}

int EditBookDialog::getYear() const
{
    return yearSpinBox->value();
}

QString EditBookDialog::getGenre() const
{
    return genreLineEdit->text();
}

QString EditBookDialog::getPublisher() const
{
    return publisherLineEdit->text();
}

QString EditBookDialog::getISBN() const
{
    return isbnLineEdit->text();
}

int EditBookDialog::getPageCount() const
{
    return pageCountSpinBox->value();
}


void EditBookDialog::setBookData(const QString &author, const QString &title, int year, const QString &genre, const QString &publisher, const QString &isbn, int pageCount)
{
    authorLineEdit->setText(author);
    titleLineEdit->setText(title);
    yearSpinBox->setValue(year);
    genreLineEdit->setText(genre);
    publisherLineEdit->setText(publisher);
    isbnLineEdit->setText(isbn);
    pageCountSpinBox->setValue(pageCount);
}

