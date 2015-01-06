/*
 * PodcastGrabber.cpp
 *
 *  Created on: May 22, 2010
 *      Author: arnaud
 */

#include "PodcastGrabber.h"
#include "debug.h"

PodcastGrabber::PodcastGrabber(string filename, string temp_folder,
                               string final_folder /*= "/media/WALKMAN/MUSIC/0podcasts/"*/) {
    this->filename = filename;
    this->temp_folder = temp_folder;
    this->final_folder = final_folder;
}

PodcastGrabber::~PodcastGrabber() {
}

void PodcastGrabber::clean_temp_folder() {
    clean_folder(temp_folder);
}

void PodcastGrabber::clean_final_folder() {
    if (dir_exists(final_folder))
        clean_folder(final_folder);
    else {
        /* create folder */
        ostringstream command;
        command << "mkdir -p " << final_folder;
        exec_system_command(command.str());
    }
}

struct thread_data {
    string line;
    string temp_folder;
    string final_folder;
};

void* process(void *threadarg) {
    thread_data* data_ptr = (thread_data*) threadarg;
    thread_data data = *data_ptr;

    /*
  * read the number of wanted files
  */
    int nb_wanted_files = 1;
    vector<string> tokens;
    istringstream iss(data.line);
    copy(istream_iterator<string> (iss), istream_iterator<string> (),
         back_inserter<vector<string> > (tokens));
    string feed_url = tokens.at(0);
    if (tokens.size() >= 2)
        nb_wanted_files = atoi(tokens.at(1).c_str());
    maggieDebug2("feed_url:'%s'", feed_url.c_str());
    maggieDebug2("nb_wanted_files:%i", nb_wanted_files);

    /*
  * find the mp3 urls
  */
    vector<string> urls;
    PodcastGrabber::find_most_recent_mp3_url(feed_url, nb_wanted_files, urls);
    //if (file_url == "")
    //continue;

    for (vector<string>::iterator it = urls.begin(); it < urls.end(); ++it) {
        string file_shortname = it->substr(1 + it->find_last_of('/'));
        if (PodcastGrabber::dir_exists(file_shortname)) {
            maggieDebug2("The file was already downloaded:'%s'",
                         file_shortname.c_str());
        } else {
            maggieDebug2("Downloading:'%s'", file_shortname.c_str());
            PodcastGrabber::download_file(*it, data.temp_folder);
        }

        PodcastGrabber::copy_if_possible(file_shortname, data.final_folder);
    } // end loop it

#if THREADS
    pthread_exit(NULL);
#endif
    return NULL;
}

void PodcastGrabber::fetch() {
    maggieDebug2("fetch()");
    vector<string> urls;
    get_file_content(filename, &urls, false);

#if THREADS
    pthread_t threads_ids[MAX_THREADS];
    int counter = 0;
#endif

    for (vector<string>::iterator it = urls.begin(); it < urls.end(); ++it) {
        // do not stop on empty lines or starting with /
        if (it->size() < 1 || it->at(0) == '/')
            continue;
        if (MAGGIE_DEBUG_LEVEL > 0)
            cout << endl;

        thread_data data;
        data.line = string(*it);
        data.final_folder = string(final_folder);
        data.temp_folder = string(temp_folder);
#if THREADS
        int rc = pthread_create(&threads_ids[++counter], NULL, process, &data);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        sleep(1);
#else
        process(&data);
#endif

    } // end loop line

#if THREADS
    pthread_exit(NULL);
#endif
}

bool PodcastGrabber::dir_exists(string dir_name) {
    struct stat st;
    if (stat(dir_name.c_str(), &st) == 0)
        return true;
    return false;
}

void PodcastGrabber::clean_folder(string path) {
    maggieDebug2("clean_folder():'%s'", path.c_str());
    ostringstream command;
    command << "cd " << path << ";";
    command << "rm *.mp3 *.mp4";
    exec_system_command(command.str());
}

void PodcastGrabber::find_most_recent_mp3_url(string feed_address,
                                              int nb_wanted_files, vector<string>& urls) {
    maggieDebug2("find_most_recent_mp3_url():'%s'", feed_address.c_str());
    // get the xml content
    string xml;
    get_url_content(feed_address, &xml, 0);//MAGGIE_DEBUG_LEVEL);
    if (xml.size() == 0) {
        maggiePrint("Nothing at the given url:'%s'", feed_address.c_str());
        return;
    }

    /*
  * find .mp3
  */
    size_t found_pos, search_begin_pos = 0;
    int search_index = 0;

    while (search_index < nb_wanted_files) {
        std::string suffix;

        suffix = ".mp4<";
        found_pos = xml.find(suffix, search_begin_pos);

        if (found_pos == string::npos) {
            suffix = ".mp4";
            found_pos = xml.find(suffix, search_begin_pos);
        } // end mp4<

        if (found_pos == string::npos) {
            suffix = ".m4v";
            found_pos = xml.find(suffix, search_begin_pos);
        } // end mp4

        if (found_pos == string::npos) {
            suffix = ".mp3<";
            found_pos = xml.find(suffix, search_begin_pos);
        } // end m4v

        if (found_pos == string::npos) {
            suffix = ".mp3";
            found_pos = xml.find(suffix, search_begin_pos);
        } // end mp3<

        if (found_pos == string::npos) {
            maggiePrint("No found suffix");
            return;
        } // end mp3

        maggieDebug2("Found suffix:%s", suffix.c_str());

        // increasing the begin index to avoid loops
        search_begin_pos = 1 + found_pos;

        // find the beginning of the url by walking backwards
        int url_begin_index = found_pos;
        for (; url_begin_index >= 0; --url_begin_index) {
            if (xml.substr(url_begin_index, 7) == "http://")
                break;
        }

        // cut to only keep the url
        int url_length = found_pos - url_begin_index;
        url_length += suffix.size();
        std::string url = xml.substr(url_begin_index, url_length);

        // remove final <
        while (url.at(url.size() - 1) == '<')
            url = url.substr(0, url.size() - 1);

        maggieDebug2("url found:'%s'", url.c_str());

        // check if it was already in the results
        bool isOK = true;
        for (vector<string>::iterator it = urls.begin(); it != urls.end(); ++it)
            if (*it == url) {
                maggieDebug2("The url was found before ! Skipping.");
                isOK = false;
                break;
            }
        if (!isOK)
            continue;

        urls.push_back(url);
        ++search_index;
    } // end loop search index
}

