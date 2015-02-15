#include "mainwindow.h"
#include <ui_mainwindow.h>
//#include "videoprocessing.h"

/*IplImage* QImage2IplImage(QImage *qimg)
{
    IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg->width(), qimg->height()), IPL_DEPTH_8U, 4);
    imgHeader->imageData = (char*) qimg->bits();

    uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->byteCount());
    memcpy(newdata, qimg->bits(), qimg->byteCount());
    imgHeader->imageData = (char*) newdata;
    //cvClo
    return imgHeader;
}*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gridLayout = new QGridLayout();
    imgDisplayLabel = new QLabel("");
    scrollArea = new QScrollArea();

    avcodec_register_all();
    av_register_all();
}

MainWindow::~MainWindow()
{
    delete ui;

    delete imgDisplayLabel;
    delete scrollArea;
    delete gridLayout;
}

void MainWindow::on_pushButton_clicked()
{
    // setar o slider
    ui->slider->setMinimum(0);

    // Processar a diferenca
    diff2Info = GEMUFF::Diff::Diff2(&v1, &v2, 0.3f);
    diffPlayer.SetVideo(&v1);
    diffPlayer.SetData(&diff2Info);
    diffPlayer.SetBufferSize(5);
    diffPlayer.SetDisplays(ui->av1, ui->av2, ui->avFinal);


    //qDebug() << "Frames on Main: " << diffPlayer.GetTimelineLenght();

    ui->slider->setMaximum(diffPlayer.GetTimelineLenght()-1);

    ui->avFinal->setScaledContents(false);
    ui->av1->setScaledContents(true);
    ui->av2->setScaledContents(true);
    ui->avFinal->setScaledContents(true);



/*    vp.Diff("/Users/josericardo/Desktop/Sequence_04.mov",
                          "/Users/josericardo/Desktop/Sequence05.mov");

    // setar o slider
    ui->slider->setMinimum(0);
    qDebug() << vp.frame_collection.size();

    ui->slider->setMaximum(vp.frame_collection.size()-1);

    ui->av1->setScaledContents(true);
    ui->av2->setScaledContents(true);
    ui->avFinal->setScaledContents(true);*/

/*    IplImage *frame_v1, *frame_v2;

    int f_count = (int) cvGetCaptureProperty(v1, CV_CAP_PROP_FRAME_COUNT);
    int fps = (int) cvGetCaptureProperty(v1, CV_CAP_PROP_FPS);
    int frame_width=(int) cvGetCaptureProperty(v1, CV_CAP_PROP_FRAME_WIDTH);     //get the width of frame
    int frame_height=(int) cvGetCaptureProperty(v1, CV_CAP_PROP_FRAME_HEIGHT);   //get the height of

    printf("\n%d\n", fps);

    frame_v1 = cvQueryFrame(v1);
    frame_v2 = cvQueryFrame(v2);

    //CvVideoWriter *vwriter = NULL;

    /*vwriter = cvCreateVideoWriter("/Users/josericardo/result.avi",
                                                  CV_FOURCC('I', 'Y', 'U', 'V'),
                                                 fps,
                                                 cvSize(frame_width, frame_height),
                                                 1);


    if (!vwriter) printf("\n%s", "Error creating writer!");

    QImage result(frame_v1->width * f_count, frame_v1->height, QImage::Format_ARGB32);
    QPainter painter (&result);

    bool play = true;
    //int x_offset = 0;

    int i = 0;
    while (frame_v1 && play)
    {
        IplImage *img_1 = cvCloneImage(frame_v1);
        IplImage *img_2 = cvCloneImage(frame_v2);


  //      if (img_1->origin)
    //    {
      //      cvFlip(img);
        //    img->origin=0;
       // }

        QImage qimg_1, qimg_2;
        qimg_1 = IplImage2QImage(img_1);
        qimg_2 = IplImage2QImage(img_2);

        ui->myLabel->setPixmap(QPixmap::fromImage(qimg_1));
        qApp->processEvents();
        // Processar a diferenca
        uchar* delta = (uchar*) malloc(sizeof(uchar) * qimg_1.width() * qimg_2.height() * 4);
        //cutCreateTimer(&timer);
        //cutStartTimer(timer);
        gIMUFFDiff(qimg_1.constBits(), qimg_2.constBits(), delta, qimg_1.width() * qimg_1.height());
        QImage img_c = QImage(delta, qimg_1.width(), qimg_1.height(), QImage::Format_ARGB32);
        QString str("Img");
        str.append(QString("%1").arg(i));
        img_c.save("/Users/josericardo/Desktop/img/"+str+".jpg");
        //IplImage* dt = QImage2IplImage(&img_c);
        //cvWriteFrame(vwriter, dt);

        i++;
        free(delta);

        //cutStopTimer(timer);
        //ui->lblTProc->setText(QString::number((float)cutGetTimerValue(timer)) + " ms ");
        //cutDeleteTimer(timer);
        // Converter para o Opencv

        // Salvar a diferenca em um filme

        //painter.drawImage(x_offset, 0, qimg);
        //ui->myLabel->setPixmap(QPixmap::fromImage(qimg));
        //qApp->processEvents();
        //x_offset += img->width;
        cvReleaseImage(&img_1);
        cvReleaseImage(&img_2);

        cvWaitKey(10);


        frame_v1=cvQueryFrame(v1);
        frame_v2=cvQueryFrame(v2);
    }
    painter.end();

    //ui->myLabel->setPixmap(QPixmap::fromImage(result));
   // ui->myLabel->setScaledContents(false);
   // ui->myLabel->resize(100, result.height());
    //result.save("/Users/josericardo/Teste.jpg");
    //imgDisplayLabel->setPixmap(QPixmap::fromImage(result));
    //imgDisplayLabel->adjustSize();

    //scrollArea->setWidget(imgDisplayLabel);
    //scrollArea->setMinimumSize(256,256);
    //scrollArea->setMaximumSize(512,512);
    //scrollArea->show();
    //setLayout(gridLayout);

    cvReleaseCapture(&v1);
    cvReleaseCapture(&v2);
    //cvReleaseVideoWriter(&vwriter);
    printf("\n%s", "Writer released!");*/
}

