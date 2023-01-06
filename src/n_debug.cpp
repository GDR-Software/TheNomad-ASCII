#ifndef _NOMAD_DEBUG
#else

#include "n_debug.h"
//#include <sys/mman.h>

//static zip_t* dbg_dir;

void __attribute__((constructor)) DBG_CreateFolder(void)
{
//    system("mkdir Files/debug");
}
/*
void __attribute__((destructor)) DBG_ZipFolder(void)
{
    zip_error_t err;
    dbg_dir = zip_open("Files/debug.zip", ZIP_CREATE, &err);
    if (!dbg_dir)
        N_Error("libzip error (could not create debug zip file): %s", zip_error_strerror(&err));
    
    FILE* logfile = fopen(LOG_PATH, "r");
    struct stat fdata;
    stat(LOG_PATH, &fdata);
    char* logmap = (char*)mmap(NULL, fdata.st_size, PROT_READ, MAP_PRIVATE, fileno(logfile), 0);
    zip_source_t* logsource = zip_source_buffer(dbg_dir, &logmap, 0);
    if (!logsource)
        N_Error("libzip error (could not create a source buffer): %s", zip_error_strerror(&err));
    nomadint_t logindex = zip_file_add(dbg_dir, "debuglog.txt", ZIP_FL_ENC_UTF_8);
    if (logindex < 0)
        N_Error("libzip error (could not add file to the archive): %s", zip_error_strerror(&err));
    

    if (zip_close(dbg_dir) < 0)
        N_Error("libzip error (could not close arhive): %s", zip_error_strerror(&err));
}
*/
#endif