void PodcastGrabber::get_file_content(string filename,
                                      vector<string>* ans_lines, bool remove_leading_spaces /*= false*/) {
    ans_lines->clear();
    ifstream infile(filename.c_str());
    string line;

    while (infile.good()) {
        std::getline(infile, line);
        int beginning_index = 0;
        if (remove_leading_spaces) {
            string::size_type first_space_index = line.find_first_not_of(" ");
            if (first_space_index != string::npos)
                beginning_index = first_space_index;
        }
        ans_lines->push_back(line.substr(beginning_index));
    }

    infile.close();
}

void PodcastGrabber::get_file_content(string filename, string* ans,
                                      bool remove_leading_spaces /*= false*/) {
    vector<string> lines;
    get_file_content(filename, &lines, remove_leading_spaces);

    ostringstream buffer;
    for (vector<string>::iterator it = lines.begin(); it < lines.end(); ++it)
        buffer << *it;
    *ans = buffer.str();
}

std::string gen_random(const int len) {
    std::ostringstream ans;
    static const char alphanum[] = "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < len; ++i)
        ans << alphanum[rand() % (sizeof(alphanum) - 1)];
    return ans.str();
}

/*!
 * \brief   fetch the content of an URL and copy it in a string
 *
 * \param   url the url
 * \param   rep a pointer toward the answer
 * \param   verbose_level 0: nothing, 1 : not much information, 2:medium, 3:much
 */
void PodcastGrabber::get_url_content(string url, string* rep, int verbose_level /*=1*/) {
    /* create the file */
    ostringstream command;
    std::string TEMP_FILENAME = gen_random(8);
    maggieDebug2("TEMP_FILENAME:'%s'", TEMP_FILENAME.c_str());
    command << "touch " << TEMP_FILENAME;
    exec_system_command(command.str());

    /* connect to the site using wget and downloaf the rep */
    command.str("");
    command << "wget \"" << url << "\""
            << " --output-document=" << TEMP_FILENAME;
    //// verbose level
    command << (verbose_level == 0 ? " --quiet" : ""); // NOT MUCH
    command << (verbose_level == 1 ? " --no-verbose" : ""); // NOT MUCH
    command << (verbose_level == 2 ? " --verbose" : ""); // MEDIUM
    command << (verbose_level == 3 ? " --debug" : ""); // MUCH

    exec_system_command(command.str());

    /* read the file */
    get_file_content(TEMP_FILENAME, rep, true);

    /* remove the file */
    command.str("");
    command << "rm " << TEMP_FILENAME;
    exec_system_command(command.str());

    //maggieDebug2("rep:'%s'", rep->c_str());
}

void PodcastGrabber::download_file(string url, string folder) {
    ostringstream command;
    // construct the wget order
    command.str("");

    // axel
    // command << "axel " << url;
    // command << " --quiet --output=" << folder;

    // wget
    command << "wget " << url;
    command << " --directory-prefix=" << folder;
    // add the folder where we want to put it
    // download the file
    exec_system_command(command.str());
}

void PodcastGrabber::exec_system_command(string command) {
    maggieDebug2("exec_system_command():'%s'", command.c_str());
    system(command.c_str());
}

void PodcastGrabber::copy_if_possible(string filename, string final_folder /*= "/media/WALKMAN"*/) {
    maggieDebug2("copy_if_possible():'%s'", filename.c_str());
    if (!dir_exists(final_folder))
        return;

    // get extension
    string extension = filename.substr(1 + filename.find_last_of('.'));

    //
    ostringstream command;
    if (extension == "mp3") {
        /*
   * direct copy
   */
        command << "cp " << filename << " " << final_folder;
        exec_system_command(command.str());
    } else {
        /*
   * convert the video
   */
        string filename_cvt = filename.substr(0, -1
                                              + filename.find_last_of('.')) + "_cvt.mp4";
        if (!dir_exists(filename_cvt)) {
            // The default setting of "-vol" for FFmpeg is 256
            command << "ffmpeg -y -i " << filename
                    << " -b 576k -s 320x240 -vcodec mpeg4 -ab 220k -vol 512 -ar 44100 -ac 2 -acodec libfaac "
                    << filename_cvt;
            // ffmpeg -y -i filename -b 576k -s 320x240 -vcodec mpeg4 -ab 220k -ar 44100 -ac 2 -acodec libfaac test.mp4
            exec_system_command(command.str());
        } else {
            maggieDebug2("file already re-encoded:'%s'", filename_cvt.c_str());
        }
        /*
   * copy it
   */
        command.str("");
        command << "cp " << filename_cvt << " " << final_folder;
        exec_system_command(command.str());
    }
}
