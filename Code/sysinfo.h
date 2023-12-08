/*
 *  sysinfo.h - SysInfo application class
 *
 *  Abstract:
 *      Defines a simple task that shows battery status,
 *      active network info, or other useful debugging or
 *      status info about the console.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */
 
#ifndef _GM_SYSINFO_H_
#define _GM_SYSINFO_H_

class SysInfo : public GMTask {
  public:
    SysInfo();
    void setup(bool rsvp) override;
    uint8_t getBatteryAvail();

    static const String appName;

  private:
    void run() override;
    void showHeader();
    void showSystemInfo();
    int showHWInfo();
    int showPlayerInfo();
    int showNetInfo();
};

#endif