void MainWindow::on_pushButton_2_clicked()
{
    /*GEMUFF::VIMUFF::PatchProcessing p;
    v2 = p.Patch(&v1, diffProcessing);

    ui->patch_slider->setMinimum(0);
    ui->patch_slider->setMaximum(v2.getNumFrames()-1);*/
}

void MainWindow::on_slider_valueChanged(int value)
{
    //diffPlayer.BufferAtTime(value, img);
    //ui->av1->setPixmap(QPixmap::fromImage(img));
    qDebug() << value;
    diffPlayer.SetTime(value);

}

void MainWindow::on_v1_diff_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v1.LoadVideo(file.toStdString());

    /*std::vector<std::string>& seq_hash = v1.getSequenceHash();

    // Carregar primeiro frame do video
    if (seq_hash.size() > 0)
    {
        QImage* img_1 = GEMUFF::VIMUFF::ImageRegister::ImageAt(seq_hash[0]);

        ui->av1->setScaledContents(true);
        ui->av1->setPixmap(QPixmap::fromImage(*img_1));
    }*/
}

void MainWindow::on_v2_diff_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);//, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                  //tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v2.LoadVideo(file.toStdString());

    // Carregar primeiro frame do video
    /*if (v2.getSequenceHash().size() > 0)
    {
        QImage* img = GEMUFF::VIMUFF::ImageRegister::ImageAt(v1.getSequenceHash()[0]);

        ui->av2->setScaledContents(true);
        ui->av2->setPixmap(QPixmap::fromImage(*img));
    }*/
}

void MainWindow::on_delta_save_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, "Save Delta",
        QDir::currentPath(), "VIMUFF Delta (*.vimuff)", new QString("VIMUFF Delta (*.vimuff"));

    if (file != NULL)
    {
        std::ofstream _diffFile;

        _diffFile.open(file.toStdString().c_str(),
                       std::ios::binary | std::ios::out | std::ios::app);
        diff2Info.write(_diffFile);

        _diffFile.close();
    }
}


void MainWindow::on_v1_load_patch_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                  tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v2.LoadVideo(file.toStdString());
}

void MainWindow::on_delta_patch_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Delta");

    //if (file != NULL)
      //  diff2Data.Load("Teste");
}

