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
 * \file JitterBuffer.cpp
 * \author Anton Runov
 * \date June 2020
 */


#include "JitterBuffer.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <cmath>

using std::cout; using std::endl;


//*******************************************************************************
JitterBuffer::JitterBuffer(int slot_size, int num_slots, int strategy) :
    RingBuffer(slot_size, num_slots)
{
    // Defaults for zero strategy
    mUnderrunIncTolerance = -1;     // never drop late packets
    mCorrIncTolerance = mNumSlots;  // never push empty blocks to correct latency
    mSimplePacketLoss = true;
    mOverflowDropStep = mNumSlots/2;

    switch (strategy) {
      case 1:
        mOverflowDropStep = 1;
        break;
      case 2:
        mSimpleUnderrun = false;
        mUnderrunIncTolerance = 1.1;
        mCorrIncTolerance = 1.2;     // should be greater than mUnderrunIncTolerance
        mSimplePacketLoss = false;
        mOverflowDropStep = 1;
        break;
    }
}

//*******************************************************************************
void JitterBuffer::insertSlotNonBlocking(const int8_t* ptrToSlot)
{
    QMutexLocker locker(&mMutex); // lock the mutex
    updateReadStats();

    if (mFullSlots == mNumSlots) {
        overflowReset();
    }

    if (-mNumSlots > mFullSlots) {
        // full reset
        mWritePosition = ( (mNumSlots/2) * mSlotSize ) % mTotalSize;
        mReadPosition = 0;
        std::memset(mRingBuffer, 0, mTotalSize);
        // Udpate Full Slots accordingly
        mBufIncUnderrun += mNumSlots/2 - mFullSlots;
        mFullSlots = mNumSlots/2;
        mLevelCur = mFullSlots;
    }
    else if (0 > mFullSlots && mLevelCur < mNumSlots - mUnderrunIncTolerance) {
        ++mFullSlots;
        ++mBufIncUnderrun;
        mLevelCur += 1.0;
    }
    else if (mLevelCur < mNumSlots - mCorrIncTolerance) {
        if (pushEmptyBlock() ) {
            mLevelCur += 1.0;
            ++mBufIncCompensate;
        }
    }
    mLevel = std::ceil(mLevelCur);


    if (0 > mFullSlots) {
        ++mOverflows;
    }
    else {
        std::memcpy(mRingBuffer+mWritePosition, ptrToSlot, mSlotSize);
        mWritePosition = (mWritePosition+mSlotSize) % mTotalSize;
    }
    mFullSlots++; //update full slots
}


//*******************************************************************************
void JitterBuffer::underrunReset()
{
    ++mUnderrunsNew;
    if (!mSimpleUnderrun) {
        --mFullSlots;
    }
}


//*******************************************************************************
void JitterBuffer::overflowReset()
{
    mReadPosition = (mReadPosition + mSlotSize*mOverflowDropStep) % mTotalSize;
    mFullSlots -= mOverflowDropStep;
    mOverflows += mOverflowDropStep;
    mBufDecOverflow += mOverflowDropStep;
    mLevelCur -= mOverflowDropStep;
    int n = qMin(mFullSlots+1, mNumSlots);
    if (n > mLevelCur+1) {
        mLevelCur = n;
    }
}


//*******************************************************************************
void JitterBuffer::processPacketLoss(int lostCount)
{
    QMutexLocker locker(&mMutex);
    mSkewRaw -= lostCount;

    if (mSimplePacketLoss) {
      mBufDecPktLoss += lostCount;
      mLevelCur -= lostCount;
      int n = qMin(mFullSlots+1, mNumSlots);
      if (n > mLevelCur) {
          mLevelCur = n;
      }
    }
    else {
        if (0 > mFullSlots) {
            int already_pushed = qMin(lostCount, -mFullSlots);
            lostCount -= already_pushed;
            mFullSlots += already_pushed;
        }
        while (0 < lostCount--) {
            if (!pushEmptyBlock()) {
              mBufDecPktLoss += lostCount+1;
              mLevelCur -= lostCount+1;
              int n = qMin(mFullSlots+1, mNumSlots);
              if (n > mLevelCur) {
                  mLevelCur = n;
              }
              break;
            }
        }
    }
}

//*******************************************************************************
bool JitterBuffer::pushEmptyBlock()
{
    if (mFullSlots >= mNumSlots-1) {
        return false;
    }
    //cout << "JitterBuffer::pushEmptyBlock" << endl;
    if (0 <= mFullSlots) {
        std::memset(mRingBuffer+mWritePosition, 0, mSlotSize);
        mWritePosition = (mWritePosition+mSlotSize) % mTotalSize;
        ++mUnderruns;
    }
    mFullSlots++;
    //mLevelCur += 1.0;
    return true;
}

