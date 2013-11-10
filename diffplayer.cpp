#include "diffplayer.h"

namespace GEMUFF {
    namespace VIMUFF {

        Diff2Player::Diff2Player(){
            numChannels = 2;
            currentIndex = 0;
        }

        void Diff2Player::SetDisplays(QLabel *_v1, QLabel *_v2, QLabel *_diff){
            video_1 = _v1;
            video_2 = _v2;
            video_diff = _diff;
        }

        void Diff2Player::SetDiffPlayer(Diff2Processing *diff){
            diffProcessing = diff;

            GenerateDiffPlayerData();
        }

        int Diff2Player::GetTimelineLenght(){
            return mFrames.size();
        }

        void Diff2Player::GenerateDiffPlayerData(){
            std::vector<NodeDiff2>& diffNodes = diffProcessing->getDiffData();

            for (int i = 0; i < diffNodes.size(); i++){

                if (diffNodes[i].id_LCS != NULL){
                    DiffFramePlayer frame;
                    frame.v1 = frame.v2 = diffNodes[i].id_LCS;
                    frame.op = OP_NONE;
                    mFrames.push_back(frame);
                }

                for (int k = 0; k < diffNodes[i].frame_diff.size(); k++){

                    FrameDiff2 _currentFrameDiff = diffNodes[i].frame_diff[k];

                    if (_currentFrameDiff.v1.op == OP_CHANGED){
                        DiffFramePlayer frame;
                        frame.op = OP_CHANGED;
                        frame.v2 = _currentFrameDiff.v2.frame_id;
                        frame.v1 = _currentFrameDiff.v1.frame_id;
                        mFrames.push_back(frame);
                    }
                }

                for (int k = 0; k < diffNodes[i].frame_diff.size(); k++){

                    FrameDiff2 _currentFrameDiff = diffNodes[i].frame_diff[k];

                    if (_currentFrameDiff.v1.op == OP_REMOVED){
                        DiffFramePlayer frame;
                        frame.op = OP_REMOVED;
                        frame.v1 = _currentFrameDiff.v1.frame_id;
                        //frame.v2 = NULL;
                        mFrames.push_back(frame);
                    }
                }

                for (int k = 0; k < diffNodes[i].frame_diff.size(); k++){

                    FrameDiff2 _currentFrameDiff = diffNodes[i].frame_diff[k];

                    if (_currentFrameDiff.v2.op == OP_ADD){
                        DiffFramePlayer frame;
                        frame.op = OP_ADD;
                        frame.v2 = _currentFrameDiff.v2.frame_id;
                        //frame.v1 = NULL;
                        mFrames.push_back(frame);
                    }
                }
            }

            qDebug() << "Frames: " << mFrames.size();
        }

        void Diff2Player::SetTime(int time){
            currentIndex = time;

            QImage v1_img((diffProcessing->getFrameWidth() + 10) * bufferSize + 20,
                          diffProcessing->getFrameHeight() + 20, QImage::Format_RGB32);

            QImage v2_img((diffProcessing->getFrameWidth() + 10) * bufferSize + 20,
                          diffProcessing->getFrameHeight() + 20, QImage::Format_RGB32);

            QImage diff_img((diffProcessing->getFrameWidth() + 10) * bufferSize + 20,
                            diffProcessing->getFrameHeight() + 20, QImage::Format_RGB32);

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
                QPoint dest(10 + (diffProcessing->getFrameWidth() + 10) * i, 10);

                QRect rect(dest.x()-10, dest.y()-10,
                           diffProcessing->getFrameWidth()+10, diffProcessing->getFrameHeight()+10);


                //qDebug() << "V1 id: " << v1[currentIndex+i].id.c_str();
                //qDebug() << "V2 id: " << v2[currentIndex+i].id.c_str();

                if (mFrames[currentIndex+i].op == OP_NONE){
                    QImage _v1 = ImageRegister::ImageAt(mFrames[currentIndex+i].v1)->toQImage();
                    QImage _v2 = ImageRegister::ImageAt(mFrames[currentIndex+i].v2)->toQImage();

                    painter.drawImage(dest, _v1);
                    painter2.drawImage(dest, _v2);
                }

                if (mFrames[currentIndex+i].op == OP_CHANGED){

                    QImage img1 = ImageRegister::ImageAt(mFrames[currentIndex+i].v1)->toQImage();
                    QImage img2 = ImageRegister::ImageAt(mFrames[currentIndex+i].v2)->toQImage();
                    QImage diff = ImageRegister::ProcessGPUDiff(&img1, &img2);

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

                if (mFrames[currentIndex+i].op == OP_REMOVED){
                    pen.setColor(Qt::red);
                    painter.setPen(pen);
                    painter.drawRect(rect);
                    painter.drawImage(dest,
                        ImageRegister::ImageAt(mFrames[currentIndex+i].v1)->toQImage());

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest,
                        ImageRegister::ImageAt(mFrames[currentIndex+i].v1)->toQImage());
                }

                if (mFrames[currentIndex+i].op == OP_ADD){
                    pen.setColor(Qt::green);
                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    painter2.drawImage(dest,
                       ImageRegister::ImageAt(mFrames[currentIndex+i].v2)->toQImage());

                    painterDiff.setPen(pen);
                    painterDiff.drawRect(rect);
                    painterDiff.drawImage(dest,
                       ImageRegister::ImageAt(mFrames[currentIndex+i].v2)->toQImage());
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
