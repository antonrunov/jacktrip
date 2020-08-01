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
JitterBuffer::JitterBuffer(int slot_size, int max_latency, int total_size, int strategy,
                                          int monitor_latency, int channels, int bit_res) :
    RingBuffer(0, 0)
{
    mSlotSize = slot_size;
    mMaxLatency = max_latency;
    mTotalSize = total_size;
    mMonitorLatency = monitor_latency;
    mNumChannels = channels;
    mAudioBitRes = bit_res;
    mMinStepSize = channels * bit_res;
    mActive = false;

    // Defaults for zero strategy
    mUnderrunIncTolerance = -10 * mSlotSize;
    mCorrIncTolerance = 100*mMaxLatency;     // should be greater than mUnderrunIncTolerance
    mOverflowDecTolerance = 100*mMaxLatency;
    mWritePosition = mMaxLatency;
    mStatUnit = mSlotSize;
    mLevelDownRate = 0.01*mSlotSize;
    mOverflowDropStep = mMaxLatency / 2;
    mLevelCur = mMaxLatency;
    mLevel = mLevelCur;
    mMonitorPosition = mReadPosition - mMonitorLatency;

    switch (strategy) {
      case 1:
        mOverflowDropStep = mSlotSize;
        break;
      case 2:
        mUnderrunIncTolerance = 1.1 * mSlotSize;
        mCorrIncTolerance = 1.2 * mSlotSize;     // should be greater than mUnderrunIncTolerance
        mOverflowDecTolerance = 0.02*mSlotSize;
        mOverflowDropStep = mSlotSize;
        break;
    }

    mRingBuffer = new int8_t[mTotalSize];
    std::memset(mRingBuffer, 0, mTotalSize);
}

//*******************************************************************************
bool JitterBuffer::insertSlotNonBlocking(const int8_t* ptrToSlot, int len, int lostLen)
{
    if (0 == len) {
        len = mSlotSize;
    }
    QMutexLocker locker(&mMutex);
    if (!mActive) {
        mActive = true;
    }
    if (mMaxLatency < len + mSlotSize) {
        mMaxLatency = len + mSlotSize;
    }
    if (0 < lostLen) {
        processPacketLoss(lostLen);
    }
    mSkewRaw += mReadsNew - len;
    mReadsNew = 0;
    mUnderruns += mUnderrunsNew;
    mUnderrunsNew = 0;
    mLevel = mSlotSize*std::ceil(mLevelCur/mSlotSize);

    // Update positions if necessary
    int32_t available = mWritePosition - mReadPosition;
    int delta = 0;
    if (available < -5*(mSlotSize+len)) {
        delta = available;
        mBufIncUnderrun += -delta;
        //cout << "reset" << endl;
    }
    else if (available + len > mMaxLatency) {
        delta = mOverflowDropStep;
        mOverflows += delta;
        mBufDecOverflow += delta;
    }
    else if (0 > available && mLevelCur < mMaxLatency - mUnderrunIncTolerance) {
        delta = -qMin(-available, mSlotSize);
        mBufIncUnderrun += -delta;
    }
    else if (mLevelCur < mMaxLatency - mCorrIncTolerance) {
        delta = -mSlotSize;
        mUnderruns += -delta;
        mBufIncCompensate += -delta;
    }

    if (0 != delta) {
        mReadPosition += delta;
        mLevelCur -= delta;
        if (mLevelCur > mMaxLatency) {
            mLevelCur = mMaxLatency;
        }
    }

    int wpos = mWritePosition % mTotalSize;
    int n = qMin(mTotalSize - wpos, len);
    std::memcpy(mRingBuffer+wpos, ptrToSlot, n);
    if (n < len) {
        //cout << "split write: " << len << "-" << n << endl;
        std::memcpy(mRingBuffer, ptrToSlot+n, len-n);
    }
    mWritePosition += len;

    return true;
}

