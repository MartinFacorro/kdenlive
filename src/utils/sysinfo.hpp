#pragma once

class SysMemInfo
{
public:
    static SysMemInfo getMemoryInfo();
    bool isSuccessful() {
        return m_successful;
    }
    int availableMemory() {
        return m_availableMemory;
    }
    int totalMemory() {
        return m_totalMemory;
    }

private:
    bool m_successful;
    int m_availableMemory;
    int m_totalMemory;
    SysMemInfo(bool successful, int availableMemory, int totalMemory) {
        this->m_successful = successful;
        this->m_availableMemory = availableMemory;
        this->m_totalMemory = totalMemory;
    }
};
