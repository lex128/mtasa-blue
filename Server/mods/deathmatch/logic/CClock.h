/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClock.h
 *  PURPOSE:     Game clock class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CCLOCK_H
#define __CCLOCK_H

class CClock
{
public:
    CClock(void);

    void Get(unsigned char& ucHour, unsigned char& ucMinute);
    void Set(unsigned char ucHour, unsigned char ucMinute);

    void          SetMinuteDuration(unsigned long ulDuration);
    unsigned long GetMinuteDuration(void);

private:
    unsigned long long m_ullMidnightTime;
    unsigned long      m_ulMinuteDuration;
};

#endif