//*******************************************************************************
void JitterBuffer::readSlotNonBlocking(int8_t* ptrToReadSlot)
{
    int len = mSlotSize;
    QMutexLocker locker(&mMutex);
    if (!mActive) {
        std::memset(ptrToReadSlot, 0, len);
        return;
    }
    mReadsNew += len;
    int32_t available = mWritePosition - mReadPosition;
    if (available < mLevelCur) {
        mLevelCur = qMax((double)available, mLevelCur-mLevelDownRate);
    }
    else {
        mLevelCur = available;
    }
    int read_len = qBound(0, available, len);
    int rpos = mReadPosition % mTotalSize;
    int n = qMin(mTotalSize - rpos, read_len);
    std::memcpy(ptrToReadSlot, mRingBuffer+rpos, n);
    if (n < read_len) {
        //cout << "split read: " << read_len << "-" << n << endl;
        std::memcpy(ptrToReadSlot+n, mRingBuffer, read_len-n);
    }
    if (read_len < len) {
        std::memset(ptrToReadSlot+read_len, 0, len-read_len);
        mUnderrunsNew += len-read_len;
    }
    mReadPosition += len;
}

//*******************************************************************************
void JitterBuffer::readMonitorSlot(int8_t* ptrToReadSlot)
{
    int len = mSlotSize;
    QMutexLocker locker(&mMutex);
    if (0 == mReadPosition) {
        std::memset(ptrToReadSlot, 0, len);
        return;
    }
    // latency correction
    int32_t d = mReadPosition - mMonitorLatency - mMonitorPosition - len;
    if (qAbs(d) > 2*mSlotSize) {
        mMonitorPosition = mReadPosition - mMonitorLatency - len;
        mMonitorPositionCorr = 0.0;
    }
    else {
        mMonitorPositionCorr += 0.0003 * d;
        int delta = mMonitorPositionCorr / mMinStepSize;
        if (0 != delta) {
            mMonitorPositionCorr -= delta * mMinStepSize;
            if (2 == mAudioBitRes) {
                // interpolate
                len += delta * mMinStepSize;
            }
            else {
                // skip
                mMonitorPosition += delta * mMinStepSize;
            }
            mMonitorSkew += delta;
        }
    }
    mMonitorDelta = d / mMinStepSize;
    int32_t available = mWritePosition - mMonitorPosition;
    int read_len = qBound(0, available, len);
    if (len == mSlotSize) {
        int rpos = mMonitorPosition % mTotalSize;
        int n = qMin(mTotalSize - rpos, read_len);
        std::memcpy(ptrToReadSlot, mRingBuffer+rpos, n);
        if (n < read_len) {
            //cout << "split read: " << read_len << "-" << n << endl;
            std::memcpy(ptrToReadSlot+n, mRingBuffer, read_len-n);
        }
        if (read_len < len) {
            std::memset(ptrToReadSlot+read_len, 0, len-read_len);
        }
    }
    else {
        // interpolation len => mSlotSize
        double K = 1.0 * len / mSlotSize;
        for (int c=0; c < mMinStepSize; c+=sizeof(int16_t)) {
            for (int j=0; j < mSlotSize/mMinStepSize; ++j) {
                int j1 = std::floor(j*K);
                double a = j*K - j1;
                int rpos = (mMonitorPosition + j1*mMinStepSize + c) % mTotalSize;
                int16_t v1 = *(int16_t*)(mRingBuffer + rpos);
                rpos = (rpos + mMinStepSize) % mTotalSize;
                int16_t v2 = *(int16_t*)(mRingBuffer + rpos);
                *(int16_t*)(ptrToReadSlot + j*mMinStepSize + c) = std::round((1-a)*v1 + a*v2);
            }
        }
    }
    mMonitorPosition += len;
}


//*******************************************************************************
void JitterBuffer::processPacketLoss(int lostLen)
{
    mSkewRaw -= lostLen;

    int32_t available = mWritePosition - mReadPosition;
    int delta = qMin(available + lostLen - mMaxLatency, lostLen);
    if (0 < delta) {
        lostLen -= delta;
        mBufDecPktLoss += delta;
        mLevelCur -= delta;
    }
    else if (mLevelCur > mMaxLatency - mOverflowDecTolerance) {
        delta = qMin(lostLen, mSlotSize);
        lostLen -= delta;
        mBufDecPktLoss += delta;
        mLevelCur -= delta;
    }
    if (lostLen >= mTotalSize) {
        std::memset(mRingBuffer, 0, mTotalSize);
        mUnderruns += lostLen;
    }
    else if (0 < lostLen) {
        int wpos = mWritePosition % mTotalSize;
        int n = qMin(mTotalSize - wpos, lostLen);
        std::memset(mRingBuffer+wpos, 0, n);
        if (n < lostLen) {
            //cout << "split write: " << lostLen << "-" << n << endl;
            std::memset(mRingBuffer, 0, lostLen-n);
        }
        mUnderruns += lostLen;
    }
    mWritePosition += lostLen;
}

