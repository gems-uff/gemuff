#include "mainwindow.h"
#include <QApplication>
#include "videoprocessing.h"
#include "diffalgorithms.h"

void DiffAlgoTest()
{
   /* std::string _a = "A";
    std::string _b = "B";
    std::string _c = "C";
    std::string _d = "D";
    std::string _e = "E";
    std::string _f = "F";
    std::string _g = "G";


    // list 1
    std::vector<std::string> list1;
    {
        list1.push_back(_a);
        list1.push_back(_b);
        list1.push_back(_c);
    }
    printf("\nList 1: ");
    for (int i = 0; i < list1.size(); i++)
        printf("%s", list1[i].c_str());

    // list 2
    std::vector<std::string> list2;
    {
        list2.push_back(_d);
        list2.push_back(_e);
        list2.push_back(_f);
    }
    printf("\nList 2: ");
    for (int i = 0; i < list2.size(); i++)
        printf("%s", list2[i].c_str());

    std::vector<GEMUFF::Diff::Node> _diff =
            GEMUFF::Diff::HierarchicalDiff::CalculateDiff(list1, list2);

    std::string sq1 = "\n--Seq1: ";
    std::string sq2 = "\n--Seq2: ";

    for (int i = 0; i < _diff.size(); i++)
    {
        if (_diff[i].id != ID_NODE_UNKNOW)
        {
            sq1 += _diff[i].id.c_str();
            sq2 += _diff[i].id.c_str();
        }

        for (int k = 0; k < _diff[i].seq1.size();k++)
        {
            sq1 += " |" + _diff[i].seq1[k] + "| ";
        }
        for (int k = 0; k < _diff[i].seq2.size();k++)
        {
            sq2 += " |" + _diff[i].seq2[k] + "| ";
        }
    }

    printf("%s", sq1.c_str());
    printf("%s", sq2.c_str());*/
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}


