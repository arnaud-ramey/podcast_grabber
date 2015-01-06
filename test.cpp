/*
 * test.cpp
 *
 *  Created on: May 22, 2010
 *      Author: arnaud
 */

#include "PodcastGrabber.h"

int main() {
    cout << "main" << endl;
    PodcastGrabber pcd("urls.txt", ".");
    pcd.clean_final_folder();
    pcd.fetch();
}
