#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QImageReader>
#include <QFileDialog>
#include "VideoProcessing.h"
#include "video.h"

#include "diffplayer.h"

struct FilmStripDiff
{
    GEMUFF::VIMUFF::_Operation op;
    std::string hashV1;
    std::string hashV2;
    uint8_t* diff;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_slider_valueChanged(int value);

    void on_v1_diff_load_clicked();

    void on_v2_diff_load_clicked();

    void on_delta_save_clicked();

    void on_v1_load_patch_load_clicked();

    void on_delta_patch_load_clicked();

    void on_patch_slider_valueChanged(int value);

    void on_btnProcessMerge_clicked();

    void on_base_load_merge_clicked();

    void on_v1_merge_load_clicked();

    void on_v2_merge_load_clicked();

    void on_merge_slider_valueChanged(int value);

private:
    void GenerateFilmStripDiff();

private:
    Ui::MainWindow *ui;

    QGridLayout* gridLayout;
    QLabel* imgDisplayLabel;
    QScrollArea* scrollArea;
    QImageReader reader;

    GEMUFF::VIMUFF::Video v1;
    GEMUFF::VIMUFF::Video v2;
    GEMUFF::VIMUFF::Video base;
    GEMUFF::VIMUFF::Diff2Processing diffProcessing;
    GEMUFF::VIMUFF::MergeProcessing mergeProcessing;
    std::vector<FilmStripDiff> filmStripDiff;
    GEMUFF::VIMUFF::sMergeLines mergeLines;
    GEMUFF::VIMUFF::Frame_Diff* current_v1_item;
    GEMUFF::VIMUFF::Frame_Diff* current_v2_item;
    GEMUFF::VIMUFF::Frame_Diff* current_final_item;
    GEMUFF::VIMUFF::Diff2Player diffPlayer;
};

#endif // MAINWINDOW_H
