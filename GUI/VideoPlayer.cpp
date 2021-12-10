#include <VideoPlayer.h>

namespace GEMUFF {
    namespace VIMUFF {

        void VideoPlayer::setSlider(QSlider *_slider){
            slider = _slider;

            QObject::connect(slider, SIGNAL(valueChanged(int)),
                    this, SLOT(setTimer(int)));
        }

        void VideoPlayer::setTimer(int time){
            currentIndex = time;


                QImage v1_img((video->getFrameWidth() + 10) * bufferSize + 20,
                              video->getFrameHeight() + 20, QImage::Format_RGB32);




                v1_img.fill(Qt::black);

                int max_frames = bufferSize > (totalFrames - currentIndex) ? totalFrames - currentIndex : bufferSize;

                QPainter painter(&v1_img);

                QPen pen;
                pen.setColor(Qt::red);
                pen.setWidth(5);

               for (int i = 0; i < max_frames; i++){

                    QPoint dest(10 + (video->getFrameWidth() + 10) * i, 10);

                    QRect rect(dest.x()-10, dest.y()-10,
                               video->getFrameWidth()+10, video->getFrameHeight()+10);

                    QImage _v1 = toQImage(ImageRegister::ImageAt(video->getHashAtFrame(currentIndex+i)));

                    painter.drawImage(dest, _v1);
                }

                painter.end();
                display->clear();

                display->setPixmap(QPixmap::fromImage(v1_img));
        }




        void PatchPlayer::InternalProcess(){
            int current_index = 0;
            int current_chunk_idx = 0;

            for (int i = 0; i < diff2->diffChunks.size(); i++){

                Diff::DiffChunk _chunk = diff2->diffChunks[i];

                for (int j = 0; j < _chunk.diffData.size(); j++){

                    Diff::DiffData _da = _chunk.diffData[j];

                    if (_da.op == Diff::DO_Change){
                        DiffFramePlayer frame;
                        frame.op = _da.op;
                        frame.v1 = ImageRegister::ImageAt(_da.v1_HashPtr);
                        frame.v2 = _da.v2_Image;
                        mFrames.push_back(frame);
                    } else if (_da.op == Diff::DO_Remove){
                        DiffFramePlayer frame;
                        frame.op = _da.op;
                        frame.v1 = ImageRegister::ImageAt(_da.v1_HashPtr);
                        mFrames.push_back(frame);
                     } else if (_da.op == Diff::DO_Add){
                        DiffFramePlayer frame;
                        frame.op = _da.op;
                        frame.v2 = _da.v2_Image;
                        mFrames.push_back(frame);
                    }

                }

                if (_chunk.lcsEntry.isValid()){
                    DiffFramePlayer frame;
                    frame.v1 = ImageRegister::ImageAt(_chunk.lcsEntry.l1_ref);
                    frame.v2 = ImageRegister::ImageAt(_chunk.lcsEntry.l2_ref);
                    frame.op = Diff::DO_None;
                    mFrames.push_back(frame);
                }

            }

            /*std::vector<Hash::AbstractHashPtr> _v1Hash =
                    v1->getSequenceHash();

            while (current_index < _v1Hash.size()){

                while (current_chunk_idx < diff2->diffChunks.size()){
                    Diff::DiffChunk _chunk = diff2->diffChunks[current_chunk_idx];

                    if (_chunk.index > current_index)
                        break;

                    if (_chunk.diffData.op == Diff::DO_Change){
                        DiffFramePlayer frame;
                        frame.op = _chunk.diffData.op;
                        std::string _s = _chunk.diffData.v1_HashPtr->toString();
                        frame.v1 = ImageRegister::ImageAt(_chunk.diffData.v1_HashPtr);
                        frame.v2 = _chunk.diffData.v2_Image;
                        mFrames.push_back(frame);
                        current_index++;
                        current_chunk_idx++;
                        continue;
                    } else if (_chunk.diffData.op == Diff::DO_Remove){
                        DiffFramePlayer frame;
                        frame.op = _chunk.diffData.op;
                        frame.v1 = ImageRegister::ImageAt(_chunk.diffData.v1_HashPtr);
                        mFrames.push_back(frame);
                     } else if (_chunk.diffData.op == Diff::DO_Add){
                        DiffFramePlayer frame;
                        frame.op = _chunk.diffData.op;
                        frame.v2 = _chunk.diffData.v2_Image;
                        mFrames.push_back(frame);
                    }

                    current_chunk_idx++;
                }

                ImagePtr _img = ImageRegister::ImageAt(_v1Hash[current_index]);

                DiffFramePlayer frame;
                frame.v1 = _img;
                frame.v2 = _img;
                frame.op = Diff::DO_None;
                mFrames.push_back(frame);

                current_index++;
            }

            while (current_chunk_idx < diff2->diffChunks.size()){
                Diff::DiffChunk _chunk = diff2->diffChunks[current_chunk_idx];

                if (_chunk.index > current_index)
                    break;

                if (_chunk.diffData.op == Diff::DO_Change){
                    DiffFramePlayer frame;
                    frame.op = _chunk.diffData.op;
                    frame.v1 = ImageRegister::ImageAt(_chunk.diffData.v1_HashPtr);
                    frame.v2 = _chunk.diffData.v2_Image;
                    mFrames.push_back(frame);
                    current_index++;
                    current_chunk_idx++;
                    continue;
                } else if (_chunk.diffData.op == Diff::DO_Remove){
                    DiffFramePlayer frame;
                    frame.op = _chunk.diffData.op;
                    frame.v1 = ImageRegister::ImageAt(_chunk.diffData.v1_HashPtr);
                    mFrames.push_back(frame);
                 } else if (_chunk.diffData.op == Diff::DO_Add){
                    DiffFramePlayer frame;
                    frame.op = _chunk.diffData.op;
                    frame.v2 = _chunk.diffData.v2_Image;
                    mFrames.push_back(frame);
                }

                current_chunk_idx++;
            }*/

            slider->setMaximum(mFrames.size()-1);
        }

