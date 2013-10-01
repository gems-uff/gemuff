#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoprocessing.h"

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
    diffProcessing.CalculateDiff(&v1, &v2);

    diffPlayer.SetDiffPlayer(&diffProcessing);
    diffPlayer.SetBufferSize(5);
    diffPlayer.SetDisplays(ui->av1, ui->av2, ui->avFinal);

    qDebug() << "Frames on Main: " << diffPlayer.GetTimelineLenght();

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
    GEMUFF::VIMUFF::PatchProcessing p;
    v2 = p.Patch(&v1, diffProcessing);

    ui->patch_slider->setMinimum(0);
    ui->patch_slider->setMaximum(v2.getNumFrames()-1);
}

void MainWindow::on_slider_valueChanged(int value)
{
    //diffPlayer.BufferAtTime(value, img);
    //ui->av1->setPixmap(QPixmap::fromImage(img));
    diffPlayer.SetTime(value);

    /*
    FilmStripDiff _diff = filmStripDiff[value];

    QImage *_img1 = GEMUFF::VIMUFF::ImageRegister::ImageAt(_diff.hashV1);
    QImage *_img2 = GEMUFF::VIMUFF::ImageRegister::ImageAt(_diff.hashV2);

    ui->av1->setPixmap(QPixmap::fromImage(*_img1));
    ui->av2->setPixmap(QPixmap::fromImage(*_img2));


    // Verificar a operacao
    if (_diff.op == GEMUFF::VIMUFF::NONE)
    {
        ui->avFinal->clear();
        QPalette pallete2;
        pallete2.setColor(backgroundRole(), Qt::black);
        ui->scrollArea_3->setPalette(pallete2);
    } else if (_diff.op == GEMUFF::VIMUFF::XOR)
    {
        QImage df = QImage((uchar*)_diff.diff, _img1->width(), _img2->height(), QImage::Format_RGB32);
        ui->avFinal->setPixmap(QPixmap::fromImage(df));

        QPalette pallete;
        pallete.setColor(backgroundRole(), Qt::yellow);
        ui->scrollArea_3->setPalette(pallete);
    } else if (_diff.op == GEMUFF::VIMUFF::ADD)
    {
        QImage df = QImage((uchar*)_diff.diff, _img1->width(), _img2->height(), QImage::Format_RGB32);
        ui->avFinal->setPixmap(QPixmap::fromImage(df));

        QPalette pallete;
        pallete.setColor(backgroundRole(), Qt::green);
        ui->scrollArea_3->setPalette(pallete);
    } else if (_diff.op == GEMUFF::VIMUFF::REMOVE)
    {
        QImage df = QImage((uchar*)_diff.diff, _img1->width(), _img2->height(), QImage::Format_RGB32);
        ui->avFinal->setPixmap(QPixmap::fromImage(df));

        QPalette pallete;
        pallete.setColor(backgroundRole(), Qt::red);
        ui->scrollArea_3->setPalette(pallete);
    }*/
}

void MainWindow::on_v1_diff_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);// this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                 // tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v1.LoadVideo(file.toStdString());

    // Debug
    qDebug() <<"Image Register\n";
    GEMUFF::VIMUFF::ImageRegister::Debug();


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
    QString file = QFileDialog::getSaveFileName(this, "Save Delta");

    if (file != NULL)
    {
        //diffProcessing.SaveDiff(file.toStdString());
    }

}

