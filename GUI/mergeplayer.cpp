
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

        int MergePlayer::getNumFrames(Diff::SeqMark &seqMark, std::vector<Diff::DiffChunk>& diffChunk){
            int nFrames = 0;
            for (int i = seqMark.offset - seqMark.nFrames; i < seqMark.offset; i++){
                nFrames += diffChunk[i].diffData.size();
            }

            nFrames += diffChunk[seqMark.offset].diffData.size();

            return nFrames;
        }

        std::vector<FramePlayer> MergePlayer::generateSubSequence(Diff::SeqMark seqMark, std::vector<Diff::DiffChunk>& diffChunk, int side){

            std::vector<FramePlayer> result;

            for (int i = seqMark.offset - seqMark.nFrames; i < seqMark.offset; i++){

                for (int j = 0; j < diffChunk[i].diffData.size(); j++){

                    Diff::DiffData _df = diffChunk[i].diffData[j];

                    FramePlayer _fp;

                    if (side == 0){
                        _fp.op_b_v1 = _df.op;

                        if (_df.op == Diff::DO_Add){
                            _fp.v1 = _df.v2_Image;
                        } else if (_df.op == Diff::DO_Remove){
                            _fp.base = _fp.v2 = ImageRegister::ImageAt(_df.v1_HashPtr);
                        } else if (_df.op == Diff::DO_Change){
                            _fp.base = _fp.v2 = ImageRegister::ImageAt(_df.v1_HashPtr);
                            _fp.v1 = ImageRegister::ProcessDIFF(ImageRegister::ImageAt(_df.v1_HashPtr),
                                                                _df.v2_Image, Device::D_GPU);
                        }
                    }
                    else {
                        _fp.op_b_v2 = _df.op;

                        if (_df.op == Diff::DO_Add){
                            _fp.v2 = _df.v2_Image;
                        } else if (_df.op == Diff::DO_Remove){
                            _fp.base = _fp.v1 = ImageRegister::ImageAt(_df.v1_HashPtr);
                        } else if (_df.op == Diff::DO_Change){
                            _fp.base = _fp.v1 = ImageRegister::ImageAt(_df.v1_HashPtr);
                            _fp.v2 = ImageRegister::ProcessDIFF(ImageRegister::ImageAt(_df.v1_HashPtr),
                                                                _df.v2_Image, Device::D_GPU);
                        }
                    }

                    result.push_back(_fp);
                }

                if (diffChunk[i].lcsEntry.isValid()){
                    FramePlayer _fp;
                    _fp.base = _fp.v1 = _fp.v2 = ImageRegister::ImageAt(diffChunk[i].lcsEntry.l1_ref);
                }
            }

            // Last chunk without adding the LCS
            std::vector<Diff::DiffData> diffData = diffChunk[seqMark.offset].diffData;
            for (int j = 0; j < diffData.size(); j++){

                Diff::DiffData _df = diffData[j];

                FramePlayer _fp;

                if (side == 0){
                    _fp.op_b_v1 = _df.op;

                    if (_df.op == Diff::DO_Add){
                        _fp.v1 = _df.v2_Image;
                    } else if (_df.op == Diff::DO_Remove){
                        _fp.base = _fp.v2 = ImageRegister::ImageAt(_df.v1_HashPtr);
                    } else if (_df.op == Diff::DO_Change){
                        _fp.base = _fp.v2 = ImageRegister::ImageAt(_df.v1_HashPtr);
                        _fp.v1 = _df.v2_Image;
                    }
                }
                else {
                    _fp.op_b_v2 = _df.op;

                    if (_df.op == Diff::DO_Add){
                        _fp.v2 = _df.v2_Image;
                    } else if (_df.op == Diff::DO_Remove){
                        _fp.base = _fp.v1 = ImageRegister::ImageAt(_df.v1_HashPtr);
                    } else if (_df.op == Diff::DO_Change){
                        _fp.base = _fp.v1 = ImageRegister::ImageAt(_df.v1_HashPtr);
                        _fp.v2 = _df.v2_Image;
                    }
                }

                result.push_back(_fp);
            }


            return result;
        }



        void MergePlayer::GeneratePlayerData(){

            int currentLCSIdx = 0;

            while (currentLCSIdx < diff3->diff3Chunks.size()){

                Diff::Diff3Chunk _currentChunk = diff3->diff3Chunks[currentLCSIdx++];                


               int nFramesv1 = getNumFrames(_currentChunk.v1, diff3->diffBaseToV1);
               int nFramesV2 = getNumFrames(_currentChunk.v2, diff3->diffBaseToV2);

                // Conflict
                if (nFramesv1 > 0 && nFramesV2 > 0){

                    // Add Base Frame
                    int currentFramesIdx = mFrames.size();

                    for (int k = 0; k < _currentChunk.base.size(); k++){
                        FramePlayer _fp;
                        _fp.base = ImageRegister::ImageAt(_currentChunk.base[k]);
                        mFrames.push_back(_fp);
                    }

                    int currentFramesLastSize = mFrames.size();

                    std::vector<FramePlayer> _fp1 = generateSubSequence(_currentChunk.v1, diff3->diffBaseToV1, 0);
                    std::vector<FramePlayer> _fp2 = generateSubSequence(_currentChunk.v2, diff3->diffBaseToV2, 1);

                    for (int i = 0; i < _fp1.size(); i++){
                        if (currentFramesIdx + i < currentFramesLastSize){
                            FramePlayer *_fp = &mFrames[currentFramesIdx + i];

                            _fp->op_b_v1 = _fp1[i].op_b_v1;
                            _fp->v1 = _fp1[i].v1;
                        } else {
                            FramePlayer _fp;

                            _fp.op_b_v1 = _fp1[i].op_b_v1;
                            _fp.v1 = _fp1[i].v1;
                            mFrames.push_back(_fp);
                        }
                    }
                    currentFramesLastSize = mFrames.size();

                    for (int i = 0; i < _fp2.size(); i++){
                        if (currentFramesIdx + i < currentFramesLastSize){
                            FramePlayer *_fp = &mFrames[currentFramesIdx + i];

                            _fp->op_b_v2 = _fp2[i].op_b_v2;
                            _fp->v2 = _fp2[i].v2;
                        } else {
                            FramePlayer _fp;

                            _fp.op_b_v2 = _fp2[i].op_b_v2;
                            _fp.v2 = _fp2[i].v2;
                            mFrames.push_back(_fp);
                        }
                    }

                } else if (nFramesv1 > 0) {

                    std::vector<FramePlayer> _fp = generateSubSequence(_currentChunk.v1, diff3->diffBaseToV1, 0);

                    for (int i = 0; i < _fp.size(); i++) mFrames.push_back(_fp[i]);
                } else if (nFramesV2 > 0){

                    std::vector<FramePlayer> _fp = generateSubSequence(_currentChunk.v2, diff3->diffBaseToV2, 1);

                    for (int i = 0; i < _fp.size(); i++) mFrames.push_back(_fp[i]);
                }


                if (_currentChunk.lcsEntry.isValid()){
                    FramePlayer _fp;
                    _fp.base = _fp.v1 = _fp.v2 = ImageRegister::ImageAt(_currentChunk.lcsEntry.l2_ref);
                    mFrames.push_back(_fp);
                }
            }
            fprintf(stderr, "Frames: %d\n", mFrames.size());
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

                    QImage _base = toQImage(fp.base);
                    painter_base.drawImage(dest, _base);

                    if (currentIndex == 0){
                        _base.save("/Users/josericardo/Desktop/img1.jpg");
                    }
                }

                if (fp.v1 != NULL){
                    if (fp.op_b_v1 == Diff::DO_Change){
                        QImage _im1 = toQImage(fp.base);
                        QImage _imdiff = toQImage(fp.v1);
                        QImage _imres = toQImage(ImageRegister::ProcessDIFF(fp.base, fp.v1, Device::D_GPU));
                        painter1.drawImage(dest, _imres);
                    } else {
                        QImage _v1 = toQImage(fp.v1);
                        painter1.drawImage(dest, _v1);
                    }

                }

                if (fp.v2 != NULL){
                    if (fp.op_b_v2 == Diff::DO_Change){
                        QImage _im1 = toQImage(fp.base);
                        QImage _imdiff = toQImage(fp.v2);
                        QImage _imres = toQImage(ImageRegister::ProcessDIFF(fp.base, fp.v2, Device::D_GPU));
                        painter2.drawImage(dest, _imres);
                    } else {
                        QImage _v2 = toQImage(fp.v2);
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
                    painterMerge.drawImage(dest, toQImage(fp.base));
                } else if (fp.op_b_v1 != Diff::DO_None && fp.op_b_v2 != Diff::DO_None){

                    pen.setColor(Qt::red);
                    painterMerge.fillRect(rect, Qt::red);

                } else if (fp.op_b_v1 != Diff::DO_None){

                    if (fp.op_b_v1 == Diff::DO_Change)
                    {
                        painterMerge.drawImage(dest, toQImage(fp.v1));
                        pen.setColor(Qt::yellow);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v1 == Diff::DO_Remove) {
                        pen.setColor(Qt::red);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v1 == Diff::DO_Add){
                        QImage _v1 = toQImage(fp.v1);
                        painterMerge.drawImage(dest, _v1);
                        pen.setColor(Qt::green);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    }
                } else if (fp.op_b_v2 != Diff::DO_None){

                    if (fp.op_b_v2 == Diff::DO_Change)
                    {
                        painterMerge.drawImage(dest, toQImage(fp.v2));
                        pen.setColor(Qt::yellow);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v2 == Diff::DO_Remove) {
                        pen.setColor(Qt::red);
                        painterMerge.setPen(pen);
                        painterMerge.drawRect(rect);
                    } else if (fp.op_b_v2 == Diff::DO_Add){
                        QImage _v2 = toQImage(fp.v2);
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
