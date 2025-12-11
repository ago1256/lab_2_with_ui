#include "mainwindow.h"
#include <QRegularExpression>
#include <QTextBlock>
#include <QScrollBar>
#include <QFontDatabase>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QContextMenuEvent>
#include <QApplication>
#include <QDir>
#include <QSplitter>
#include <QListWidget>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), 
      current_page(0), 
      total_pages(0), 
      current_search_index(0),
      page_display(nullptr),
      open_file_button(nullptr),
      splitter(nullptr)
{
    setup_ui();
    setup_connections();
    
    context_menu = new QMenu(this);
    add_word_action = new QAction("Добавить слово в словарь", this);
    context_menu->addAction(add_word_action);
    
    connect(add_word_action, &QAction::triggered, this, &MainWindow::on_add_word_from_context_menu);

    page_display->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(page_display, &QTextEdit::customContextMenuRequested, this, [this](const QPoint &pos) {
        QTextCursor cursor = page_display->cursorForPosition(pos);
        int cursor_pos = cursor.position();
        selected_word = extract_word_from_position(cursor_pos);
        
        if (!selected_word.isEmpty() && !global_dict.contains(selected_word.toStdString())) {
            add_word_action->setText(QString("Добавить '%1' в словарь").arg(selected_word));
            add_word_action->setData(selected_word);
            context_menu->exec(page_display->mapToGlobal(pos));
        }
    });
    
    qDebug() << "Количество слов в словаре:" << global_dict.size();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setup_ui()
{
    splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);
    
    QWidget *left_widget = new QWidget(this);
    QVBoxLayout *main_layout = new QVBoxLayout(left_widget);
    
    QHBoxLayout *control_layout = new QHBoxLayout();
    
    open_file_button = new QPushButton("Выбрать файл", this);
    format_button = new QPushButton("Форматировать текст", this);
    add_word_button = new QPushButton("Добавить слово", this);
    
    open_file_button->setMinimumWidth(120);
    format_button->setMinimumWidth(140);
    add_word_button->setMinimumWidth(120);
    
    control_layout->addWidget(open_file_button);
    control_layout->addWidget(format_button);
    control_layout->addWidget(add_word_button);
    control_layout->addStretch();
    
    QHBoxLayout *settings_layout = new QHBoxLayout();
    
    QLabel *width_label = new QLabel("Ширина страницы:", this);
    page_width_spin = new QSpinBox(this);
    page_width_spin->setMinimum(40);
    page_width_spin->setMaximum(200);
    page_width_spin->setValue(PAGE_WIDTH);
    page_width_spin->setFixedWidth(60);
    
    QLabel *lines_label = new QLabel("Строк на странице:", this);
    lines_per_page_spin = new QSpinBox(this);
    lines_per_page_spin->setMinimum(10);
    lines_per_page_spin->setMaximum(100);
    lines_per_page_spin->setValue(LINES_PER_PAGE);
    lines_per_page_spin->setFixedWidth(60);
    
    settings_layout->addWidget(width_label);
    settings_layout->addWidget(page_width_spin);
    settings_layout->addSpacing(15);
    settings_layout->addWidget(lines_label);
    settings_layout->addWidget(lines_per_page_spin);
    settings_layout->addStretch();
    
    file_name_label = new QLabel("Файл не выбран", this);
    file_name_label->setStyleSheet("font-weight: bold; color: blue; padding: 2px;");
    
    QHBoxLayout *search_layout = new QHBoxLayout();
    
    QLabel *search_label = new QLabel("Поиск:", this);
    search_input = new QLineEdit(this);
    search_input->setPlaceholderText("Введите слово для поиска");
    search_input->setMinimumWidth(200);
    search_button = new QPushButton("Найти", this);
    search_button->setFixedWidth(80);
    prev_search_button = new QPushButton("←", this);
    prev_search_button->setFixedWidth(40);
    prev_search_button->setToolTip("Предыдущий результат");
    next_search_button = new QPushButton("→", this);
    next_search_button->setFixedWidth(40);
    next_search_button->setToolTip("Следующий результат");
    
    search_layout->addWidget(search_label);
    search_layout->addWidget(search_input);
    search_layout->addWidget(search_button);
    search_layout->addWidget(prev_search_button);
    search_layout->addWidget(next_search_button);
    search_layout->addStretch();
    
    QHBoxLayout *page_nav_layout = new QHBoxLayout();
    
    prev_page_button = new QPushButton("← Назад", this);
    prev_page_button->setFixedWidth(100);
    page_label = new QLabel("Страница 1 из 1", this);
    page_label->setAlignment(Qt::AlignCenter);
    next_page_button = new QPushButton("Вперед →", this);
    next_page_button->setFixedWidth(100);
    
    page_nav_layout->addWidget(prev_page_button);
    page_nav_layout->addWidget(page_label);
    page_nav_layout->addWidget(next_page_button);
    page_nav_layout->addStretch();
    
    page_display = new QTextEdit(this);
    page_display->setReadOnly(true);
    page_display->setPlaceholderText("Загрузите файл");
    
    QFont fixed_font("Courier New", 10);
    page_display->setFont(fixed_font);
    
    status_label = new QLabel(this);
    status_label->setStyleSheet("background-color: #f0f0f0; padding: 5px;");
    status_label->setFrameShape(QFrame::Box);
    
    dict_info = new QLabel(
        QString("Словарь: data/dict.txt | Загружено слов: %1 | ")
        .arg(global_dict.size()), 
        this
    );
    dict_info->setStyleSheet("color: green; font-size: 10px; padding: 2px;");
    
    main_layout->addLayout(control_layout);
    main_layout->addLayout(settings_layout);
    main_layout->addWidget(file_name_label);
    main_layout->addLayout(search_layout);
    main_layout->addLayout(page_nav_layout);
    main_layout->addWidget(page_display, 1);
    main_layout->addWidget(status_label);
    main_layout->addWidget(dict_info);
    
    QWidget *right_widget = new QWidget(this);
    QVBoxLayout *right_layout = new QVBoxLayout(right_widget);
    
    QLabel *results_label = new QLabel("Результаты поиска:", this);
    results_label->setStyleSheet("font-weight: bold; font-size: 12px; padding: 5px;");
    
    search_results_list = new QListWidget(this);
    search_results_list->setMaximumWidth(350);
    search_results_list->setAlternatingRowColors(true);
    
    right_layout->addWidget(results_label);
    right_layout->addWidget(search_results_list, 1);
    
    splitter->addWidget(left_widget);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 3); 
    splitter->setStretchFactor(1, 1);
    
    setWindowTitle("Spell checker with Radix tree");
    resize(1200, 700);
}