void MainWindow::GenerateFilmStripDiff()
{
 /*   std::vector<std::string>& v1_strip = v1.getSequenceHash();
    std::vector<std::string>& v2_strip = v2.getSequenceHash();

    int _frame_size = v1.getFrameWidth() * v2.getFrameHeight() * 4;

    int v1_frame_index = 0;
    int v2_frame_index = 0;

    while (v1_frame_index < v1_strip.size())
    {
        // Verificar se existe operacoes nesse frame
        GEMUFF::VIMUFF::Frame_Diff *_diff =
                diffProcessing.DiffAtFrame(v1_frame_index);

        if (_diff == NULL)
        {
            FilmStripDiff _filmdiff;
            _filmdiff.op = GEMUFF::VIMUFF::NONE;
            _filmdiff.hashV1 = v1_strip[v1_frame_index++];
            _filmdiff.hashV2 = v2_strip[v2_frame_index++];
            _filmdiff.diff = NULL;

            filmStripDiff.push_back(_filmdiff);
        }
        else
        {
            // Verificar a operacao
            if (_diff->op == GEMUFF::VIMUFF::REMOVE)
            {
                for (int k = 0; k < _diff->numFrames; k++)
                {
                    FilmStripDiff _filmdiff;
                    _filmdiff.op = _diff->op;
                    _filmdiff.hashV1 = v1_strip[v1_frame_index++];
                    _filmdiff.hashV2 = v2_strip[v2_frame_index];
                    _filmdiff.diff = &_diff->buffer[_frame_size*k];

                    filmStripDiff.push_back(_filmdiff);
                }
            } else if (_diff->op == GEMUFF::VIMUFF::ADD)
            {
                for (int k = 0; k < _diff->numFrames; k++)
                {
                    FilmStripDiff _filmdiff;
                    _filmdiff.op = _diff->op;
                    _filmdiff.hashV1 = v1_strip[v1_frame_index];
                    _filmdiff.hashV2 = v2_strip[v2_frame_index++];
                    _filmdiff.diff = &_diff->buffer[_frame_size*k];

                    filmStripDiff.push_back(_filmdiff);
                }
            } else if (_diff->op == GEMUFF::VIMUFF::XOR)
            {
                for (int k = 0; k < _diff->numFrames; k++)
                {
                    FilmStripDiff _filmdiff;
                    _filmdiff.op = _diff->op;
                    _filmdiff.hashV1 = v1_strip[v1_frame_index++];
                    _filmdiff.hashV2 = v2_strip[v2_frame_index++];
                    _filmdiff.diff = &_diff->buffer[_frame_size*k];

                    filmStripDiff.push_back(_filmdiff);
                }
            }
        }
    }*/
}



void MainWindow::on_v1_load_patch_load_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
                  tr("Videos (*.avi *.mpg *.mov)"));

    if (file != NULL)
        v1.LoadVideo(file.toStdString());
}

void MainWindow::on_delta_patch_load_clicked()
{
    /*QString file = QFileDialog::getOpenFileName(this, "Delta");

    if (file != NULL)
        diffProcessing.LoadDiff(file.toStdString());*/
}

void MainWindow::on_patch_slider_valueChanged(int value)
{
    /*QImage *_img = GEMUFF::VIMUFF::ImageRegister::ImageAt(v2.getSequenceHash()[value]);
    ui->av1_2->setScaledContents(true);

    ui->av1_2->setPixmap(QPixmap::fromImage(*_img));*/
}

void MainWindow::on_btnProcessMerge_clicked()
{
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
    //QString file = QFileDialog::getOpenFileName(this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
    //              tr("Videos (*.avi *.mpg *.mov)"));

    QString file = "/Users/josericardo/Desktop/A.mov";

   /* if (file != NULL)
        v1.LoadVideo(file.toStdString());

    printf("\nV1: %s", file.toStdString().c_str());*/
}

void MainWindow::on_v2_merge_load_clicked()
{
    //QString file = QFileDialog::getOpenFileName(this, "Video", tr("Videos (*.avi *.mpg *.mov)"),
     //             tr("Videos (*.avi *.mpg *.mov)"));
    QString file = "/Users/josericardo/Desktop/C.mov";

    printf("\nV2: %s", file.toStdString().c_str());

    if (file != NULL)
        v2.LoadVideo(file.toStdString());
}

