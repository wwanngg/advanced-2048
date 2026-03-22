#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QMap>
#include <QString>

namespace Constants {
    const inline QMap<int, QString> styles { 
        { 2, "background-color: white; color: black; font: bold 48px; border-radius: 10px;"}, 
        { 4, "background-color: yellow; color: black; font: bold 48px; border-radius: 10px;" },
        { 8, "background-color: chocolate; color: white; font: bold 48px; border-radius: 10px;" },
        { 16, "background-color: peru; color: white; font: bold 48px; border-radius: 10px;" },
        { 32, "background-color: tomato; color: white; font: bold 48px; border-radius: 10px;" },
        { 64, "background-color: red; color: white; font: bold 48px; border-radius: 10px;" },
        { 128, "background-color: wheat; color: white; font: bold 48px; border-radius: 10px;" },
        { 256, "background-color: gold; color: white; font: bold 48px; border-radius: 10px;" },
        { 512, "background-color: goldenrod; color: white; font: bold 48px; border-radius: 10px;" },
        { 1024, "background-color: darkgoldenrod; color: white; font: bold 48px; border-radius: 10px;" },
        { 2048, "background-color: black; color: white; font: bold 48px; border-radius: 10px;" } };
    inline QMap<int, QString> caixukunStyles {
        { 2, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/1.jpg); padding-bottom: 5px; padding-right: 5px;"}, 
        { 4, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/2.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 8, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/3.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 16, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/4.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 32, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/5.png); padding-bottom: 5px; padding-right: 5px;" },
        { 64, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/6.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 128, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/7.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 256, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/8.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 512, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/9.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 1024, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/10.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2048, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/11.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2049, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/12.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2050, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/13.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2051, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/14.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2052, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/15.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2053, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/16.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2054, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/17.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2055, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/18.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2056, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/19.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2057, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/20.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2058, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/21.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2059, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/22.png); padding-bottom: 5px; padding-right: 5px;" },
        { 2060, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/23.jpg); padding-bottom: 5px; padding-right: 5px;" },
        { 2061, "color: white; font: bold 10px; border-radius: 10px; border-image: url(../resources/24.jpg); padding-bottom: 5px; padding-right: 5px;" } };
    const inline int windowSize{ 600 };
    const inline int labelSize{ 150 };
    const inline int duration{ 800 };
    const inline int maxUsedNum{ 2048 };
}

#endif