void MainWindow::setup_connections()
{
    connect(open_file_button, &QPushButton::clicked, this, &MainWindow::on_open_file_clicked);
    connect(format_button, &QPushButton::clicked, this, &MainWindow::on_format_text_clicked);
    connect(add_word_button, &QPushButton::clicked, this, &MainWindow::on_add_word_clicked);
    connect(next_page_button, &QPushButton::clicked, this, &MainWindow::on_next_page_clicked);
    connect(prev_page_button, &QPushButton::clicked, this, &MainWindow::on_prev_page_clicked);
    
    connect(search_button, &QPushButton::clicked, this, &MainWindow::on_search_word_clicked);
    connect(next_search_button, &QPushButton::clicked, this, &MainWindow::on_next_search_result_clicked);
    connect(prev_search_button, &QPushButton::clicked, this, &MainWindow::on_prev_search_result_clicked);
    connect(search_input, &QLineEdit::returnPressed, this, &MainWindow::on_search_word_clicked);
    connect(search_results_list, &QListWidget::itemClicked, this, &MainWindow::on_search_result_clicked);
}

void MainWindow::on_open_file_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, "Выберите текстовый файл", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    
    if (!file_path.isEmpty()) {
        QFile file(file_path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            
            original_text = content;
            file.close();
            
            page_display->setPlainText(content);
            
            current_file_path = file_path;
            QFileInfo file_info(file_path);
            file_name_label->setText(QString("Файл: %1").arg(file_info.fileName()));
            status_label->setText(QString("Файл загружен: %1 | Слов в словаре: %2").arg(file_info.fileName()).arg(global_dict.size()));
            
            search_results.clear();
            all_search_results.clear();
            search_results_list->clear();
            search_input->clear();
            pages.clear();
            current_page = 0;
            current_search_index = 0;
            
            page_label->setText("Страница 1 из 1");
            
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        }
    }
}

