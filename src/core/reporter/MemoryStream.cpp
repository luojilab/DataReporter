//
// Created by bingjian on 2018/11/6.
//

#include <cstddef>
#include <Buffer.h>
#include <MiniPBCoder.h>
#include <File.h>
#include <RawOutput.h>
#include "MemoryStream.h"
#include "TimeUtil.h"
#include "StringUtil.h"
#include "IoUtil.h"

namespace future {

    MemoryStream::MemoryStream(const std::shared_ptr<Buffer> &buffer) : m_Buffer(buffer),
                                                                    m_PosPtr(NULL), m_Len(0) {
        m_PosPtr = (unsigned char *) buffer->GetBegin();
        m_Len = buffer->Length();
    }

    MemoryStream::~MemoryStream() {
    }

    void MemoryStream::Write(const std::string &data, const std::string &date) {
        std::lock_guard<std::mutex> lk(m_Mut);
        Buffer buffer = MiniPBCoder::EncodeString(data, date);
        RawOutput rawOutput(m_PosPtr, (unsigned char *) m_Buffer->GetEnd() - m_PosPtr);
        if (m_PosPtr + buffer.Length() > m_Buffer->GetEnd()) {
            return;
        }

        rawOutput.WriteRawData(buffer);
        m_PosPtr += buffer.Length();
    }

    void MemoryStream::MoveToFile(const std::string &path, void *safeBuf) {
        std::lock_guard<std::mutex> lk(m_Mut);
        long count = 0;
        unsigned char *begin = (unsigned char *) m_Buffer->GetBegin();
        count = m_PosPtr - begin;
        memcpy(safeBuf, begin, count);
        m_Buffer->Clear();
        m_PosPtr = begin;

        IoUtil::WriteFile(path, safeBuf, count);
    }

    std::int64_t MemoryStream::MoveToMem(void *mem, long maxLen) {
        std::lock_guard<std::mutex> lk(m_Mut);
        unsigned char *begin = (unsigned char *) m_Buffer->GetBegin();
        long validLen = m_PosPtr - begin;
        long copyLen = maxLen > validLen ? validLen : maxLen;
        memcpy(mem, begin, copyLen);
        memset(begin, 0, copyLen);
        m_PosPtr = begin;
        return copyLen;
    }

    std::size_t MemoryStream::GetOffset() {
        std::lock_guard<std::mutex> lk(m_Mut);
        if (m_PosPtr == NULL) {
            return 0;
        }
        return m_PosPtr - (unsigned char *) m_Buffer->GetBegin();
    }

    void MemoryStream::Seek(int offset) {
        std::lock_guard<std::mutex> lk(m_Mut);
        if (m_PosPtr == NULL) {
            return;
        }
        if (offset > 0) {
            if (m_PosPtr - (unsigned char *) m_Buffer->GetBegin() + offset >=
                m_Buffer->Length()) {
                m_PosPtr = (unsigned char *) m_Buffer->GetEnd();
            } else {
                m_PosPtr += offset;
            }
        } else {
            if (m_PosPtr + offset < m_Buffer->GetBegin()) {
                m_PosPtr = (unsigned char *) m_Buffer->GetBegin();
            } else {
                m_PosPtr += offset;
            }
        }
    }

    void MemoryStream::Clear(std::size_t offset) {
        std::lock_guard<std::mutex> lk(m_Mut);
        if (offset > m_Buffer->Length()) {
            return;
        }
        memset((unsigned char *) m_Buffer->GetBegin() + offset, 0,
               m_Buffer->Length() - offset);
    }

    size_t MemoryStream::Size(){
        return m_Buffer->Length();
    }
}