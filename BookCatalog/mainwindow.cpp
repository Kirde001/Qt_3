#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "editbookdialog.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupModel();
    setupSearch();

    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::addBook);
    connect(ui->btnEdit, &QPushButton::clicked, this, &MainWindow::editBook);
    connect(ui->btnRemove, &QPushButton::clicked, this, &MainWindow::removeBook);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::searchBooks);

    connect(ui->actionImportCSV, &QAction::triggered, this, &MainWindow::importCSV);
    connect(ui->actionExportCSV, &QAction::triggered, this, &MainWindow::exportCSV);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);

    connect(ui->actionImportJSON, &QAction::triggered, this, &MainWindow::importJSON);
    connect(ui->actionExportJSON, &QAction::triggered, this, &MainWindow::exportJSON);
    connect(ui->actionExitJSON, &QAction::triggered, this, &QApplication::quit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModel()
{
    model = new QStandardItemModel(this);
    model->setColumnCount(7);
    model->setHeaderData(0, Qt::Horizontal, "Автор");
    model->setHeaderData(1, Qt::Horizontal, "Название");
    model->setHeaderData(2, Qt::Horizontal, "Год");
    model->setHeaderData(3, Qt::Horizontal, "Жанр");
    model->setHeaderData(4, Qt::Horizontal, "Издательство");
    model->setHeaderData(5, Qt::Horizontal, "ISBN");
    model->setHeaderData(6, Qt::Horizontal, "Страниц");

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);

    ui->tableView->setModel(proxyModel);
    ui->tableView->setSortingEnabled(true);
}

void MainWindow::addBook()
{
    QString author = QInputDialog::getText(this, "Добавить книгу", "Автор:");
    if (author.isEmpty()) {
        return;
    }
    QString title = QInputDialog::getText(this, "Добавить книгу", "Название:");
    if (title.isEmpty()) {
        return;
    }
    bool ok;
    int year = QInputDialog::getInt(this, "Добавить книгу", "Год издания:", 2000, 0, 2100, 1, &ok);
    if (!ok) {
        return;
    }
    QString genre = QInputDialog::getText(this, "Добавить книгу", "Жанр:");
    if (genre.isEmpty()) {
        return;
    }
    QString publisher = QInputDialog::getText(this, "Добавить книгу", "Издательство:");
    if (publisher.isEmpty()) {
        return;
    }
    QString isbn = QInputDialog::getText(this, "Добавить книгу", "ISBN:");
    int pageCount = QInputDialog::getInt(this, "Добавить книгу", "Количество страниц:", 0, 0, 10000, 1, &ok);
    if (!ok) {
        return;
    }
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(author)
             << new QStandardItem(title)
             << new QStandardItem(QString::number(year))
             << new QStandardItem(genre)
             << new QStandardItem(publisher)
             << new QStandardItem(isbn)
             << new QStandardItem(QString::number(pageCount));
    model->appendRow(rowItems);

    QMessageBox::information(this, "Успех", "Книга успешно добавлена!");
}

void MainWindow::editBook()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Выберите книгу для редактирования");
        return;
    }

    int row = index.row();
    QString author = model->item(row, 0)->text();
    QString title = model->item(row, 1)->text();
    int year = model->item(row, 2)->text().toInt();
    QString genre = model->item(row, 3)->text();
    QString publisher = (model->columnCount() > 4 && model->item(row, 4)) ? model->item(row, 4)->text() : "";
    QString isbn = (model->columnCount() > 5 && model->item(row, 5)) ? model->item(row, 5)->text() : "";
    int pageCount = (model->columnCount() > 6 && model->item(row, 6)) ? model->item(row, 6)->text().toInt() : 0;


    EditBookDialog dialog(this);
    dialog.setBookData(author, title, year, genre, publisher, isbn, pageCount);

    if (dialog.exec() == QDialog::Accepted) {
        model->setItem(row, 0, new QStandardItem(dialog.getAuthor()));
        model->setItem(row, 1, new QStandardItem(dialog.getTitle()));
        model->setItem(row, 2, new QStandardItem(QString::number(dialog.getYear())));
        model->setItem(row, 3, new QStandardItem(dialog.getGenre()));
        model->setItem(row, 4, new QStandardItem(dialog.getPublisher()));
        model->setItem(row, 5, new QStandardItem(dialog.getISBN()));
        model->setItem(row, 6, new QStandardItem(QString::number(dialog.getPageCount())));

    }
}

void MainWindow::removeBook()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Выберите книгу для удаления");
        return;
    }

    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    if (sourceIndex.isValid()) {
        model->removeRow(sourceIndex.row());
    }
}

void MainWindow::setupSearch()
{
    ui->searchFieldComboBox->addItem("Все поля");
    ui->searchFieldComboBox->addItem("Автор");
    ui->searchFieldComboBox->addItem("Название");
    ui->searchFieldComboBox->addItem("Год");
    ui->searchFieldComboBox->addItem("Жанр");
    ui->searchFieldComboBox->addItem("Издательство");
    ui->searchFieldComboBox->addItem("ISBN");
    ui->searchFieldComboBox->addItem("Страниц");
}