void MainWindow::on_format_text_clicked()
{
    if (original_text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите файл");
        return;
    }
    
    size_t page_width = page_width_spin->value();
    size_t lines_per_page = lines_per_page_spin->value();
    
    Text_formation formatter(page_width, lines_per_page);
    std::vector<std::string> formatted_pages = formatter.format_to_pages(original_text.toStdString());
    
    pages.clear();
    for (const auto& page : formatted_pages) {
        pages.append(QString::fromStdString(page));
    }
    
    total_pages = pages.size();
    current_page = 0;
    
    search_results.clear();
    all_search_results.clear();
    search_results_list->clear();
    search_input->clear();
    current_search_index = 0;
    
    update_page_view();
    
    status_label->setText(QString("Текст отформатирован: %1 страниц | Ширина: %2 символов, строк на странице: %3")
                         .arg(total_pages).arg(page_width).arg(lines_per_page));
}

void MainWindow::on_add_word_clicked()
{
    bool ok;
    QString word = QInputDialog::getText(this, "Добавить слово", 
                                        "Введите слово для добавления в словарь:",
                                        QLineEdit::Normal, "", &ok);
    
    if (ok && !word.isEmpty()) {
        global_dict.insert(word.toStdString());
        
        QString dict_txt_path = "../data/dict.txt";
        save_dictionary_full();
  
        QMessageBox::information(this, "Успех", QString("Слово '%1' добавлено в словарь").arg(word));
        highlight_all();
        dict_info->setText(QString("Словарь: data/dict.txt | Загружено слов: %1").arg(global_dict.size()));
    }
}

void MainWindow::on_add_word_from_context_menu()
{
    QString word_to_add;
    if (add_word_action->data().isValid()) {
        word_to_add = add_word_action->data().toString();
    } else if (!selected_word.isEmpty()) {
        word_to_add = selected_word;
    }
    
    if (!word_to_add.isEmpty()) {
        global_dict.insert(word_to_add.toStdString());
        
        QString dict_path = "../data/dict.txt";
        if (save_dictionary_full()) {

            dict_info->setText(QString("Словарь: data/dict.txt | Загружено слов: %1 | ").arg(global_dict.size()));
            
            if (!pages.isEmpty()) {
                highlight_all();
            } else if (!original_text.isEmpty()) {
                page_display->setPlainText(original_text);
                highlight_all();
            }
            
            status_label->setText(QString("Слово '%1' добавлено в словарь").arg(word_to_add));
            
            QMessageBox::information(this, "Успех", 
                QString("Слово '%1' добавлено в словарь.\nВсего слов в словаре: %2")
                .arg(word_to_add)
                .arg(global_dict.size()));
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить словарь");
            status_label->setText("Ошибка сохранения словаря");
        }
        
        add_word_action->setData(QVariant());
        selected_word.clear();
    }
}

void MainWindow::on_next_page_clicked()
{
    if (current_page < total_pages - 1) {
        current_page++;
        update_page_view();
        highlight_all();  
    }
}

void MainWindow::on_prev_page_clicked()
{
    if (current_page > 0) {
        current_page--;
        update_page_view();
        highlight_all(); 
    }
}

void MainWindow::on_search_word_clicked()
{
    QString search_word = search_input->text().trimmed();
    if (search_word.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите слово для поиска");
        return;
    }
    
    if (pages.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала отформатируйте текст");
        return;
    }
    
    perform_global_search(search_word);
}

void MainWindow::on_next_search_result_clicked()
{
    if (search_results.empty()) {
        QMessageBox::information(this, "Информация", "На этой странице нет результатов поиска");
        return;
    }
    
    if (current_search_index < (int)search_results.size() - 1) {
        current_search_index++;
        show_current_search_result();
    } else {
        int next_page_with_results = -1;
        for (int page = current_page + 1; page < total_pages; ++page) {
            for (const auto& result : all_search_results) {
                if (result.page == page) {
                    next_page_with_results = page;
                    break;
                }
            }
            if (next_page_with_results != -1) break;
        }
        
        if (next_page_with_results != -1) {
            current_page = next_page_with_results;
            update_page_view();
            current_search_index = 0;
            show_current_search_result();
        } else {
            QMessageBox::information(this, "Информация", 
                "Это последний результат поиска в документе");
        }
    }
}

