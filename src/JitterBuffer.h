//*****************************************************************
/*
  JackTrip: A System for High-Quality Audio Network Performance
  over the Internet

  Copyright (c) 2020 Juan-Pablo Caceres, Chris Chafe.
  SoundWIRE group at CCRMA, Stanford University.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/
//*****************************************************************

/**
 * \file JitterBuffer.h
 * \author Anton Runov
 * \date June 2020
 */

#ifndef __JITTERBUFFER_H__
#define __JITTERBUFFER_H__

#include "RingBuffer.h"

class JitterBuffer : public RingBuffer
{
public:
    JitterBuffer(int slot_size, int num_slots, int strategy);
    virtual ~JitterBuffer() {}

    virtual void insertSlotNonBlocking(const int8_t* ptrToSlot);
    virtual void processPacketLoss(int lostCount);

protected:
    virtual void underrunReset();
    virtual void overflowReset();

    bool pushEmptyBlock();

    double mUnderrunIncTolerance;
    double mCorrIncTolerance;
    bool   mSimplePacketLoss;
    int    mOverflowDropStep;
};


#endif //__JITTERBUFFER_H__
