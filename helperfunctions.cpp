#include "helperfunctions.h"

namespace GEMUFF{

    void Helper::PrintList(std::vector<Hash::AbstractHash*> &list, char *name){

        qDebug() << "List: " << name  << "** Size: " << list.size();

        for (int i = 0; i < list.size(); i++){
            qDebug() << list[i]->toString().c_str();
        }
    }

    void Helper::PrintList(std::vector<LCSEntry> &list, char *name){

        qDebug() << "List: " << name  << "** Size: " << list.size();

        for (int i = 0; i < list.size(); i++){
            qDebug() << "L1: " << list[i].l1_ref->toString().c_str();
            qDebug() << "L2: " << list[i].l2_ref->toString().c_str();
        }
    }

    void Helper::PrintList(std::vector<Hash::AbstractHashPtr> &list, char *name){

        qDebug() << "List: " << name  << "** Size: " << list.size();

        for (int i = 0; i < list.size(); i++){
            qDebug() << "L1: " << list[i]->toString().c_str();
        }
    }

    std::vector<LCSEntry> Helper::LCS(std::vector<Hash::AbstractHashPtr> _s,
                                      std::vector<Hash::AbstractHashPtr> _t,
                                      float thresold){

        std::vector<LCSEntry> result;
        std::vector<Hash::AbstractHashPtr> s;
        std::vector<Hash::AbstractHashPtr> t;

        bool reversed = false;
        if (_s.size() > _t.size()){
            reversed = true;
            s = _t;
            t = _s;
        }
        else{
            s = _s;
            t = _t;
        }


       int n = s.size(), m = t.size(), i, j, **a;

       if (!n || !m) { /* empty input string */
          return result;
       }

       a = (int**)calloc(n + 1, sizeof(int*));
       a[0] = (int*)calloc((n + 1) * (m + 1), sizeof(int));

       for (i = a[0][0] = 0; i <= n; i++) { /* find the length */
          if (!i || (a[i] = a[i - 1] + m)) {
             for (j = 0; j <= m; j++) {
                if (!i || !j) { /* initialize the base row/column */
                   a[i][j] = 0;
                } else if (s[i - 1]->isSimilar(t[j - 1], thresold)) { /* diagonal step */
                   a[i][j] = a[i - 1][j - 1] + 1;
                } else { /* horizontal or vertical step */
                   a[i][j] = a[i][j - 1] > a[i - 1][j] ? a[i][j - 1] : a[i - 1][j];
                }
             }
          } else {
             abort(); /* memory failure */
          }
       }

       if (!(i = a[n][m])) {
          return result; /* no common sub-sequence */
       }

       while (n > 0 && m > 0) { /* back track to find the sequence */
           if (s[n - 1]->isSimilar(t[m - 1], thresold) && m--) {
               LCSEntry _entry;
               _entry.l1_ref = reversed ? t[m] : s[--n];
               _entry.l2_ref = reversed ? s[--n] : t[m];
               result.push_back(_entry);
          } else {
             a[n][m - 1] >= a[n - 1][m] ? m-- : n--;
          }
       }

       std::reverse(result.begin(), result.end());

       free(a[0]);
       free(a);

       return result;
    }
}
