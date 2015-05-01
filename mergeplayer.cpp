
#include "mergeplayer.h"

namespace GEMUFF {
    namespace VIMUFF {

        MergePlayer::MergePlayer(){
            numChannels = 2;
            currentIndex = 0;
        }

        void MergePlayer::SetDisplays(QLabel* _vbase, QLabel* _v1, QLabel* _v2, QLabel* _merge){

            video_base = _vbase;
            video_1 = _v1;
            video_2 = _v2;
            video_merge = _merge;
        }


        void MergePlayer::SetData(Diff::Diff3Info *_diff3){
            diff3 = _diff3;

            GeneratePlayerData();
        }

        int MergePlayer::GetTimelineLenght(){
            return mFrames.size();
        }

        void MergePlayer::GeneratePlayerData(){

            int currentLCSIdx = 0;
            int current_base_idx = 0;
            int current_chunk_idx = 0;


            std::vector<Hash::AbstractHashPtr> _vbaseseq =
                    vbase->getSequenceHash();

            while (currentLCSIdx < diff3->lcs.size()){
                // Locate the lcs in all lists
                int idx_base_lcs = Diff::FindKey(diff3->lcs[currentLCSIdx].l1_ref, _vbaseseq, current_base_idx, 1.0f);

                bool haschanged = false;

                    if (current_chunk_idx < diff3->diff3chunks.size()){
                        Diff::Diff3Chunk _chunk = diff3->diff3chunks[current_chunk_idx];

                        if (_chunk.index == current_base_idx){

                            // Check for conflicts
                            if (_chunk.basev1.size() > 0 && _chunk.basev2.size() > 0){

                                int max = _chunk.basev1.size() > _chunk.basev2.size() ? _chunk.basev1.size() : _chunk.basev2.size();

                                for (int i = 0; i < max; i++){


                                    FramePlayer frame;

                                    if (i < _chunk.basev1.size()){
                                        frame.op_b_v1 = _chunk.basev1[i].op;

                                        if (frame.op_b_v1 != Diff::DO_Remove)
                                            frame.v1 = _chunk.basev1[i].v1_Image;

                                        if (frame.op_b_v1 == Diff::DO_Change)
                                            haschanged = true;
                                    }

                                    if (i < _chunk.basev2.size()){
                                        frame.op_b_v2 = _chunk.basev2[i].op;

                                        if (frame.op_b_v2 != Diff::DO_Remove)
                                            frame.v2 = _chunk.basev2[i].v1_Image;

                                        if (frame.op_b_v2 == Diff::DO_Change)
                                            haschanged = true;
                                    }

                                    if (current_base_idx < idx_base_lcs ){
                                        frame.base = ImageRegister::ImageAt(_vbaseseq[current_base_idx++]);
                                    } else if (current_base_idx == idx_base_lcs && idx_base_lcs == diff3->lcs.size()-1){
                                        frame.base = ImageRegister::ImageAt(_vbaseseq[current_base_idx++]);
                                    }


                                    mFrames.push_back(frame);
                                }

                            } else { // Only one side has modified


                                if (_chunk.basev1.size() > 0){

                                    for (int i = 0; i < _chunk.basev1.size(); i++){
                                        FramePlayer frame;

                                        if (_chunk.basev1[i].op == Diff::DO_Change){
                                            frame.op_b_v1 = _chunk.basev1[i].op;
                                            frame.base = _chunk.basev1[i].v1_Image;
                                            frame.v1 = _chunk.basev1[i].v2_Image;
                                            frame.v2 = _chunk.basev1[i].v1_Image;
                                            frame.op_b_v2 = Diff::DO_None;
                                            current_base_idx++;
                                            haschanged = true;
                                        } else if (_chunk.basev1[i].op == Diff::DO_Remove){
                                            frame.op_b_v1 = _chunk.basev1[i].op;
                                            frame.base = _chunk.basev1[i].v1_Image;
                                            frame.op_b_v2 = Diff::DO_None;
                                            frame.v2 = _chunk.basev1[i].v1_Image;
                                            current_base_idx++;
                                        } else if (_chunk.basev1[i].op == Diff::DO_Add){
                                            frame.op_b_v1 = _chunk.basev1[i].op;
                                            frame.op_b_v2 = Diff::DO_None;
                                            frame.v1 = _chunk.basev1[i].v2_Image;
                                        }

                                        mFrames.push_back(frame);
                                    }


                                } else if (_chunk.basev2.size() > 0) {

                                    for (int i = 0; i < _chunk.basev2.size(); i++){
                                        FramePlayer frame;

                                        if (_chunk.basev2[i].op == Diff::DO_Change){
                                            frame.op_b_v2 = _chunk.basev2[i].op;
                                            frame.base = _chunk.basev2[i].v1_Image;
                                            frame.v2 = _chunk.basev2[i].v2_Image;
                                            frame.v1 = _chunk.basev2[i].v1_Image;
                                            frame.op_b_v1 = Diff::DO_None;
                                            current_base_idx++;
                                            mFrames.push_back(frame);
                                            haschanged = true;
                                        } else if (_chunk.basev2[i].op == Diff::DO_Remove){
                                            frame.op_b_v2 = _chunk.basev2[i].op;
                                            frame.base = _chunk.basev2[i].v1_Image;
                                            frame.v1 = _chunk.basev2[i].v1_Image;
                                            frame.op_b_v1 = Diff::DO_None;
                                            current_base_idx++;
                                        } else if (_chunk.basev2[i].op == Diff::DO_Add){
                                            frame.op_b_v2 = _chunk.basev2[i].op;
                                            frame.op_b_v1 = Diff::DO_None;
                                            frame.v2 = _chunk.basev2[i].v2_Image;
                                        }

                                        mFrames.push_back(frame);
                                    }

                                }

                            }



                            current_chunk_idx++;
                        }

                    }


                    if (!haschanged){
                       while (current_base_idx <= idx_base_lcs){
                            FramePlayer frame;
                            frame.op_b_v1 = frame.op_b_v2 = Diff::DO_None;
                            frame.v1 = frame.v2 = frame.base = ImageRegister::ImageAt(_vbaseseq[current_base_idx]);
                            mFrames.push_back(frame);
                            current_base_idx++;
                        }
                    }



                currentLCSIdx++;

            }


            // Process the rest of frames
            int _s = diff3->diff3chunks.size();
            while (current_chunk_idx < diff3->diff3chunks.size()){

                        Diff::Diff3Chunk _chunk = diff3->diff3chunks[current_chunk_idx];

                        if (_chunk.index == current_base_idx){

                            // Check for conflicts
                            if (_chunk.basev1.size() > 0 && _chunk.basev2.size() > 0){

                                int max = _chunk.basev1.size() > _chunk.basev2.size() ? _chunk.basev1.size() : _chunk.basev2.size();

                                for (int i = 0; i < max; i++){
                                    FramePlayer frame;

                                    if (i < _chunk.basev1.size()){
                                        frame.op_b_v1 = _chunk.basev1[i].op;
                                        frame.v1 = _chunk.basev1[i].v1_Image;
                                    }

                                    if (i < _chunk.basev2.size()){
                                        frame.op_b_v2 = _chunk.basev2[i].op;
                                        frame.v2 = _chunk.basev2[i].v1_Image;
                                    }

                                    if (current_base_idx < _vbaseseq.size()){
                                        frame.base = ImageRegister::ImageAt(_vbaseseq[current_base_idx++]);
                                    }

                                    mFrames.push_back(frame);
                                }

                            } else { // Only one side has modified



                                if (_chunk.basev1.size() > 0){

                                    for (int i = 0; i < _chunk.basev1.size(); i++){
                                        FramePlayer frame;

                                        if (_chunk.basev1[i].op == Diff::DO_Change){
                                            frame.op_b_v1 = _chunk.basev1[i].op;
                                            frame.base = _chunk.basev1[i].v1_Image;
                                            frame.v1 = _chunk.basev1[i].v2_Image;
                                            frame.v2 = _chunk.basev1[i].v1_Image;
                                            frame.op_b_v2 = Diff::DO_None;
                                        } else if (_chunk.basev1[i].op == Diff::DO_Remove){
                                            frame.op_b_v1 = _chunk.basev1[i].op;
                                            frame.base = _chunk.basev1[i].v1_Image;
                                            frame.op_b_v2 = Diff::DO_None;
                                            frame.v2 = _chunk.basev1[i].v1_Image;
                                        } else if (_chunk.basev1[i].op == Diff::DO_Add){
                                            frame.op_b_v1 = _chunk.basev1[i].op;
                                            frame.op_b_v2 = Diff::DO_None;;
                                            frame.v1 = _chunk.basev1[i].v2_Image;
                                        }

                                        mFrames.push_back(frame);
                                    }


                                } else if (_chunk.basev2.size() > 0) {

                                    for (int i = 0; i < _chunk.basev2.size(); i++){
                                        FramePlayer frame;

                                        if (_chunk.basev2[i].op == Diff::DO_Change){
                                            frame.op_b_v2 = _chunk.basev2[i].op;
                                            frame.base = _chunk.basev2[i].v1_Image;
                                            frame.v2 = _chunk.basev2[i].v2_Image;
                                            frame.v1 = _chunk.basev2[i].v1_Image;
                                            frame.op_b_v1 = Diff::DO_None;
                                            mFrames.push_back(frame);
                                        } else if (_chunk.basev2[i].op == Diff::DO_Remove){
                                            frame.op_b_v2 = _chunk.basev2[i].op;
                                            frame.base = _chunk.basev2[i].v1_Image;
                                            frame.v1 = _chunk.basev2[i].v1_Image;
                                            frame.op_b_v1 = Diff::DO_None;
                                        } else if (_chunk.basev2[i].op == Diff::DO_Add){
                                            frame.op_b_v2 = _chunk.basev2[i].op;
                                            frame.op_b_v1 = Diff::DO_None;
                                            frame.v2 = _chunk.basev2[i].v2_Image;
                                        }

                                        mFrames.push_back(frame);
                                    }

                                }

                            }

                            if (current_base_idx < _vbaseseq.size()){
                                current_base_idx++;
                            }
                        }
                        current_chunk_idx++;
                    }






            qDebug() << "Frames: " << mFrames.size();
        }


