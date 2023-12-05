/*
 *  about.h - An "about box"
 *
 *  Abstract:
 *      Defines a simple "game" task that hooks into the
 *      menu and task scheduler to show credits/version
 *      info about the console.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_ABOUT_H_
#define _GM_ABOUT_H_

#define GM_VERSION 0.4

class AboutBox : public GMTask {
  public:
    AboutBox();
    void setup();

  private:
    void showAboutBox();
    void run() override;
};

#endif
