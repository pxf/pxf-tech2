#ifndef _SSTAT_H_
#define _SSTAT_H_

long int sstat_memoryusage();
//int64_t sstat_cpuusage();
int sstat_savefiledialog(char* _filename); // return 0 if we can save to _filename
int sstat_openfiledialog(char* _filename); // return 0 if we can open _filename
void sstat_getcwd(char* _filepath, size_t _maxsize);

#endif /* _SSTAT_H_ */