void MainWindow::on_merge_slider_valueChanged(int value)
{
/*
    GEMUFF::VIMUFF::sMergeFrame* _basef = &mergeLines.channelBase[value];
    GEMUFF::VIMUFF::sMergeFrame* _af = &mergeLines.channelA[value];
    GEMUFF::VIMUFF::sMergeFrame* _bf = &mergeLines.channelB[value];

    if (_basef->show)
    {
        QImage *_img1 = GEMUFF::VIMUFF::ImageRegister::ImageAt(_basef->frame_by_key);
        ui->base_merge->setPixmap(QPixmap::fromImage(*_img1));
        ui->lbl_base_paused->clear();
    } else
        ui->lbl_base_paused->setText("Paused");


    if (_af->show)
    {
        ui->lbl_v1_paused->clear();

        switch (_af->op)
        {
            case GEMUFF::VIMUFF::XOR:
            {
                QImage *_imgb = GEMUFF::VIMUFF::ImageRegister::ImageAt(_basef->frame_by_key);

                uchar* _final = (uchar*) malloc(sizeof(uchar) * 4 * _imgb->width() * _imgb->height());

                gIMUFFPatch(_imgb->constBits(),
                        (uchar*)_af->data, &_final[0], _imgb->width() * _imgb->height());


                QImage _v2 = QImage(_final,
                            _imgb->width(), _imgb->height(), QImage::Format_RGB32);

                ui->av1_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::yellow);
                ui->m_v1_sc->setPalette(pallete);

            }
            break;

            case GEMUFF::VIMUFF::ADD:
            {
                QImage _v2 = QImage(_af->data,
                            v1.getFrameWidth(), v1.getFrameHeight(), QImage::Format_RGB32);

                ui->av1_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::green);
                ui->m_v1_sc->setPalette(pallete);
            }
            break;

            case GEMUFF::VIMUFF::REMOVE:
            {
                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::red);
                ui->m_v1_sc->setPalette(pallete);
            }
            break;

            case GEMUFF::VIMUFF::NONE:
            {
                QImage *_imgb = GEMUFF::VIMUFF::ImageRegister::ImageAt(_af->frame_by_key);

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::gray);
                ui->av1_merge->setPixmap(QPixmap::fromImage(*_imgb));
                ui->m_v1_sc->setPalette(pallete);
            }
            break;
        }
    }
    else
        ui->lbl_v1_paused->setText("Paused");



    if (_bf->show)
    {
        ui->lbl_v2_paused->clear();

        switch (_bf->op)
        {
            case GEMUFF::VIMUFF::XOR:
            {
                QImage *_imgb = GEMUFF::VIMUFF::ImageRegister::ImageAt(_basef->frame_by_key);

                uchar* _final = (uchar*) malloc(sizeof(uchar) * 4 * _imgb->width() * _imgb->height());

                gIMUFFPatch(_imgb->constBits(),
                        (uchar*)_bf->data, &_final[0], _imgb->width() * _imgb->height());


                QImage _v2 = QImage(_final,
                            _imgb->width(), _imgb->height(), QImage::Format_RGB32);

                ui->av2_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::yellow);
                ui->m_v2_sc->setPalette(pallete);

            }
            break;

            case GEMUFF::VIMUFF::REMOVE:
            {
                ui->lbl_v2_paused->setText("Removed");
                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::red);
                ui->m_v2_sc->setPalette(pallete);
            }
            break;

            case GEMUFF::VIMUFF::ADD:
            {
                QImage _v2 = QImage(_bf->data,
                            v2.getFrameWidth(), v2.getFrameHeight(), QImage::Format_RGB32);

                ui->av2_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::green);
                ui->m_v2_sc->setPalette(pallete);
            }
            break;

            case GEMUFF::VIMUFF::NONE:
            {
                QImage *_imgb = GEMUFF::VIMUFF::ImageRegister::ImageAt(_bf->frame_by_key);

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::gray);
                ui->av2_merge->setPixmap(QPixmap::fromImage(*_imgb));
                ui->m_v2_sc->setPalette(pallete);
            }
            break;
        }
    }
    else
        ui->lbl_v2_paused->setText("Paused");


    // Canal Merge
    GEMUFF::VIMUFF::sMergeResult* mergeResult = &mergeLines.channelMerge[value];

    // Conflito?
    if (mergeResult->conflicted)
    {
        ui->lbl_merge_paused->setText("Conflito!");
    } else
    {
        ui->lbl_merge_paused->clear();

        // Operacao
        switch (mergeResult->op)
        {
            case GEMUFF::VIMUFF::XOR:
            {
                QImage *_imgb = GEMUFF::VIMUFF::ImageRegister::ImageAt(_basef->frame_by_key);

                uchar* _final = (uchar*) malloc(sizeof(uchar) * 4 * _imgb->width() * _imgb->height());

                gIMUFFPatch(_imgb->constBits(),
                        (uchar*)mergeResult->data, &_final[0], _imgb->width() * _imgb->height());


                QImage _v2 = QImage(_final,
                            _imgb->width(), _imgb->height(), QImage::Format_RGB32);

                ui->av_final_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::yellow);
                ui->m_merge_sc->setPalette(pallete);
            }
            break;


            case GEMUFF::VIMUFF::ADD:
            {
                QImage _v2 = QImage(mergeResult->data,
                            v2.getFrameWidth(), v2.getFrameHeight(), QImage::Format_RGB32);

                ui->av_final_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::green);
                ui->m_merge_sc->setPalette(pallete);
            }
            break;

            case GEMUFF::VIMUFF::NONE:
            {
                QImage *_imgb = GEMUFF::VIMUFF::ImageRegister::ImageAt(_bf->frame_by_key);

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::gray);
                ui->av_final_merge->setPixmap(QPixmap::fromImage(*_imgb));
                ui->m_merge_sc->setPalette(pallete);
            }
            break;
        }

        if (mergeResult->paused)
           ui->lbl_merge_paused->setText("Paused");

    }*/


    /*else
    {

    }

    if (item->v2_frame_diff != NULL)
    {
        switch (item->v2_frame_diff->op)
        {
            case GEMUFF::VIMUFF::XOR:
            {
                QImage _v2 = QImage((uchar*)&item->v2_frame_diff->buffer[item->v2_offset],
                                _img1->width(), _img1->height(), QImage::Format_RGB32);
                ui->av2_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::yellow);
                ui->m_v2_sc->setPalette(pallete);

            }
            break;

            case GEMUFF::VIMUFF::ADD:
            {
                current_v2_item = item->v2_frame_diff;
                ui->merge_v2_slider->setMinimum(0);
                ui->merge_v2_slider->setMaximum(current_v2_item->numFrames-1);
                QImage _v2 = QImage((uchar*)&current_v2_item->buffer[0],
                            _img1->width(), _img1->height(), QImage::Format_RGB32);
                ui->av2_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::blue);
                ui->m_v2_sc->setPalette(pallete);
            }
            break;

            case GEMUFF::VIMUFF::REMOVE:
            {
                current_v2_item = item->v2_frame_diff;
                ui->merge_v2_slider->setMinimum(0);
                ui->merge_v2_slider->setMaximum(current_v2_item->numFrames-1);
                QImage _v2 = QImage((uchar*)&current_v2_item->buffer[0],
                        _img1->width(), _img1->height(), QImage::Format_RGB32);
                ui->av2_merge->setPixmap(QPixmap::fromImage(_v2));

                QPalette pallete;
                pallete.setColor(backgroundRole(), Qt::darkMagenta);
                ui->m_v2_sc->setPalette(pallete);
            }
            break;
        }
    }
    else
    {
        QPalette pallete;
        pallete.setColor(backgroundRole(), Qt::black);
        ui->m_v2_sc->setPalette(pallete);
        ui->av2_merge->clear();
    }

    if (item->v1_frame_diff != NULL && item->v2_frame_diff != NULL)
    {
        // conflito

    }
    else if (item->v1_frame_diff != NULL)
    {
        switch (item->v1_frame_diff->op)
        {
            case GEMUFF::VIMUFF::XOR:
            {

                uchar* _final = (uchar*) malloc(sizeof(uchar) * 4 * _img1->width() * _img1->height());

                gIMUFFPatch(_img1->constBits(),
                            &item->v1_frame_diff->buffer[item->v1_offset],
                            &_final[0], _img1->width() * _img1->height());


                QImage _v2 = QImage(_final,
                                _img1->width(), _img1->height(), QImage::Format_RGB32);
                ui->av_final_merge->setPixmap(QPixmap::fromImage(_v2));
            }
            break;
        }
    }
    else if (item->v2_frame_diff != NULL)
    {
        switch (item->v2_frame_diff->op)
        {
            case GEMUFF::VIMUFF::XOR:
            {

                uchar* _final = (uchar*) malloc(sizeof(uchar) * 4 * _img1->width() * _img1->height());

                gIMUFFPatch(_img1->constBits(),
                            &item->v2_frame_diff->buffer[item->v2_offset],
                            &_final[0], _img1->width() * _img1->height());


                QImage _v2 = QImage(_final,
                                _img1->width(), _img1->height(), QImage::Format_RGB32);
                ui->av_final_merge->setPixmap(QPixmap::fromImage(_v2));
            }
            break;
        }
    }
    else
    {
        ui->av_final_merge->setPixmap(QPixmap::fromImage(*_img1));
    }*/



    // Video 2

}
