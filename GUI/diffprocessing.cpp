#include "diffprocessing.h"

namespace GEMUFF {
    namespace VIMUFF {

        Diff2Processing::Diff2Processing()
        {
        }

        void Diff2Processing::CalculateDiff(Video *v1, Video *v2)
        {
            mV1 = v1;
            mV2 = v2;
            mFrameWidth = v1->getFrameWidth();
            mFrameHeight = v1->getFrameHeight();
            mFPS = v1->getFPS();

            std::vector<Hash::AbstractHashPtr>& v1_sequence_hash = v1->getSequenceHash();
            std::vector<Hash::AbstractHashPtr>& v2_sequence_hash = v2->getSequenceHash();


             // Processar video final
             int _frameSize = sizeof(uint8_t) * 4 * mFrameWidth * mFrameHeight;

             #ifdef _DEBUG_TIME
             QTime time;
             time.restart();
             #endif

             std::vector<Diff::Node> nodes =
                     Diff::HierarchicalDiff::CalculateDiff(v1_sequence_hash, v2_sequence_hash, 1.0f);

             #ifdef _DEBUG_TIME
             qDebug() << "MD5 LCS: (ms): " << time.elapsed();
             #endif _DEBUG_TIME

             // Criar as estruturas de diff baseado nos intervalos calculados
            GenerateDiffData(nodes);
        }


        void Diff2Processing::GenerateDiffData(std::vector<Diff::Node> &interval){

            #ifdef _DEBUG_TIME
            QTime time;
            int totalSimilarHashTime = 0;
            #endif

            for (int i = 0; i < interval.size(); i++)
            {
                NodeDiff2 node_diff2;
                node_diff2.id_LCS = interval[i].id1;

                // Check if both intervals have modifications
                if (interval[i].seq1.size() > 0 && interval[i].seq2.size() > 0){

                    #ifdef _DEBUG_TIME
                    time.restart();
                    #endif

                    // New LCS considering this subinterval and using a similar hash algorithm
                    std::vector<VIMUFF::ImagePtr> v1_images;
                    for (int k = 0; k < interval[i].seq1.size(); k++){
                        VIMUFF::ImagePtr img = ImageRegister::ImageAt(interval[i].seq1[k]);
                        v1_images.push_back(img);
                    }                 

                    std::vector<Hash::AbstractHashPtr> v1_sequence_hash =
                            Hash::MarrHildretchHash::GenerateHashBatch(v1_images);
                    for (int k = 0; k < interval[i].seq1.size(); k++){
                        v1_sequence_hash[k]->setData(interval[i].seq1[k]);

                         static bool save = true;
                         if (save){
                             ImagePtr im1 = ImageRegister::ImageAt(interval[i].seq1[k]);
                             //ImagePtr im2 = ImageRegister::ImageAt(similarHashRes[k].id2->getData());
                             save = false;
                             QImage _img1 = im1->toQImage();
                             //QImage _img2 = im2->toQImage();
                             _img1.save("/Users/josericardo/imseq1.jpg");
                             //_img2.save("/Users/josericardo/imseq2.jpg");

                         }

                    }


                    std::vector<VIMUFF::ImagePtr> v2_images;
                    for (int k = 0; k < interval[i].seq2.size(); k++){
                        VIMUFF::ImagePtr img = ImageRegister::ImageAt(interval[i].seq2[k]);

                        static bool save = true;
                        if (save){
                            save = false;
                            QImage _img = img->toQImage();
                            _img.save("/Users/josericardo/seq2.jpg");

                        }
                        v2_images.push_back(img);
                    }

                    std::vector<Hash::AbstractHashPtr> v2_sequence_hash =
                            Hash::MarrHildretchHash::GenerateHashBatch(v2_images);
                    for (int k = 0; k < interval[i].seq2.size(); k++){
                        v2_sequence_hash[k]->setData(interval[i].seq2[k]);
                    }

                    std::vector<Diff::Node> similarHashRes = Diff::HierarchicalDiff::CalculateDiff(
                                v1_sequence_hash, v2_sequence_hash, 1.0f);

                    #ifdef _DEBUG_TIME
                    totalSimilarHashTime += time.restart();
                    #endif

                    for (int k = 0; k < similarHashRes.size(); k++)
                    {
                        if (similarHashRes[k].id1 != NULL &&
                                similarHashRes[k].id2 != NULL) {

                           /* static bool save = true;
                            if (save){
                                ImagePtr im1 = ImageRegister::ImageAt(similarHashRes[k].id1->getData());
                                ImagePtr im2 = ImageRegister::ImageAt(similarHashRes[k].id2->getData());
                                save = false;
                                QImage _img1 = im1->toQImage();
                                QImage _img2 = im2->toQImage();
                                _img1.save("/Users/josericardo/imseq1.jpg");
                                _img2.save("/Users/josericardo/imseq2.jpg");

                            }*/

                            FrameDiff2 fd2;
                            fd2.v1.op = fd2.v2.op = OP_CHANGED;

                            fd2.v1.frame_id = similarHashRes[k].id1->getData();
                            fd2.v2.frame_id = similarHashRes[k].id2->getData();
                            node_diff2.frame_diff.push_back(fd2);
                        }

                        // Elements removed from v1
                        for (int z = 0; z < similarHashRes[k].seq1.size(); z++){
                            FrameDiff2 fd2;

                            fd2.v1.op = OP_REMOVED; fd2.v2.op = OP_NONE;
                            fd2.v1.frame_id = similarHashRes[k].seq1[z]->getData();
                            //fd2.v2.frame_id = NULL;
                            node_diff2.frame_diff.push_back(fd2);
                        }

                        // Elements added to v2
                        for (int z = 0; z < similarHashRes[k].seq2.size(); z++){
                            FrameDiff2 fd2;

                            fd2.v1.op = OP_NONE; fd2.v2.op = OP_ADD;
                            //fd2.v1.frame_id = NULL;
                            fd2.v2.frame_id = similarHashRes[k].seq2[z]->getData();
                            node_diff2.frame_diff.push_back(fd2);
                        }
                    }

                    v1_sequence_hash.clear();
                    v2_sequence_hash.clear();
                } else { // Only one side has frames added or removed

                    // Frames removed from v1
                    for (int k = 0; k < interval[i].seq1.size(); k++){
                        FrameDiff2 fd2;
                        fd2.v1.op = OP_REMOVED; fd2.v2.op = OP_NONE;
                        fd2.v1.frame_id = interval[i].seq1[k];
                        //fd2.v2.frame_id = NULL;
                        node_diff2.frame_diff.push_back(fd2);
                    }

                    // Frames added to v2
                    for (int k = 0; k < interval[i].seq2.size(); k++){
                        FrameDiff2 fd2;
                        fd2.v1.op = OP_NONE; fd2.v2.op = OP_ADD;
                        //fd2.v1.frame_id = NULL;
                        fd2.v2.frame_id = interval[i].seq2[k];
                        node_diff2.frame_diff.push_back(fd2);
                    }
                }

                mDiffNodes.push_back(node_diff2);
            }

            #ifdef _DEBUG_TIME
            qDebug() << "Similarity Hash: (ms): " << totalSimilarHashTime;
            #endif
        }