        void MergePlayer::SetTime(int time){
            currentIndex = time;

            FramePlayer _current =
                    mFrames[currentIndex];

            QImage base_img((diff3->base_width + 10) * bufferSize + 20,
                          diff3->base_height + 20, QImage::Format_RGB32);

            QImage v1_img((diff3->base_width + 10) * bufferSize + 20,
                          diff3->base_height + 20, QImage::Format_RGB32);

            QImage v2_img((diff3->base_width+ 10) * bufferSize + 20,
                          diff3->base_height + 20, QImage::Format_RGB32);

            QImage merge_img((diff3->base_width + 10) * bufferSize + 20,
                            diff3->base_height + 20, QImage::Format_RGB32);

            base_img.fill(Qt::black);
            v1_img.fill(Qt::black);
            v2_img.fill(Qt::black);
            merge_img.fill(Qt::black);

            int max_frames = bufferSize > (mFrames.size() - currentIndex) ? mFrames.size() - currentIndex : bufferSize;

            QPainter painter_base(&base_img);
            QPainter painter1(&v1_img);
            QPainter painter2(&v2_img);
            QPainter painterMerge(&merge_img);


            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(5);


            QPen pen2;
            pen2.setColor(Qt::red);
            pen2.setWidth(5);
            painter2.setPen(pen2);


            for (int i = 0; i < max_frames; i++){
                QPoint dest(10 + (diff3->base_width + 10) * i, 10);

                QRect rect(dest.x()-10, dest.y()-10,
                           diff3->base_width+10,
                           diff3->base_height+10);

                FramePlayer fp = mFrames[currentIndex+i];


                //qDebug() << "V1 id: " << v1[currentIndex+i].id.c_str();
                //qDebug() << "V2 id: " << v2[currentIndex+i].id.c_str();

                if (fp.base != NULL){

                    QImage _base = fp.base->toQImage();
                    painter_base.drawImage(dest, _base);

                    if (currentIndex == 0){
                        _base.save("/Users/josericardo/Desktop/img1.jpg");
                    }
                }

                if (fp.v1 != NULL){
                    if (fp.op_b_v1 == Diff::DO_Change){
                        QImage _im1 = fp.base->toQImage();
                        QImage _imdiff = fp.v1->toQImage();
                        QImage _imres = ImageRegister::ProcessGPUDiff(&_im1, &_imdiff);
                        painter1.drawImage(dest, _imres);
                    } else {
                        QImage _v1 = fp.v1->toQImage();
                        painter1.drawImage(dest, _v1);
                    }

                }

                if (fp.v2 != NULL){
                    if (fp.op_b_v2 == Diff::DO_Change){
                        QImage _im1 = fp.base->toQImage();
                        QImage _imdiff = fp.v2->toQImage();
                        QImage _imres = ImageRegister::ProcessGPUDiff(&_im1, &_imdiff);
                        painter2.drawImage(dest, _imres);
                    } else {
                        QImage _v2 = fp.v2->toQImage();
                        painter2.drawImage(dest, _v2);
                    }
                }

                switch (fp.op_b_v1){
                case Diff::DO_Change:
                    pen.setColor(Qt::yellow);
                    painter1.setPen(pen);
                    painter1.drawRect(rect);
                    break;

                case Diff::DO_Remove:
                    pen.setColor(Qt::red);
                    painter1.setPen(pen);
                    painter1.drawRect(rect);
                    break;

                case Diff::DO_Add:
                    pen.setColor(Qt::green);
                    painter1.setPen(pen);
                    painter1.drawRect(rect);
                    break;
                }

                switch (fp.op_b_v2){
                case Diff::DO_Change:
                    pen.setColor(Qt::yellow);
                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    break;

                case Diff::DO_Remove:
                    pen.setColor(Qt::red);
                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    break;

                case Diff::DO_Add:
                    pen.setColor(Qt::green);
                    painter2.setPen(pen);
                    painter2.drawRect(rect);
                    break;
                }

                // Drawing
                if (fp.op_b_v1 == Diff::DO_None && fp.op_b_v2 == Diff::DO_None){
                    painterMerge.drawImage(dest, fp.base->toQImage());
                } else if (fp.op_b_v1 != Diff::DO_None && fp.op_b_v2 != Diff::DO_None){



                } else if (fp.op_b_v1 != Diff::DO_None){

                    if (fp.op_b_v1 == Diff::DO_Change)
                    {
                        QImage _im1 = fp.base->toQImage();
                        QImage _imdiff = fp.v1->toQImage();
                        QImage _imres = ImageRegister::ProcessGPUDiff(&_im1, &_imdiff);

                        painterMerge.drawImage(dest, _imres);
                        pen.setColor(Qt::yellow);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v1 == Diff::DO_Remove) {
                        pen.setColor(Qt::red);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v1 == Diff::DO_Add){
                        QImage _v1 = fp.v1->toQImage();
                        painterMerge.drawImage(dest, _v1);
                        pen.setColor(Qt::green);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    }
                } else if (fp.op_b_v2 != Diff::DO_None){

                    if (fp.op_b_v2 == Diff::DO_Change)
                    {
                        QImage _im1 = fp.base->toQImage();
                        QImage _imdiff = fp.v2->toQImage();
                        QImage _imres = ImageRegister::ProcessGPUDiff(&_im1, &_imdiff);

                        painterMerge.drawImage(dest, _imres);
                        pen.setColor(Qt::yellow);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v2 == Diff::DO_Remove) {
                        pen.setColor(Qt::red);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v2 == Diff::DO_Add){
                        QImage _v2 = fp.v2->toQImage();
                        painterMerge.drawImage(dest, _v2);
                        pen.setColor(Qt::green);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    }
                }
            }

            painter_base.end();
            painter1.end();
            painter2.end();
            painterMerge.end();

            video_base->clear();
            video_1->clear();
            video_2->clear();
            video_merge->clear();

            video_base->setPixmap(QPixmap::fromImage(base_img));
            video_1->setPixmap(QPixmap::fromImage(v1_img));
            video_2->setPixmap(QPixmap::fromImage(v2_img));
            video_merge->setPixmap(QPixmap::fromImage(merge_img));
        }
    }
}