void MainWindow::on_patch_slider_valueChanged(int value)
{
    /*QImage *_img = GEMUFF::VIMUFF::ImageRegister::ImageAt(v2.getSequenceHash()[value]);
    ui->av1_2->setScaledContents(true);

    ui->av1_2->setPixmap(QPixmap::fromImage(*_img));*/
}

void MainWindow::on_btnProcessMerge_clicked()
{
   ui->merge_slider->setMinimum(0);

    diff3Info = GEMUFF::Diff::Diff3(&base, &v1, &v2, 0.3);
    mergePlayer.SetDisplays(ui->lblBaseMerge, ui->lblV1Merge, ui->lblV2Merge, ui->lblResultMerge);
    mergePlayer.SetBase(&base);
    mergePlayer.SetData(&diff3Info);
    mergePlayer.SetBufferSize(5);



    //qDebug() << "Frames on Main: " << diffPlayer.GetTimelineLenght();

    ui->merge_slider->setMaximum(mergePlayer.GetTimelineLenght()-1);

    ui->lblBaseMerge->setScaledContents(true);
    ui->lblV1Merge->setScaledContents(true);
    ui->lblV2Merge->setScaledContents(true);
    ui->lblResultMerge->setScaledContents(true);



    /*mergeProcessing.Merge(&base, &v1, &v2);

    // Recuperar a tabela de merge
    mergeLines = mergeProcessing.getMergeChannels();

    ui->merge_slider->setMinimum(0);
    ui->merge_slider->setMaximum(mergeLines.channelBase.size() - 1);
    ui->av1_merge->setScaledContents(true);
    ui->base_merge->setScaledContents(true);
    ui->av2_merge->setScaledContents(true);
    ui->av_final_merge->setScaledContents(true);
    mergeProcessing.DebugMergeTable();*/
}

void MainWindow::on_base_load_merge_clicked()
{
    //QString file = QFileDialog::getOpenFileName(this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
      //            tr("Videos (*.avi *.mpg *.mov)"));
    /*QString file = "/Users/josericardo/Desktop/Sequence_04.mov";

    if (file != NULL)
        base.LoadVideo(file.toStdString());

    printf("\nBase: %s", file.toStdString().c_str());*/
}

void MainWindow::on_v1_merge_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v1.LoadVideo(file.toStdString());
}

void MainWindow::on_v2_merge_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v2.LoadVideo(file.toStdString());
}

void MainWindow::on_merge_slider_valueChanged(int value)
{
    mergePlayer.SetTime(value);
}

void MainWindow::on_delta_open_clicked()
{

}

void MainWindow::on_btnLoadV1_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v1.LoadVideo(file.toStdString());
}

void MainWindow::on_btnDiffLoad_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL){
        std::ifstream _delta;
        _delta.open(file.toStdString().c_str(),
                   std::ios::binary | std::ios::out | std::ios::app);
        diff2Info.read(_delta);
        diff2Info.Debug();
        _delta.close();
    }
}

void MainWindow::on_btnPatchProcess_clicked()
{
    // setar o slider
    ui->slider->setMinimum(0);

    // Processar a diferenca
    patchPlayer.setVideo(&v1);
    patchPlayer.setDisplay(ui->lblV1_to_Patch, ui->lblDiff, ui->lblPatchResult);
    patchPlayer.setSlider(ui->patch_slider);
    //patchPlayer.SetBufferSize(5);
    patchPlayer.Process(&diff2Info);


    //qDebug() << "Frames on Main: " << diffPlayer.GetTimelineLenght();

    //ui->slider->setMaximum(diffPlayer.GetTimelineLenght()-1);

    ui->lblV1_to_Patch->setScaledContents(true);
    ui->lblDiff->setScaledContents(true);
    ui->lblPatchResult->setScaledContents(true);



}

void MainWindow::on_btn_saveVideoPatched_clicked()
{
    GEMUFF::VIMUFF::Video videoPatched =
            GEMUFF::Diff::Patch(diff2Info, &v1);


    // v_t.LoadFromImages(v1.getSequenceHash(), 960, 540,
      //                  AV_CODEC_ID_RAWVIDEO, AV_PIX_FMT_RGB32, AV_PIX_FMT_YUV420P, 400000, 30, v1.getFormatContext());

}

void MainWindow::on_base_merge_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        base.LoadVideo(file.toStdString());
}
