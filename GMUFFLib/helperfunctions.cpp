#include "helperfunctions.h"
#include "lcs.h"

namespace GEMUFF{

/*
int xdlt_load_mmfile(char const *fname, mmfile_t *mf, int binmode) {
                char cc;
                int fd;
                long size, bsize;
                char *blk;

                if (xdl_init_mmfile(mf, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0)
                        return -1;
                if ((fd = open(fname, O_RDONLY)) == -1) {
                        perror(fname);
                        xdl_free_mmfile(mf);
                        return -1;
                }
                if ((size = bsize = lseek(fd, 0, SEEK_END)) > 0 && !binmode) {
                        if (lseek(fd, -1, SEEK_END) != (off_t) -1 &&
                            read(fd, &cc, 1) && cc != '\n')
                                bsize++;
                }
                lseek(fd, 0, SEEK_SET);
                if (!(blk = (char *) xdl_mmfile_writeallocate(mf, bsize))) {
                        xdl_free_mmfile(mf);
                        close(fd);
                        return -1;
                }
                if (read(fd, blk, (size_t) size) != (size_t) size) {
                        perror(fname);
                        xdl_free_mmfile(mf);
                        close(fd);
                        return -1;
                }
                close(fd);
                if (bsize > size)
                        blk[size] = '\n';
                return 0;
        }

    void SaveFile(std::vector<Hash::AbstractHashPtr>& _s, char* filename){
        FILE* f = fopen(filename, "w");

        for (int i = 0; i < _s.size(); i++){
            const char* str = _s[i]->toString().c_str();
            char* sep = "\n";
            fwrite(str, _s[i]->toString().size(), 1, f);
            fwrite("\n", 1, 1, f);
        }

        fclose(f);
    }

    static int xdlt_outf(void *priv, mmbuffer_t *mb, int nbuf) {
        int i;

        for (i = 0; i < nbuf; i++)
                if (!fwrite(mb[i].ptr, mb[i].size, 1, (FILE *) priv))
                    return -1;

        return 0;
    }

    static void *wrap_malloc(void *priv, unsigned int size) {

        return malloc(size);
    }


    static void wrap_free(void *priv, void *ptr) {

        free(ptr);
    }


    static void *wrap_realloc(void *priv, void *ptr, unsigned int size) {

        return realloc(ptr, size);
    }
    */


    /*void DiffXLib(char* file1, char* file2, char* result){
        int ctxlen = 4;

        memallocator_t malt;
        malt.priv = NULL;
        malt.malloc = wrap_malloc;
        malt.free = wrap_free;
        malt.realloc = wrap_realloc;
        xdl_set_allocator(&malt);

        xdemitcb_t ecb;
        mmfile_t mf1, mf2;
        xpparam_t xpp;
        xdemitconf_t xecfg;

        FILE* f = fopen(result, "w");

        ecb.priv = f;
        ecb.outf = xdlt_outf;

        xpp.flags = 0;

        xecfg.ctxlen = ctxlen;



        xdlt_load_mmfile(file1, &mf1, 0);
        xdlt_load_mmfile(file2, &mf2, 0);

        if (xdl_diff(&mf1, &mf2, &xpp, &xecfg, &ecb) < 0) {
            xdl_free_mmfile(&mf2);
            xdl_free_mmfile(&mf1);
            fprintf(stderr, "LibXDiff::Error!");
         }


         fclose(f);

         xdl_free_mmfile(&mf2);
         xdl_free_mmfile(&mf1);
    }*/

    std::vector<std::string> toString(std::vector<Hash::AbstractHashPtr>& _s){

        std::vector<std::string> lst;

        for (int i = 0; i < _s.size(); i++){
            lst.push_back(_s[i]->toString());
        }

        return lst;
    }


    void Helper::PrintList(std::vector<Hash::AbstractHash*> &list, char *name){

        fprintf(stdout, "List: %s ** Size: %d\n", name, list.size());

        for (int i = 0; i < list.size(); i++){
            fprintf(stdout, "%s\n", list[i]->toString().c_str());
        }
    }

    void Helper::PrintList(std::vector<LCSEntry> &list, char *name){

    	fprintf(stdout, "List: %s ** Size: %d\n", name, list.size());

        for (int i = 0; i < list.size(); i++){
        	fprintf(stdout, "L1: %s\n", list[i].l1_ref->toString().c_str());
        	fprintf(stdout, "L2: %s\n", list[i].l2_ref->toString().c_str());
        }
    }

    void Helper::PrintList(std::vector<Hash::AbstractHashPtr> &list, char *name){

    	fprintf(stdout, "List: %s ** Size: %d\n", name, list.size());

        for (int i = 0; i < list.size(); i++){
        	fprintf(stdout, "L1: %s\n", list[i]->toString().c_str());
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



    std::vector<LCSEntry> Helper::LCS(std::vector<Hash::AbstractHashPtr> _s,
                                      std::vector<Hash::AbstractHashPtr> _t){


        std::vector<std::string> _sV = toString(_s);
        std::vector<std::string> _tV = toString(_t);


        std::vector<std::string> LCS(_tV.size());
        std::vector<std::string>::iterator end = lcs(_sV.begin(), _sV.end(),
                           _tV.begin(), _tV.end(),
                           LCS.begin());

        LCS.resize(end-LCS.begin());


        std::vector<LCSEntry> result;

        std::map<std::string,Hash::AbstractHashPtr> _h1, _h2;

        for (int i = 0; i < _s.size(); i++){
            _h1.insert(std::make_pair(_s[i]->toString(), _s[i]));
        }

        for (int i = 0; i < _t.size(); i++){
              _h2.insert(std::make_pair(_t[i]->toString(), _t[i]));
        }


        for (int i = 0; i < LCS.size(); i++){
            LCSEntry _entry;

            Hash::AbstractHashPtr abs1 = _h1[LCS[i]];
            Hash::AbstractHashPtr abs2 = _h2[LCS[i]];

            assert(abs1 != NULL && "Null key");
            assert(abs2 != NULL && "Null key");

            _entry.l1_ref = abs1;
            _entry.l2_ref = abs2;
            result.push_back(_entry);
        }

        return result;
    }

}