void MainWindow::on_prev_search_result_clicked()
{
    if (search_results.empty()) {
        QMessageBox::information(this, "Информация", "На этой странице нет результатов поиска");
        return;
    }
    
    if (current_search_index > 0) {
        current_search_index--;
        show_current_search_result();
    } else {
        int prev_page_with_results = -1;
        for (int page = current_page - 1; page >= 0; --page) {
            for (const auto& result : all_search_results) {
                if (result.page == page) {
                    prev_page_with_results = page;
                    break;
                }
            }
            if (prev_page_with_results != -1) break;
        }
        
        if (prev_page_with_results != -1) {
            current_page = prev_page_with_results;
            update_page_view();
            current_search_index = search_results.size() - 1;
            show_current_search_result();
        } else {
            QMessageBox::information(this, "Информация", 
                "Это первый результат поиска в документе");
        }
    }
}

void MainWindow::on_search_result_clicked(QListWidgetItem* item) {
    int global_index = search_results_list->row(item);
    if (global_index >= 0 && global_index < all_search_results.size()) {
        const PageSearchResult& result = all_search_results[global_index];
        
        if (result.page != current_page) {
            current_page = result.page;
            update_page_view();
        }
        
        current_search_index = -1;
        for (size_t i = 0; i < search_results.size(); ++i) {
            int pos_in_page = calculate_position_in_page_text(result);
            if (pos_in_page == search_results[i].position) {
                current_search_index = static_cast<int>(i);
                break;
            }
        }
        
        if (current_search_index != -1) {
            show_current_search_result();
        }
    }
}

void MainWindow::perform_global_search(const QString& word) {
    all_search_results.clear();
    search_results_list->clear();
    
    if (word.isEmpty()) {
        status_label->setText("Введите слово для поиска");
        return;
    }
    
    if (pages.isEmpty()) {
        status_label->setText("Сначала отформатируйте текст");
        return;
    }

    for (int page_idx = 0; page_idx < pages.size(); ++page_idx) {
        QString page_text = pages[page_idx];
        QStringList lines = page_text.split('\n');
        
        for (int line_idx = 0; line_idx < lines.size(); ++line_idx) {
            QString line = lines[line_idx];
            
            std::vector<int> positions;
            std::string line_std = line.toStdString();
            std::string word_std = word.toStdString();
            positions = word_find_BMH(word_std, line_std);
            
            for (int pos : positions) {
                if (pos >= 0 && pos + word.length() <= line.length()) {
                    QString found_word = line.mid(pos, word.length());
                    
                bool is_whole_word = true;
                    
                    if (pos > 0) {
                        QChar prev_char = line[pos - 1];
                        is_whole_word = prev_char.isSpace() || prev_char.isPunct();
                    }
                    
                    if (pos + word.length() < line.length()) {
                        QChar next_char = line[pos + word.length()];
                        is_whole_word = is_whole_word && (next_char.isSpace() || next_char.isPunct());
                    }
                    
                    if (!is_whole_word) {
                        continue; 
                    }
                    
                    PageSearchResult result;
                    result.page = page_idx;
                    result.line = line_idx;
                    result.position = pos;
                    result.length = word.length();
                    result.word = found_word;
                    result.absolute_position = calculate_absolute_position(page_idx, line_idx, pos);
                    
                    all_search_results.push_back(result);
                    
                    QString item_text = QString("Стр. %1, строка %2: \"%3\"")
                        .arg(page_idx + 1)
                        .arg(line_idx + 1)
                        .arg(result.word);
                    search_results_list->addItem(item_text);
                }
            }
        }
    }
    
    update_search_results_on_current_page();
    
    if (!all_search_results.empty()) {
        if (!search_results.empty()) {
            current_search_index = 0;
            show_current_search_result();
        }
        
        status_label->setText(QString("Найдено: %1 совпадений во всем документе").arg(all_search_results.size()));
    } else {
        current_search_index = -1;
        highlight_all(); 
        status_label->setText(QString("Слово '%1' не найдено").arg(word));
    }
}

int MainWindow::calculate_absolute_position(int page, int line, int pos_in_line) const {
    if (pages.isEmpty()) return 0;
    
    int absolute_pos = 0;
    
    for (int i = 0; i < page && i < pages.size(); ++i) {
        absolute_pos += pages[i].length() + 1; 
    }

    QString page_text = pages[page];
    QStringList lines = page_text.split('\n');

    if (line >= lines.size() || line < 0) {
        return absolute_pos;
    }
    
    for (int i = 0; i < line && i < lines.size(); ++i) {
        absolute_pos += lines[i].length() + 1; 
    }
    
    if (pos_in_line >= 0 && pos_in_line <= lines[line].length()) {
        absolute_pos += pos_in_line;
    }
    
    return absolute_pos;
}