        void PatchPlayer::setSlider(QSlider *_slider){
            slider = _slider;

            QObject::connect(slider, SIGNAL(valueChanged(int)),
                    this, SLOT(setTimer(int)));
        }

        void PatchPlayer::setTimer(int time){
            currentIndex = time;

            DiffFramePlayer _current =
                    mFrames[currentIndex];

            QImage v1_img((diff2->base_width + 10) * bufferSize + 20,
                          diff2->base_height + 20, QImage::Format_RGB32);

            QImage v2_img((diff2->base_width+ 10) * bufferSize + 20,
                          diff2->base_height + 20, QImage::Format_RGB32);

            QImage diff_img((diff2->base_width + 10) * bufferSize + 20,
                            diff2->base_height + 20, QImage::Format_RGB32);

            v1_img.fill(Qt::black);
            v2_img.fill(Qt::black);
            diff_img.fill(Qt::black);

            int max_frames = bufferSize > (mFrames.size() - currentIndex) ? mFrames.size() - currentIndex : bufferSize;

            QPainter painter(&v1_img);
            QPainter painter2(&v2_img);
            QPainter painterDiff(&diff_img);


            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(5);


            QPen pen2;
            pen2.setColor(Qt::red);
            pen2.setWidth(5);
            painter2.setPen(pen2);



            for (int i = 0; i < max_frames; i++){
                QPoint dest(10 + (diff2->base_width + 10) * i, 10);

                QRect rect(dest.x()-10, dest.y()-10,
                           diff2->base_width+10,
                           diff2->base_height+10);


                //qDebug() << "V1 id: " << v1[currentIndex+i].id.c_str();
                //qDebug() << "V2 id: " << v2[currentIndex+i].id.c_str();

                if (mFrames[currentIndex+i].op == Diff::DO_None){
                    QImage _v1 = toQImage(mFrames[currentIndex+i].v1);
                    QImage _v2 = toQImage( mFrames[currentIndex+i].v2);

                    painter.drawImage(dest, _v1);
                    painter2.drawImage(dest, _v2);
                }

                if (mFrames[currentIndex+i].op == Diff::DO_Change){

                    QImage img1 = toQImage(mFrames[currentIndex+i].v1);
                    QImage diff = toQImage(mFrames[currentIndex+i].v2);

#ifdef VIMUFF_GPU
                    QImage img2 =  toQImage(GEMUFF::VIMUFF::ImageRegister::ProcessDIFF(
                                mFrames[currentIndex+i].v1, mFrames[currentIndex+i].v2, Device::D_GPU));
#else
                    QImage img2 = toQImage(GEMUFF::VIMUFF::ImageRegister::ProcessDIFF(
                                mFrames[currentIndex+i].v1, mFrames[currentIndex+i].v2, Device::D_CPU));
#endif

                    pen.setColor(Qt::yellow);
                    painter.setPen(pen);
                    painter.drawRect(rect);
                    painter.drawImage(dest, img1);

                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    painter2.drawImage(dest, img2);

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest, diff);
                }

                if (mFrames[currentIndex+i].op == Diff::DO_Remove){
                    pen.setColor(Qt::red);
                    painter.setPen(pen);
                    painter.drawRect(rect);
                    painter.drawImage(dest,
                        toQImage(mFrames[currentIndex+i].v1));

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest,
                        toQImage(mFrames[currentIndex+i].v1));
                }

                if (mFrames[currentIndex+i].op == Diff::DO_Add){

                    pen.setColor(Qt::green);
                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    painter2.drawImage(dest,
                       toQImage(mFrames[currentIndex+i].v2));

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest,
                       toQImage(mFrames[currentIndex+i].v2));
                }
            }

            painter.end();
            painter2.end();
            painterDiff.end();

            video_1->clear();
            video_2->clear();
            video_diff->clear();

            video_1->setPixmap(QPixmap::fromImage(v1_img));
            video_2->setPixmap(QPixmap::fromImage(v2_img));
            video_diff->setPixmap(QPixmap::fromImage(diff_img));
        }
    }
}
