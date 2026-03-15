#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QMap>
#include <QString>

namespace Constants {
    const inline QMap<int, QString> styles{ 
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
    const inline int windowSize{ 600 };
    const inline int labelSize{ 150 };
    const inline int duration{ 800 };
    const inline int maxUsedNum{ 2048 };
}

#endif