int MainWindow::calculate_position_in_page_text(const PageSearchResult& result) const {
    if (pages.isEmpty() || result.page != current_page) return 0;
    
    QString page_text = pages[current_page];
    QStringList lines = page_text.split('\n');
    
    if (result.line >= lines.size() || result.line < 0) {
        return 0;
    }
    
    int position = 0;
    for (int i = 0; i < result.line && i < lines.size(); ++i) {
        position += lines[i].length() + 1;
    }
    
    if (result.position >= 0 && result.position <= lines[result.line].length()) {
        position += result.position;
    }
    
    return position;
}

void MainWindow::update_search_results_on_current_page() {
    search_results.clear();
    
    for (const auto& result : all_search_results) {
        if (result.page == current_page) {
            SimpleSearchResult simple_result;
            simple_result.position = calculate_position_in_page_text(result);
            simple_result.length = result.length;
            search_results.push_back(simple_result);
        }
    }
    
    if (search_results.empty()) {
        current_search_index = -1;
    } else if (current_search_index >= static_cast<int>(search_results.size())) {
        current_search_index = 0;
    }
}



void MainWindow::update_page_view()
{
    if (pages.isEmpty()) {
        page_display->clear();
        page_label->setText("Страница 1 из 1");
        return;
    }
    
    if (current_page >= 0 && current_page < pages.size()) {
        page_display->setPlainText(pages[current_page]);
        page_label->setText(QString("Страница %1 из %2").arg(current_page + 1).arg(total_pages));
        
        highlight_all();
        
        update_search_results_on_current_page();
        
        if (!search_input->text().trimmed().isEmpty()) {
            if (!search_results.empty()) {
                status_label->setText(QString("Страница %1: %2 совпадений").arg(current_page + 1).arg(search_results.size()));
                
                if (current_search_index >= search_results.size()) {
                    current_search_index = 0;
                }
                show_current_search_result();
            } else {
                status_label->setText(QString("Страница %1: слово не найдено").arg(current_page + 1));
                current_search_index = -1;
            }
        }
    }
}

void MainWindow::highlight_all()
{
    if (pages.isEmpty()) return;
    
    QString current_text = pages[current_page];
    
    QTextDocument *doc = new QTextDocument(this);
    doc->setPlainText(current_text);
    
    QTextCursor cursor(doc);
    
    QRegularExpression word_regex("\\b[a-zA-Z0-9]+(?:['\\-:][a-zA-Z0-9]+)*\\b");
    QRegularExpressionMatchIterator it = word_regex.globalMatch(current_text);
    
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString word = match.captured();
        
        if (!global_dict.contains(word.toStdString())) {
            cursor.setPosition(match.capturedStart());
            cursor.setPosition(match.capturedEnd(), QTextCursor::KeepAnchor);
            
            QTextCharFormat error_format;
            error_format.setUnderlineColor(Qt::red);
            error_format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            cursor.setCharFormat(error_format);
        }
    }
    
    if (!search_results.empty() && !search_input->text().trimmed().isEmpty()) {

        QTextCharFormat search_format;
        search_format.setBackground(Qt::yellow);
        
        for (size_t i = 0; i < search_results.size(); ++i) {
            const auto& result = search_results[i];
            if (result.position >= 0 && result.position + result.length <= current_text.length()) {
                cursor.setPosition(result.position);
                cursor.setPosition(result.position + result.length, QTextCursor::KeepAnchor);
                
                if (i == static_cast<size_t>(current_search_index)) {
                    QTextCharFormat current_format;
                    current_format.setBackground(QColor(255, 165, 0));
                    
                    QTextCharFormat existing = cursor.charFormat();
                    if (existing.underlineStyle() == QTextCharFormat::SpellCheckUnderline) {
                        current_format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
                        current_format.setUnderlineColor(Qt::red);
                    }
                    
                    cursor.setCharFormat(current_format);
                } else {
                    QTextCharFormat existing = cursor.charFormat();
                    if (existing.underlineStyle() == QTextCharFormat::SpellCheckUnderline) {
                        search_format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
                        search_format.setUnderlineColor(Qt::red);
                    }
                    
                    cursor.setCharFormat(search_format);
                }
            }
        }
    }
    
    page_display->setDocument(doc);
    
    QTextDocument* old_doc = page_display->document();
    if (old_doc != doc) {
        delete old_doc;
    }
}

