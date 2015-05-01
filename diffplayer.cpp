#include "diffplayer.h"

namespace GEMUFF {
    namespace VIMUFF {

        Diff2Player::Diff2Player(){
            numChannels = 2;
            currentIndex = 0;
            video_1 = video_2 = video_diff = NULL;
        }

        void Diff2Player::SetDisplays(QLabel *_v1, QLabel *_v2, QLabel *_diff){
            video_1 = _v1;
            video_2 = _v2;
            video_diff = _diff;
        }

        void Diff2Player::Clear(){
            if (video_1 != NULL) video_1->clear();
            if (video_2 != NULL) video_2->clear();
            if (video_diff != NULL) video_diff->clear();

            mFrames.clear();

            currentIndex = 0;
        }


        void Diff2Player::SetData(Diff::Diff2Info *_diff2){
            diff2 = _diff2;

            GenerateDiffPlayerData();
        }

        int Diff2Player::GetTimelineLenght(){
            return mFrames.size();
        }

        void Diff2Player::GenerateDiffPlayerData(){

            int current_index = 0;
            int current_chunk_idx = 0;

            std::vector<Hash::AbstractHashPtr> _v1Hash =
                    video1->getSequenceHash();

            while (current_index < _v1Hash.size()){

                int cur_offset = 0;

                while (current_chunk_idx < diff2->diffChunks.size()){
                    Diff::DiffChunk _chunk = diff2->diffChunks[current_chunk_idx];

                    if (_chunk.index > current_index)
                        break;

                    if (_chunk.diffData.op == Diff::DO_Change){
                        DiffFramePlayer frame;
                        frame.op = _chunk.diffData.op;
                        frame.v1 = _chunk.diffData.v1_Image;
                        frame.v2 = _chunk.diffData.v2_Image;
                        mFrames.push_back(frame);
                        cur_offset++;
                        //current_chunk_idx++;
                        //continue;
                    } else if (_chunk.diffData.op == Diff::DO_Remove){
                        DiffFramePlayer frame;
                        frame.op = _chunk.diffData.op;
                        frame.v1 = _chunk.diffData.v1_Image;
                        mFrames.push_back(frame);
                        cur_offset++;
                     } else if (_chunk.diffData.op == Diff::DO_Add){
                        DiffFramePlayer frame;
                        frame.op = _chunk.diffData.op;
                        frame.v2 = _chunk.diffData.v2_Image;
                        mFrames.push_back(frame);
                    }

                    current_chunk_idx++;
                }

                current_index += cur_offset;

                if (current_index < _v1Hash.size()){

                    ImagePtr _img = ImageRegister::ImageAt(_v1Hash[current_index]);

                    DiffFramePlayer frame;
                    frame.v1 = _img;
                    frame.v2 = _img;
                    frame.op = Diff::DO_None;
                    mFrames.push_back(frame);


                }
                current_index++;
            }


            while (current_chunk_idx < diff2->diffChunks.size()){
                Diff::DiffChunk _chunk = diff2->diffChunks[current_chunk_idx];

                if (_chunk.index > current_index)
                    break;

                if (_chunk.diffData.op == Diff::DO_Change){
                    DiffFramePlayer frame;
                    frame.op = _chunk.diffData.op;
                    frame.v1 = _chunk.diffData.v1_Image;
                    frame.v2 = _chunk.diffData.v2_Image;
                    mFrames.push_back(frame);
                    current_index++;
                    current_chunk_idx++;
                    continue;
                } else if (_chunk.diffData.op == Diff::DO_Remove){
                    DiffFramePlayer frame;
                    frame.op = _chunk.diffData.op;
                    frame.v1 = _chunk.diffData.v1_Image;
                    mFrames.push_back(frame);
                 } else if (_chunk.diffData.op == Diff::DO_Add){
                    DiffFramePlayer frame;
                    frame.op = _chunk.diffData.op;
                    frame.v2 = _chunk.diffData.v2_Image;
                    mFrames.push_back(frame);
                }

                current_chunk_idx++;
            }

            //qDebug() << "Frames: " << mFrames.size();
        }


        void Diff2Player::SetTime(int time){
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
                    QImage _v1 = mFrames[currentIndex+i].v1->toQImage();
                    QImage _v2 = mFrames[currentIndex+i].v2->toQImage();

                    painter.drawImage(dest, _v1);
                    painter2.drawImage(dest, _v2);
                }

                if (mFrames[currentIndex+i].op == Diff::DO_Change){

                    QImage img1 = mFrames[currentIndex+i].v1->toQImage();
                    QImage diff = mFrames[currentIndex+i].v2->toQImage();
                    QImage img2 = ImageRegister::ProcessGPUDiff(&img1, &diff);

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
                        mFrames[currentIndex+i].v1->toQImage());

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest,
                        mFrames[currentIndex+i].v1->toQImage());
                }

                if (mFrames[currentIndex+i].op == Diff::DO_Add){
                    pen.setColor(Qt::green);
                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    painter2.drawImage(dest,
                       mFrames[currentIndex+i].v2->toQImage());

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest,
                       mFrames[currentIndex+i].v2->toQImage());
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