int MainWindow::getColumnIndex(const QString &columnTitle) const
{
    for (int i = 0; i < model->columnCount(); ++i) {
        if (model->headerData(i, Qt::Horizontal).toString() == columnTitle) {
            return i;
        }
    }
    return -1;
}

void MainWindow::searchBooks()
{
    QString searchText = ui->searchLineEdit->text();
    QString selectedField = ui->searchFieldComboBox->currentText();

    if (selectedField == "Все поля") {
        proxyModel->setFilterKeyColumn(-1);
        proxyModel->setFilterFixedString(searchText);
    } else {
        int column = getColumnIndex(selectedField);
        if (column != -1) {
            proxyModel->setFilterKeyColumn(column);
            proxyModel->setFilterFixedString(searchText);
        }
    }
}


void MainWindow::exportCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Экспорт в CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {

        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream out(&file);

    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList fields;
        for (int col = 0; col < model->columnCount(); ++col) {
            QString text = model->item(row, col)->text();
            text.replace("\"", "\"\"");
            fields.append('"' + text + '"');
        }
        out << fields.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, "Экспорт", "Файл сохранён успешно!");
}

void MainWindow::importCSV()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Импорт CSV", "", "CSV Files (*.csv)");
        if (fileName.isEmpty()) {

            return;
        }

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
            return;
        }

        QTextStream in(&file);
        model->clear();
        setupModel();

        int rowCount = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");

            if (fields.size() != 4) {
                continue;
            }

            QList<QStandardItem *> rowItems;
            for (const QString &field : fields)
                rowItems.append(new QStandardItem(field.trimmed()));

            model->appendRow(rowItems);
            rowCount++;
        }

        file.close();

        if (rowCount == 0) {
            QMessageBox::information(this, "Импорт CSV", "Файл загружен, но он пуст.");
        } else {
            QMessageBox::information(this, "Импорт CSV", "Файл успешно загружен: " + fileName);
        }
}


void MainWindow::exportJSON()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Экспорт в JSON", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
        fileName += ".json";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QJsonArray bookList;
    for (int row = 0; row < model->rowCount(); ++row) {
        QJsonObject bookObject;
        bookObject["Автор"] = model->item(row, getColumnIndex("Автор"))->text();
        bookObject["Название"] = model->item(row, getColumnIndex("Название"))->text();
        bookObject["Год"] = model->item(row, getColumnIndex("Год"))->text().toInt();
        bookObject["Жанр"] = model->item(row, getColumnIndex("Жанр"))->text();
        bookObject["Издательство"] = model->item(row, getColumnIndex("Издательство"))->text();
        bookObject["ISBN"] = model->item(row, getColumnIndex("ISBN"))->text();
        bookObject["Страниц"] = model->item(row, getColumnIndex("Страниц"))->text().toInt();
        bookList.append(bookObject);
    }

    QJsonDocument jsonDoc(bookList);
    file.write(jsonDoc.toJson(QJsonDocument::Indented));

    file.close();
    QMessageBox::information(this, "Экспорт", "Файл сохранён успешно!");
}

void MainWindow::importJSON()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Импорт JSON", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

    if (!jsonDoc.isArray()) {
        QMessageBox::warning(this, "Ошибка импорта", "Файл JSON имеет некорректный формат (ожидается массив).");
        return;
    }

    model->clear();
    setupModel();

    QJsonArray bookList = jsonDoc.array();
    int rowCount = 0;
    for (const QJsonValue &value : bookList) {
        if (value.isObject()) {
            QJsonObject bookObject = value.toObject();

            QString author = bookObject.value("Автор").toString();
            QString title = bookObject.value("Название").toString();
            int year = bookObject.value("Год").toInt();
            QString genre = bookObject.value("Жанр").toString();
            QString publisher = bookObject.value("Издательство").toString();
            QString isbn = bookObject.value("ISBN").toString();
            int pageCount = bookObject.value("Страниц").toInt();
            if (author.isEmpty() || title.isEmpty() || genre.isEmpty()) {

                 qDebug() << "Пропущена запись из JSON из-за отсутствия обязательных полей";
                 continue;
            }

            QList<QStandardItem *> rowItems;
            rowItems << new QStandardItem(author)
                     << new QStandardItem(title)
                     << new QStandardItem(QString::number(year))
                     << new QStandardItem(genre)
                     << new QStandardItem(publisher)
                     << new QStandardItem(isbn)
                     << new QStandardItem(QString::number(pageCount));

            model->appendRow(rowItems);
            rowCount++;
        }
    }

    if (rowCount == 0 && !bookList.isEmpty()) {
        QMessageBox::information(this, "Импорт JSON", "Файл загружен, но ни одна запись не была корректно импортирована.");
    } else if (rowCount == 0) {
         QMessageBox::information(this, "Импорт JSON", "Файл загружен, но он пуст или содержит некорректные данные.");
    }
    else {
        QMessageBox::information(this, "Импорт JSON", QString("Успешно загружено %1 записей из файла: %2").arg(rowCount).arg(fileName));
    }
}