void MainWindow::show_current_search_result() {
    if (search_results.empty() || current_search_index < 0 || 
        current_search_index >= static_cast<int>(search_results.size())) {
        qDebug() << "show_current_search_result: нет результатов";
        return;
    }
    
    const auto& result = search_results[current_search_index];
    QString page_text = pages[current_page];
    
    qDebug() << "Показываем результат #" << current_search_index 
             << "позиция:" << result.position
             << "длина:" << result.length;
    
    if (result.position < 0 || result.position + result.length > page_text.length()) {
        qDebug() << "Ошибка: позиция выходит за границы текста!";
        status_label->setText("Ошибка: позиция выходит за границы текста");
        return;
    }
    
    int start = std::max(0, result.position - 10);
    int end = page_text.length()> result.position + result.length + 10 ? result.position + result.length + 10: page_text.length();
    QString context = page_text.mid(start, end - start);
    qDebug() << "Контекст: ..." << context << "...";
    
    QTextCursor cursor(page_display->document());
    cursor.setPosition(result.position);
    page_display->setTextCursor(cursor);
    page_display->ensureCursorVisible();
    
    highlight_all();
    

    int global_index = -1;
    for (size_t i = 0; i < all_search_results.size(); ++i) {
        const PageSearchResult& global_result = all_search_results[i];
        if (global_result.page == current_page) {
            int pos_in_page = calculate_position_in_page_text(global_result);
            if (pos_in_page == result.position) {
                global_index = static_cast<int>(i);
                break;
            }
        }
    }
    
    if (global_index >= 0 && global_index < search_results_list->count()) {
        search_results_list->setCurrentRow(global_index);
        search_results_list->scrollToItem(search_results_list->item(global_index));
    }
    
    status_label->setText(QString("Результат %1 из %2 на странице %3 (поз.%4)")
                         .arg(current_search_index + 1)
                         .arg(search_results.size())
                         .arg(current_page + 1)
                         .arg(result.position));
}

QString MainWindow::extract_word_from_position(int position) const
{
    QString text = page_display->toPlainText();
    if (position < 0 || position >= text.length()) return QString();

    int start = position;
    while (start > 0 && (text[start - 1].isLetterOrNumber() || 
                         text[start - 1] == '\'' || 
                         text[start - 1] == '-')) {
        start--;
    }
    

    int end = position;
    while (end < text.length() && (text[end].isLetterOrNumber() || 
                                   text[end] == '\'' || 
                                   text[end] == '-')) {
        end++;
    }
    
    if (start < end) {
        std::string word_str = text.mid(start, end - start).toStdString();
        QString word = QString::fromStdString(word_str);
        
        while (!word.isEmpty() && 
               (word.front() == '\'' || word.front() == '-' || 
                word.back() == '\'' || word.back() == '-')) {
            if (word.front() == '\'' || word.front() == '-') {
                word.remove(0, 1);
            }
            if (!word.isEmpty() && (word.back() == '\'' || word.back() == '-')) {
                word.chop(1);
            }
        }
        
        return word;
    }
    
    return QString();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (page_display->rect().contains(page_display->mapFromGlobal(event->globalPos()))) {
        QPoint pos = page_display->mapFromGlobal(event->globalPos());
        
        QTextCursor cursor = page_display->cursorForPosition(pos);
        int cursor_pos = cursor.position();
        
        selected_word = extract_word_from_position(cursor_pos);
        
        if (!selected_word.isEmpty()) {
            if (!global_dict.contains(selected_word.toStdString())) {
                add_word_action->setText(QString("Добавить '%1' в словарь").arg(selected_word));
                add_word_action->setData(selected_word);
                context_menu->exec(event->globalPos());
                return;
            } else {
                QAction* info_action = new QAction(QString("'%1' уже в словаре").arg(selected_word), this);
                info_action->setEnabled(false);
                QMenu temp_menu;
                temp_menu.addAction(info_action);
                temp_menu.exec(event->globalPos());
                return;
            }
        }
    }
    QMainWindow::contextMenuEvent(event);
}
