#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QWidget>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QColor>
#include <QFont>
#include <QFileDialog>
#include <QInputDialog>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QRegularExpression>
#include <QLineEdit>
#include <QSpinBox>
#include <QListWidget>
#include <QSplitter>
#include <vector>
#include "../dict/dict.h"
#include "../text_operations/text_formation.h"
#include "../text_operations/BMH_search_alg.h"

struct PageSearchResult {
    int page;       
    int line;   
    int position;  
    int length;    
    QString word;   
    int absolute_position;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_open_file_clicked();
    void on_format_text_clicked();
    void on_add_word_clicked();
    void on_next_page_clicked();
    void on_prev_page_clicked();
    void on_add_word_from_context_menu();
    void on_search_word_clicked();
    void on_next_search_result_clicked();
    void on_prev_search_result_clicked();
    void on_search_result_clicked(QListWidgetItem* item);

private:
    void setup_ui();
    void setup_connections();
    void highlight_all();
    void update_page_view();
    QString extract_word_from_position(int position) const;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void show_current_search_result();
    

    void perform_global_search(const QString& word);
    void update_search_results_on_current_page();
    int calculate_absolute_position(int page, int line, int pos_in_line) const;
    int calculate_position_in_page_text(const PageSearchResult& result) const;
    
    QTextEdit *page_display;
    QPushButton *open_file_button;
    QPushButton *format_button;
    QPushButton *add_word_button;
    QPushButton *next_page_button;
    QPushButton *prev_page_button;
    QLabel *status_label;
    QLabel *page_label;
    QLabel *file_name_label;
    QLabel *dict_info;
    
    QSpinBox *page_width_spin;
    QSpinBox *lines_per_page_spin;
    
    QLineEdit *search_input;
    QPushButton *search_button;
    QPushButton *next_search_button;
    QPushButton *prev_search_button;
    
    QListWidget *search_results_list;

    QMenu *context_menu;
    QAction *add_word_action;
    QString selected_word;
    
    static const int PAGE_WIDTH = 80;
    static const int LINES_PER_PAGE = 30;
    
    int current_page;
    int total_pages;
    QStringList pages;
    QString current_file_path;
    QString original_text;
    
    struct SimpleSearchResult {
        int position;
        int length;
    };
    std::vector<SimpleSearchResult> search_results;
    int current_search_index;
    
    std::vector<PageSearchResult> all_search_results;
    
    QWidget *central_widget;
    QSplitter *splitter;
};