        bool Diff2Processing::SaveDiff(std::string filename){

            boost::filesystem::path dir("/Users/josericardo/tmp/diffs");

            if (boost::filesystem::create_directories(dir)){

                int size = mDiffNodes.size();
                std::ofstream _diffFile;
                _diffFile.open("/Users/josericardo/tmp/diff2.bin", std::ios::binary | std::ios::out | std::ios::app);
                _diffFile.write((char*)&size, sizeof(int));


                for (int i = 0; i < mDiffNodes.size(); i++){


                    NodeDiff2 _diffNode = mDiffNodes[i];

                    _diffFile.write((char*) &_diffNode, sizeof(_diffNode));

                    for (int k = 0; k < _diffNode.frame_diff.size(); k++){
                        FrameDiff2 _fd2 = _diffNode.frame_diff[k];

                        std::stringstream ss;

                        if (_fd2.v1.op == OP_REMOVED){
                            ss << "/Users/josericardo/tmp/diffs/" << "I" << i << k << ".png";
                            ImageRegister::ImageAt(_fd2.v1.frame_id)->toQImage().save(ss.str().c_str());
                        } else if (_fd2.v1.op == OP_CHANGED){
                            _diffFile << "F " <<  _fd2.v2.frame_id->toString() << std::endl << " OC" << std::endl;

                            QImage img1 = ImageRegister::ImageAt(_fd2.v1.frame_id)->toQImage();
                            QImage img2 = ImageRegister::ImageAt(_fd2.v2.frame_id)->toQImage();
                            QImage diff = ImageRegister::ProcessGPUDiff(&img1, &img2);

                            ss << "/Users/josericardo/tmp/diffs/" << "I" << i << k << ".png";

                            diff.save(ss.str().c_str());
                        }
                    }

                }

                _diffFile.close();

                return true;
            }


            return false;
        }

        int Diff2Processing::Patch(Video *v2, std::string filename){

            // Register all diff images
            boost::filesystem::path dir("/Users/josericardo/tmp/diffs");
            boost::filesystem::recursive_directory_iterator end_iter;

            if (boost::filesystem::exists(dir) && boost::filesystem::is_directory(dir)){
                for (boost::filesystem::recursive_directory_iterator dir_iter(dir);
                     dir_iter != end_iter; ++dir_iter ){

                    if (boost::filesystem::is_regular_file(dir_iter->status()) &&
                            (dir_iter->path().extension() == ".png"))
                    {
                        QImage img(dir_iter->path().c_str());
                        ImageRegister::RegisterFrame((unsigned char*)img.constBits(), img.width(), img.height(), 3);
                    }
                }
            }

            // Read file
            int size;
            std::vector<NodeDiff2> nodes;
            NodeDiff2 _nodeDiff;
            std::ifstream _diffFile("/Users/josericardo/tmp/diff2.bin", std::ios::binary | std::ios::app);
            _diffFile.read((char*)&size, sizeof(int));

            printf("size: %d\n", size);

            for (int i = 0; i < size; i++){
                _diffFile.read((char*)&_nodeDiff, sizeof(NodeDiff2));
            }





            _diffFile.close();


        }
    }
}
