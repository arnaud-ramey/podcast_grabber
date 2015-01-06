/*
 * PodcastGrabber.h
 *
 *  Created on: May 22, 2010
 *      Author: arnaud
 */

#ifndef PODCASTGRABBER_H_
#define PODCASTGRABBER_H_

//#define TEMP_FILENAME "temp_filename.txt"
#define THREADS        0
#define MAX_THREADS    5

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;

// c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <pthread.h>

using namespace std;

class PodcastGrabber {
public:
    PodcastGrabber(string filename, string temp_folder, string final_folder =
                   "/media/WALKMAN/MUSIC/0podcasts/");
    virtual ~PodcastGrabber();
    void clean_temp_folder();
    void clean_final_folder();
    void fetch();

    //private:
    string filename;
    string temp_folder;
    string final_folder;
    vector<string> addresses;

    static void clean_folder(string path);
    static void find_most_recent_mp3_url(string feed_address,
                                         int nb_wanted_files, vector<string>& urls);
    static void copy_if_possible(string filename, string final_folder);

    static void get_file_content(string filename, vector<string>* ans_lines,
                                 bool remove_leading_spaces = false);
    static void get_file_content(string filename, string* ans,
                                 bool remove_leading_spaces = false);
    static void get_url_content(string url, string* rep, int verbose_level = 1);
    static void download_file(string url, string folder);
    static void exec_system_command(string order);
    static bool dir_exists(string dir_name);
};

#endif /* PODCASTGRABBER_H_ */
